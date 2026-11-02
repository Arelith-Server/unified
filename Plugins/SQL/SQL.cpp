#include "SQL.hpp"
#include "Targets/MySQL.hpp"
#include "Targets/PostgreSQL.hpp"
#include "Targets/SQLite.hpp"
#include "API/Globals.hpp"
#include "API/Constants.hpp"
#include "API/CAppManager.hpp"
#include "API/CServerExoApp.hpp"
#include "API/CNWSItem.hpp" // Needed for static_cast from CGameObject
#include "API/CNWSModule.hpp"
#include <algorithm>
#include <chrono>
#include <thread>
#include <cstring>
#include <dlfcn.h>

using namespace NWNXLib;

namespace Core {
    extern bool g_CoreShuttingDown;
}

static SQL::SQL* g_plugin;

NWNX_PLUGIN_ENTRY Plugin* PluginLoad(Services::ProxyServiceList* services)
{
    g_plugin = new SQL::SQL(services);
    return g_plugin;
}

using namespace NWNXLib::Services;

namespace SQL {

SQL::SQL(Services::ProxyServiceList* services)
    : Plugin(services), m_nextQueryId(0), m_queryMetrics(false), m_queryPrepared(false),
      m_asyncQueueLimit(10000), m_asyncShutdownTimeout(10000), m_nextAsyncJobId(0), m_asyncPumpQueued(false)
{

#define REGISTER(func) \
    ScriptAPI::RegisterEvent(PLUGIN_NAME, #func, \
        [this](ArgumentStack&& args){ return func(std::move(args)); })

    REGISTER(PrepareQuery);
    REGISTER(ExecutePreparedQuery);
    REGISTER(ReadyToReadNextRow);
    REGISTER(ReadNextRow);
    REGISTER(ReadDataInActiveRow);
    REGISTER(PreparedInt);
    REGISTER(PreparedString);
    REGISTER(PreparedFloat);
    REGISTER(PreparedObjectId);
    REGISTER(PreparedObjectFull);
    REGISTER(PreparedNULL);
    REGISTER(ReadFullObjectInActiveRow);
    REGISTER(GetAffectedRows);
    REGISTER(GetDatabaseType);
    REGISTER(DestroyPreparedQuery);
    REGISTER(GetLastError);
    REGISTER(GetPreparedQueryParamCount);
    REGISTER(PushAsyncParam);
    REGISTER(PushAsyncParamInt);
    REGISTER(PushAsyncParamFloat);
    REGISTER(PushAsyncParamNULL);
    REGISTER(ExecuteAsync);
    REGISTER(GetAsyncJobId);
    REGISTER(GetAsyncSuccess);
    REGISTER(GetAsyncAffectedRows);
    REGISTER(GetAsyncLastError);
    REGISTER(AsyncReadyToReadNextRow);
    REGISTER(AsyncReadNextRow);
    REGISTER(AsyncReadDataInActiveRow);
    REGISTER(AsyncFlush);
    REGISTER(GetAsyncQueueDepth);

#undef REGISTER

    m_queryMetrics = Config::Get<bool>("QUERY_METRICS", false);

    if (m_queryMetrics)
    {
        Resamplers::ResamplerFuncPtr sum = &Resamplers::template Sum<int64_t>;
        GetServices()->m_metrics->SetResampler("SQLQueries", sum, std::chrono::seconds(1));

        // Worst wait, run time and queue depth of the async jobs finished in each second.
        Resamplers::ResamplerFuncPtr max = &Resamplers::template Max<int64_t>;
        GetServices()->m_metrics->SetResampler("SQLAsync", max, std::chrono::seconds(1));
    }

    m_databaseType = Config::Get<std::string>("TYPE", "MYSQL");
    std::transform(std::begin(m_databaseType), std::end(m_databaseType), std::begin(m_databaseType), ::toupper);

    LOG_INFO("Connecting to type %s", m_databaseType);
    m_target = CreateTarget();

    m_utf8 = Config::Get<bool>("USE_UTF8", false);

    Reconnect(19);

    m_asyncQueueLimit = static_cast<size_t>(std::max(1, Config::Get<int32_t>("ASYNC_QUEUE_LIMIT", 10000)));
    m_asyncShutdownTimeout = std::chrono::seconds(std::max(0, Config::Get<int32_t>("ASYNC_SHUTDOWN_TIMEOUT", 10)));

    // Completions are handed to the core's main-thread queue as std::function objects whose code
    // lives in this library, and a worker that can't be joined at shutdown is detached. Keep the
    // library mapped when NWNX unloads plugins so neither can end up running unmapped code.
    Dl_info info;
    if (dladdr(reinterpret_cast<void*>(&PluginLoad), &info) && info.dli_fname)
        dlopen(info.dli_fname, RTLD_NOW | RTLD_NOLOAD | RTLD_NODELETE);

    if (Config::Get<bool>("ASYNC_DISABLE", false))
    {
        LOG_NOTICE("Async SQL disabled by NWNX_SQL_ASYNC_DISABLE: async calls run synchronously.");
    }
    else if (m_databaseType != "MYSQL")
    {
        LOG_NOTICE("The async SQL worker only supports MySQL: async calls run synchronously.");
    }
    else
    {
        LOG_INFO("Starting the async SQL worker (queue limit %zu, shutdown timeout %lld s).",
            m_asyncQueueLimit, static_cast<long long>(
                std::chrono::duration_cast<std::chrono::seconds>(m_asyncShutdownTimeout).count()));
        m_asyncWorker = std::make_unique<AsyncWorker>(CreateTarget(),
            [this](AsyncJobPtr job){ PostAsyncCompletion(std::move(job)); });
    }
}

SQL::~SQL()
{
    if (m_asyncWorker)
    {
        using namespace std::chrono;
        const size_t depth = m_asyncWorker->GetDepth();
        LOG_INFO("Stopping the async SQL worker: %zu job(s) to drain, timeout %lld s.", depth,
            static_cast<long long>(duration_cast<seconds>(m_asyncShutdownTimeout).count()));

        const auto start = steady_clock::now();
        bool joined = true;
        auto left = m_asyncWorker->Shutdown(m_asyncShutdownTimeout, [this](uint64_t connectionId)
        {
            // Runs on the main thread, on the main connection: break off the worker's query.
            if (connectionId && m_target->IsConnected() &&
                m_target->PrepareQuery("KILL " + std::to_string(connectionId)))
            {
                m_target->ExecuteQuery();
                m_target->DestroyPreparedQuery();
            }
        }, joined);
        const auto took = duration_cast<milliseconds>(steady_clock::now() - start);

        if (left.empty())
        {
            LOG_INFO("Async SQL worker stopped after %lld ms; queue drained.", static_cast<long long>(took.count()));
        }
        else
        {
            LOG_ERROR("Async SQL worker stopped after %lld ms; %zu queued job(s) were NOT run:",
                static_cast<long long>(took.count()), left.size());
            size_t logged = 0;
            for (const auto& job : left)
            {
                if (++logged > 100)
                {
                    LOG_ERROR("  ... and %zu more.", left.size() - 100);
                    break;
                }
                LOG_ERROR("  Job ID '%i': %s", job->id, job->query);
            }
        }

        if (joined)
            m_asyncWorker.reset();
        else
            (void)m_asyncWorker.release(); // still running; leaked on purpose
    }

    std::lock_guard<std::mutex> lock(m_asyncCompletedLock);
    if (!m_asyncCompleted.empty())
        LOG_DEBUG("%zu async SQL callback(s) not run because the server is shutting down.", m_asyncCompleted.size());
    g_plugin = nullptr;
}

std::unique_ptr<ITarget> SQL::CreateTarget()
{
    if (m_databaseType == "MYSQL")
    {
#if defined(NWNX_SQL_MYSQL_SUPPORT)
        return std::make_unique<MySQL>();
#else
        throw std::runtime_error("Targeting MySQL, but no MySQL support built in.");
#endif
    }
    else if (m_databaseType == "POSTGRESQL")
    {
#if defined(NWNX_SQL_POSTGRESQL_SUPPORT)
        return std::make_unique<PostgreSQL>();
#else
        throw std::runtime_error("Targeting PostgreSQL, but no PostgreSQL support built in.");
#endif
    }
    else if (m_databaseType == "SQLITE")
    {
        return std::make_unique<SQLite>();
    }
    throw std::runtime_error("Invalid database type selected.");
}

bool SQL::Reconnect(int32_t attempts)
{
    LOG_WARNING("Database connection lost. Reconnecting..");

    for (int32_t i = 0; i < attempts; i++)
    {
        try
        {
            m_target->Connect();
            LOG_NOTICE("Reconnect successful.");
            break;
        }
        catch (std::runtime_error& e)
        {
            LOG_ERROR("Reconnect attempt %d out of %d failed: %s",
                i+1, attempts, e.what());

            // NOTE: We are on the main thread and cannot sleep for to long, as
            // it will stall the entire server. If a reconnect fails here, the
            // user requested operation will fail as well. It is then up to the
            // user to retry later (e.g. Use DelayCommand()), and another
            // reconnect attempt will be triggered automatically.
            if (i != attempts - 1)
                std::this_thread::sleep_for(std::chrono::milliseconds(1 << i));
        }
    }
    return m_target->IsConnected();
}

ArgumentStack SQL::PrepareQuery(ArgumentStack&& args)
{
    m_activeQuery = args.extract<std::string>();

    if (m_utf8)
    {
        m_activeQuery = String::ToUTF8(m_activeQuery);
    }

    m_activeResults = ResultSet();

    if (!m_target->IsConnected() && !Reconnect(3))
    {
        LOG_ERROR("Database connection lost. Aborting.");
        return false;
    }

    m_queryPrepared = m_target->PrepareQuery(m_activeQuery);

    // IsConnected() can be a cached state, so a dropped connection may only show up here.
    // Preparing has no side effects, so reconnect and try once more.
    if (!m_queryPrepared && !m_target->IsConnected() && Reconnect(3))
    {
        m_target->GetLastError(true); // the failed attempt's message no longer applies
        m_queryPrepared = m_target->PrepareQuery(m_activeQuery);
    }
    return m_queryPrepared;
}

ArgumentStack SQL::ExecutePreparedQuery(ArgumentStack&&)
{
    if (!m_queryPrepared)
    {
        LOG_WARNING("Trying to execute prepared query without successful PrepareQuery() call");
        return 0;
    }

    // NOTE: For MySQL, IsConnected() only knows about connection errors seen so far, so a connection
    // that dropped since the last call still reads as connected. The MySQL target then reconnects and
    // runs the statement again only when it provably never reached the server; otherwise the call
    // fails. It is up to the user to check the return value, and repeat the query if needed.
    if (!m_target->IsConnected())
    {
        LOG_DEBUG("Not Connected");
        if (!Reconnect())
        {
            LOG_ERROR("Database connection lost. Aborting.");
            return 0;
        }
        else
        {
            // Prepared queries are lost on reconnect.
            // Prepared arguments are not, however, so we can still recover
            if (!m_target->PrepareQuery(m_activeQuery))
            {
                LOG_ERROR("Recovery PrepareQuery() failed: %s", m_target->GetLastError());
                return 0;
            }

        }
    }

    const int32_t queryId = ++m_nextQueryId;

    std::optional<ResultSet> query;

    if (m_queryMetrics)
    {
        const auto timeBefore = std::chrono::high_resolution_clock::now();
        query = m_target->ExecuteQuery();
        const auto timeAfter = std::chrono::high_resolution_clock::now();

        using namespace std::chrono;
        nanoseconds dur = duration_cast<nanoseconds>(timeAfter - timeBefore);

        GetServices()->m_metrics->Push(
            "SQLQueries",
            { { "ns", std::to_string(dur.count()) } },
            { { "ID", std::to_string(queryId) } });
    }
    else
    {
        query = m_target->ExecuteQuery();
    }

    const bool querySucceeded = query.has_value();

    m_activeResults = query.value_or(ResultSet());

    if (querySucceeded)
    {
        // queries that execute commands return the number of affected rows.
        // queries that fetch results return a results size.
        if (m_target->GetAffectedRows() >= 0)
        {
            // this was not a result set type query
            LOG_INFO("Successful SQL query. Query ID: '%i', Query: '%s', Rows affected: '%u'.",
                queryId, m_activeQuery, m_target->GetAffectedRows());
        }
        else
        {
            LOG_INFO("Successful SQL query. Query ID: '%i', Query: '%s', Results Count: '%u'.",
                queryId, m_activeQuery, m_activeResults.size());
        }
    }
    else
    {
        LOG_WARNING("Failed SQL query. Query ID: '%i', Query: '%s'.", queryId, m_activeQuery);
        std::string lastError = m_target->GetLastError();
        LOG_WARNING("Failure Message. Query ID: '%i', \"%s\"", queryId, lastError);
    }

    return querySucceeded ? queryId : 0;
}

ArgumentStack SQL::ReadyToReadNextRow(ArgumentStack&&)
{
    return m_activeResults.empty() ? 0 : 1;
}

ArgumentStack SQL::ReadNextRow(ArgumentStack&&)
{
    if (m_activeResults.empty())
    {
        throw std::runtime_error("No more rows to read.");
    }

    m_activeRow = std::move(m_activeResults.front());
    m_activeResults.pop();
    return {};
}

ArgumentStack SQL::ReadDataInActiveRow(ArgumentStack&& args)
{
    const size_t column = args.extract<int32_t>();

    if (column >= m_activeRow.size())
    {
        throw std::runtime_error("Trying to access column outside of range.");
    }

    return ScriptAPI::Arguments(m_utf8 ? String::FromUTF8(m_activeRow[column]) : m_activeRow[column]);
}
ArgumentStack SQL::PreparedInt(ArgumentStack&& args)
{
    const auto position = args.extract<int32_t>();
    const auto value = args.extract<int32_t>();
    if (position >= m_target->GetPreparedQueryParamCount())
    {
        LOG_WARNING("Prepared argument (pos:%d, value:0x%08x) out of bounds", position, value);
    }
    else
    {
        m_target->PrepareInt(position, value);
    }
    return {};
}
ArgumentStack SQL::PreparedString(ArgumentStack&& args)
{
    const auto position = args.extract<int32_t>();
    const auto value = args.extract<std::string>();
    if (position >= m_target->GetPreparedQueryParamCount())
    {
        LOG_WARNING("Prepared argument (pos:%d, value:'%s') out of bounds", position, value);
    }
    else
    {
        m_target->PrepareString(position, m_utf8 ? String::ToUTF8(value) : value);
    }
    return {};
}
ArgumentStack SQL::PreparedFloat(ArgumentStack&& args)
{
    const auto position = args.extract<int32_t>();
    const auto value = args.extract<float>();
    if (position >= m_target->GetPreparedQueryParamCount())
    {
        LOG_WARNING("Prepared argument (pos:%d, value:'%f') out of bounds", position, value);
    }
    else
    {
        m_target->PrepareFloat(position, value);
    }
    return {};
}
ArgumentStack SQL::PreparedObjectId(ArgumentStack&& args)
{
    auto position = args.extract<int32_t>();
    auto value = args.extract<ObjectID>();
    int32_t valInt;
    std::memcpy(&valInt, &value, sizeof(valInt)); static_assert(sizeof(valInt) == sizeof(value));
    if (position >= m_target->GetPreparedQueryParamCount())
    {
        LOG_WARNING("Prepared argument (pos:%d, value:ObjID-%08x) out of bounds", position, valInt);
    }
    else
    {
        m_target->PrepareInt(position, valInt);
    }
    return {};
}
ArgumentStack SQL::PreparedObjectFull(ArgumentStack&& args)
{
    auto position = args.extract<int32_t>();
    auto value = args.extract<ObjectID>();
    bool base64 = !!args.extract<int32_t>();

    if (position >= m_target->GetPreparedQueryParamCount())
    {
        LOG_WARNING("Prepared argument (pos:%d, value:ObjID-%08x) out of bounds", position, static_cast<int32_t>(value));
    }
    else
    {
        CGameObject *pObject = API::Globals::AppManager()->m_pServerExoApp->GetGameObject(value);
        if (base64) {
            std::string serializedObject = Utils::SerializeGameObjectB64(pObject);
            m_target->PrepareString(position, serializedObject);
        } else {
            std::vector<uint8_t> serializedObjectVec = Utils::SerializeGameObject(pObject);
            m_target->PrepareBinary(position, serializedObjectVec);
        }
    }
    return {};
}
ArgumentStack SQL::PreparedNULL(ArgumentStack&& args)
{
    auto position = args.extract<int32_t>();

    if (position >= m_target->GetPreparedQueryParamCount())
    {
        LOG_WARNING("Prepared argument (pos:%d, value:NULL) out of bounds", position);
    }
    else
    {
        m_target->PrepareNULL(position);
    }
    return {};
}

ArgumentStack SQL::ReadFullObjectInActiveRow(ArgumentStack&& args)
{
    const size_t column = args.extract<int32_t>();
    const auto owner = args.extract<ObjectID>();
    const auto x = args.extract<float>();
    const auto y = args.extract<float>();
    const auto z = args.extract<float>();
    const bool base64 = !!args.extract<int32_t>();

    if (column >= m_activeRow.size())
    {
        throw std::runtime_error("Trying to access column outside of range.");
    }

    std::string serialized = m_activeRow[column];
    ObjectID retval = API::Constants::OBJECT_INVALID;
    CGameObject *pObject = base64 ? Utils::DeserializeGameObjectB64(serialized) : Utils::DeserializeGameObject(std::vector<uint8_t>(serialized.begin(), serialized.end()));
    if (pObject)
    {
        retval = static_cast<ObjectID>(pObject->m_idSelf);
        ASSERT(API::Globals::AppManager()->m_pServerExoApp->GetGameObject(retval));

        CGameObject *pOwner = API::Globals::AppManager()->m_pServerExoApp->GetGameObject(owner);
        if (auto *pArea = Utils::AsNWSArea(pOwner))
        {
            if (!Utils::AddToArea(pObject, pArea, x, y, z))
                LOG_WARNING("Failed to add object %x to area %x (%f,%f,%f)", retval, owner, x, y, z);
        }
        else if (auto *pItem = Utils::AsNWSItem(pObject))
        {
            if (!Utils::AcquireItem(pItem, pOwner))
                LOG_WARNING("Failed to 'acquire' deserialized item %x by owner %x", retval, owner);
        }
        else
        {
            LOG_INFO("No valid owner given, object %x deserialized outside world bounds", retval);
        }
    }
    return retval;
}

ArgumentStack SQL::GetAffectedRows(ArgumentStack&&)
{
    return m_target->GetAffectedRows();
}

ArgumentStack SQL::GetDatabaseType(ArgumentStack&&)
{
    return m_databaseType;
}

ArgumentStack SQL::DestroyPreparedQuery(ArgumentStack&&)
{
    m_target->DestroyPreparedQuery();
    m_queryPrepared = false;
    return {};
}

ArgumentStack SQL::GetLastError(ArgumentStack&&)
{
    return m_target->GetLastError(true);
}

ArgumentStack SQL::GetPreparedQueryParamCount(ArgumentStack&&)
{
    return m_queryPrepared ? m_target->GetPreparedQueryParamCount() : -1;
}

// ---------------------------------------------------------------------------------------------
// Async API. Everything below runs on the main thread, except PostAsyncCompletion.

ArgumentStack SQL::PushAsyncParam(ArgumentStack&& args)
{
    AsyncParam param;
    param.type = AsyncParam::Type::String;
    param.s = args.extract<std::string>();
    if (m_utf8)
        param.s = String::ToUTF8(param.s);
    m_asyncParams.push_back(std::move(param));
    return {};
}

ArgumentStack SQL::PushAsyncParamInt(ArgumentStack&& args)
{
    AsyncParam param;
    param.type = AsyncParam::Type::Int;
    param.n = args.extract<int32_t>();
    m_asyncParams.push_back(std::move(param));
    return {};
}

ArgumentStack SQL::PushAsyncParamFloat(ArgumentStack&& args)
{
    AsyncParam param;
    param.type = AsyncParam::Type::Float;
    param.f = args.extract<float>();
    m_asyncParams.push_back(std::move(param));
    return {};
}

ArgumentStack SQL::PushAsyncParamNULL(ArgumentStack&&)
{
    m_asyncParams.emplace_back();
    return {};
}

ArgumentStack SQL::ExecuteAsync(ArgumentStack&& args)
{
    auto query = args.extract<std::string>();
    auto callbackScript = args.extract<std::string>();
    const auto callbackTarget = args.extract<ObjectID>();

    auto job = std::make_shared<AsyncJob>();
    job->params.swap(m_asyncParams); // pushed parameters belong to this call, whatever happens

    if (query.empty())
    {
        LOG_WARNING("ExecuteAsync called with an empty query.");
        return 0;
    }

    m_nextAsyncJobId = (m_nextAsyncJobId == INT32_MAX) ? 1 : m_nextAsyncJobId + 1;
    job->id = m_nextAsyncJobId;
    job->query = m_utf8 ? String::ToUTF8(query) : std::move(query);
    job->callbackScript = std::move(callbackScript);
    job->callbackTarget = callbackTarget;
    job->queuedAt = std::chrono::steady_clock::now();

    if (m_asyncWorker)
    {
        if (m_asyncWorker->Enqueue(job, m_asyncQueueLimit))
            return job->id;

        // Backpressure: past the cap the job runs here, on the main connection. Warn at most once a second.
        static auto lastWarning = std::chrono::steady_clock::time_point();
        static size_t overflowed = 0;
        overflowed++;
        if (job->queuedAt - lastWarning >= std::chrono::seconds(1))
        {
            LOG_WARNING("Async SQL queue is full (limit %zu): %zu job(s) run synchronously since the last warning, latest job ID '%i'.",
                m_asyncQueueLimit, overflowed, job->id);
            lastWarning = job->queuedAt;
            overflowed = 0;
        }
    }

    RunAsyncJobSync(job);
    return job->id;
}

void SQL::RunAsyncJobSync(const AsyncJobPtr& job)
{
    job->ranSync = true;
    RunJob(*m_target, *job, [this]{ return Reconnect(3); });
    // The job used the main connection's statement, so nothing prepared through the sync API is left.
    m_queryPrepared = false;
    // Callbacks always run on a later tick, never inside ExecuteAsync, sync or not.
    PostAsyncCompletion(job);
}

// Any thread.
void SQL::PostAsyncCompletion(AsyncJobPtr job)
{
    bool post = false;
    {
        std::lock_guard<std::mutex> lock(m_asyncCompletedLock);
        m_asyncCompleted.push_back(std::move(job));
        if (!m_asyncPumpQueued)
            post = m_asyncPumpQueued = true;
    }
    if (post)
    {
        // One pending main-thread task at a time, however many jobs finish before the next tick.
        Tasks::QueueOnMainThread([]
        {
            if (g_plugin && !Core::g_CoreShuttingDown)
                g_plugin->RunAsyncCompletions();
        });
    }
}

void SQL::RunAsyncCompletions()
{
    std::deque<AsyncJobPtr> done;
    {
        std::lock_guard<std::mutex> lock(m_asyncCompletedLock);
        done.swap(m_asyncCompleted);
        m_asyncPumpQueued = false;
    }

    const size_t depth = m_asyncWorker ? m_asyncWorker->GetDepth() : 0;
    using namespace std::chrono;

    for (auto& job : done)
    {
        if (job->success)
        {
            if (job->affectedRows >= 0)
                LOG_INFO("Successful async SQL query. Job ID: '%i', Query: '%s', Rows affected: '%d'.",
                    job->id, job->query, job->affectedRows);
            else
                LOG_INFO("Successful async SQL query. Job ID: '%i', Query: '%s', Results Count: '%zu'.",
                    job->id, job->query, job->results.size());
        }
        else
        {
            LOG_WARNING("Failed async SQL query. Job ID: '%i', Query: '%s', Error: \"%s\"",
                job->id, job->query, job->error);
        }

        LOG_DEBUG("Async SQL job '%i': waited %lld us, ran %lld us%s; queue depth %zu.", job->id,
            static_cast<long long>(duration_cast<microseconds>(job->waitTime).count()),
            static_cast<long long>(duration_cast<microseconds>(job->execTime).count()),
            job->ranSync ? " (synchronously)" : "", depth);

        if (m_queryMetrics)
        {
            GetServices()->m_metrics->Push("SQLAsync",
                { { "wait_ns", std::to_string(job->waitTime.count()) },
                  { "exec_ns", std::to_string(job->execTime.count()) },
                  { "depth",   std::to_string(depth) } });
        }

        if (job->callbackScript.empty())
            continue;

        ObjectID target = job->callbackTarget;
        if (target == API::Constants::OBJECT_INVALID)
        {
            auto* pModule = Utils::GetModule();
            target = pModule ? static_cast<ObjectID>(pModule->m_idSelf) : API::Constants::OBJECT_INVALID;
        }
        if (target == API::Constants::OBJECT_INVALID || !Utils::GetGameObject(target))
        {
            LOG_DEBUG("Async SQL job '%i': callback '%s' skipped, object %x no longer exists.",
                job->id, job->callbackScript, job->callbackTarget);
            continue;
        }

        // The callback reads this job's own rows, never the sync API's.
        auto previousJob = std::move(m_asyncCurrent);
        auto previousRow = std::move(m_asyncActiveRow);
        m_asyncCurrent = job;
        m_asyncActiveRow.clear();

        Utils::ExecuteScript(job->callbackScript, target);

        m_asyncCurrent = std::move(previousJob);
        m_asyncActiveRow = std::move(previousRow);
    }
}

ArgumentStack SQL::GetAsyncJobId(ArgumentStack&&)
{
    return m_asyncCurrent ? m_asyncCurrent->id : 0;
}

ArgumentStack SQL::GetAsyncSuccess(ArgumentStack&&)
{
    return (m_asyncCurrent && m_asyncCurrent->success) ? 1 : 0;
}

ArgumentStack SQL::GetAsyncAffectedRows(ArgumentStack&&)
{
    return m_asyncCurrent ? m_asyncCurrent->affectedRows : -1;
}

ArgumentStack SQL::GetAsyncLastError(ArgumentStack&&)
{
    return m_asyncCurrent ? m_asyncCurrent->error : std::string();
}

ArgumentStack SQL::AsyncReadyToReadNextRow(ArgumentStack&&)
{
    return (m_asyncCurrent && !m_asyncCurrent->results.empty()) ? 1 : 0;
}

ArgumentStack SQL::AsyncReadNextRow(ArgumentStack&&)
{
    if (!m_asyncCurrent || m_asyncCurrent->results.empty())
    {
        throw std::runtime_error("No more async rows to read.");
    }

    m_asyncActiveRow = std::move(m_asyncCurrent->results.front());
    m_asyncCurrent->results.pop();
    return {};
}

ArgumentStack SQL::AsyncReadDataInActiveRow(ArgumentStack&& args)
{
    const size_t column = args.extract<int32_t>();

    if (column >= m_asyncActiveRow.size())
    {
        throw std::runtime_error("Trying to access column outside of range.");
    }

    return ScriptAPI::Arguments(m_utf8 ? String::FromUTF8(m_asyncActiveRow[column]) : m_asyncActiveRow[column]);
}

ArgumentStack SQL::AsyncFlush(ArgumentStack&& args)
{
    const auto timeout = args.extract<float>();
    if (!m_asyncWorker)
        return 1;

    using namespace std::chrono;
    const auto wait = milliseconds(static_cast<int64_t>(std::max(0.0f, timeout) * 1000.0f));
    const auto start = steady_clock::now();
    const bool flushed = m_asyncWorker->Flush(wait);
    const auto waited = duration_cast<milliseconds>(steady_clock::now() - start);

    if (flushed)
        LOG_DEBUG("Async SQL queue flushed in %lld ms.", static_cast<long long>(waited.count()));
    else
        LOG_WARNING("Async SQL flush timed out after %lld ms; %zu job(s) still queued or running.",
            static_cast<long long>(waited.count()), m_asyncWorker->GetDepth());
    return flushed ? 1 : 0;
}

ArgumentStack SQL::GetAsyncQueueDepth(ArgumentStack&&)
{
    return m_asyncWorker ? static_cast<int32_t>(m_asyncWorker->GetDepth()) : 0;
}

}
