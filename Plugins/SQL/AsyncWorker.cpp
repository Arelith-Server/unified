#include "AsyncWorker.hpp"

#include <algorithm>

using namespace NWNXLib;
using namespace std::chrono;

namespace SQL {

void RunJob(ITarget& target, AsyncJob& job, const std::function<bool()>& reconnect)
{
    const auto start = steady_clock::now();
    job.waitTime = duration_cast<nanoseconds>(start - job.queuedAt);
    job.success = false;
    job.affectedRows = -1;
    job.results = ResultSet();
    job.error.clear();

    try
    {
        if (!target.IsConnected() && !reconnect())
        {
            job.error = "Database connection lost";
        }
        else
        {
            target.GetLastError(true);
            bool prepared = target.PrepareQuery(job.query);

            // IsConnected() is a cached state, so a dropped connection may only show up here.
            // Preparing has no side effects, so reconnect and try once more.
            if (!prepared && !target.IsConnected() && reconnect())
            {
                target.GetLastError(true);
                prepared = target.PrepareQuery(job.query);
            }

            if (!prepared)
            {
                job.error = target.GetLastError(true);
            }
            else if (target.GetPreparedQueryParamCount() != static_cast<int32_t>(job.params.size()))
            {
                job.error = "Query expects " + std::to_string(target.GetPreparedQueryParamCount()) +
                    " parameters, but " + std::to_string(job.params.size()) + " were pushed";
            }
            else
            {
                for (size_t i = 0; i < job.params.size(); i++)
                {
                    const auto& param = job.params[i];
                    const auto pos = static_cast<int32_t>(i);
                    switch (param.type)
                    {
                        case AsyncParam::Type::String: target.PrepareString(pos, param.s); break;
                        case AsyncParam::Type::Int:    target.PrepareInt(pos, param.n);    break;
                        case AsyncParam::Type::Float:  target.PrepareFloat(pos, param.f);  break;
                        case AsyncParam::Type::Null:   target.PrepareNULL(pos);            break;
                    }
                }

                // The target itself retries once on "server has gone away" (2006) and never on
                // "lost connection during query" (2013), where the statement may already have run.
                if (auto results = target.ExecuteQuery())
                {
                    job.success = true;
                    job.results = std::move(*results);
                    job.affectedRows = target.GetAffectedRows();
                }
                else
                {
                    job.error = target.GetLastError(true);
                }
            }
            target.DestroyPreparedQuery();
        }
    }
    catch (const std::exception& e)
    {
        job.success = false;
        job.error = e.what();
    }

    if (!job.success && job.error.empty())
        job.error = "Undefined/unknown";

    job.execTime = duration_cast<nanoseconds>(steady_clock::now() - start);
}

AsyncWorker::AsyncWorker(std::unique_ptr<ITarget> target, std::function<void(AsyncJobPtr)> onComplete)
    : m_target(std::move(target)), m_onComplete(std::move(onComplete))
{
    m_thread = std::thread([this]{ Run(); });
}

AsyncWorker::~AsyncWorker()
{
    if (m_thread.joinable())
    {
        bool joined;
        Shutdown(milliseconds(0), [](uint64_t){}, joined);
    }
}

bool AsyncWorker::Enqueue(AsyncJobPtr job, size_t maxDepth)
{
    {
        std::lock_guard<std::mutex> lock(m_lock);
        if (!m_accepting || m_exited || m_queue.size() >= maxDepth)
            return false;
        m_queue.push_back(std::move(job));
    }
    m_wake.notify_one();
    return true;
}

bool AsyncWorker::Flush(milliseconds timeout)
{
    std::unique_lock<std::mutex> lock(m_lock);
    m_idle.wait_for(lock, timeout, [this]{ return (m_queue.empty() && !m_inFlight) || m_connectFailing; });
    return m_queue.empty() && !m_inFlight;
}

bool AsyncWorker::IsConnectFailing()
{
    std::lock_guard<std::mutex> lock(m_lock);
    return m_connectFailing;
}

size_t AsyncWorker::GetDepth()
{
    std::lock_guard<std::mutex> lock(m_lock);
    return m_queue.size() + (m_inFlight ? 1 : 0);
}

std::deque<AsyncJobPtr> AsyncWorker::Shutdown(milliseconds timeout,
    const std::function<void(uint64_t)>& killInFlight, bool& joined)
{
    std::deque<AsyncJobPtr> left;
    bool exited;
    {
        std::unique_lock<std::mutex> lock(m_lock);
        m_accepting = false;
        m_idle.wait_for(lock, timeout, [this]{ return m_exited || (m_queue.empty() && !m_inFlight); });
        left.swap(m_queue);
        m_stop = true;
    }
    m_wake.notify_all();
    m_stopSignal.notify_all();

    {
        // A job still running past the timeout gets a moment to finish, then its connection is
        // killed from the main connection so the thread can be joined.
        std::unique_lock<std::mutex> lock(m_lock);
        exited = m_idle.wait_for(lock, seconds(1), [this]{ return m_exited; });
    }
    if (!exited)
    {
        LOG_WARNING("Async SQL worker is still running a job; killing its connection.");
        killInFlight(m_connectionId);
        std::unique_lock<std::mutex> lock(m_lock);
        exited = m_idle.wait_for(lock, seconds(5), [this]{ return m_exited; });
    }

    if (exited)
    {
        m_thread.join();
        joined = true;
    }
    else
    {
        LOG_ERROR("Async SQL worker did not stop; detaching it.");
        m_thread.detach();
        joined = false;
    }
    return left;
}

bool AsyncWorker::EnsureConnected()
{
    if (m_target->IsConnected())
        return true;

    auto delay = milliseconds(100);
    for (int attempt = 1; !m_stop; attempt++)
    {
        try
        {
            m_target->Connect();
            m_connectionId = m_target->GetConnectionId();
            {
                std::lock_guard<std::mutex> lock(m_lock);
                m_connectFailing = false;
            }
            LOG_NOTICE("Async SQL worker connected (connection id %llu, attempt %d).",
                static_cast<unsigned long long>(m_connectionId.load()), attempt);
            return true;
        }
        catch (const std::runtime_error& e)
        {
            m_connectionId = 0;
            {
                std::lock_guard<std::mutex> lock(m_lock);
                m_connectFailing = true;
            }
            m_idle.notify_all();
            if (attempt == 1 || attempt % 10 == 0)
                LOG_ERROR("Async SQL worker: connect attempt %d failed: %s. Retrying.", attempt, e.what());
        }

        // Only the worker waits here; the main thread is never put to sleep by a reconnect.
        std::unique_lock<std::mutex> lock(m_lock);
        m_stopSignal.wait_for(lock, delay, [this]{ return m_stop.load(); });
        delay = std::min(delay * 2, milliseconds(5000));
    }
    return false;
}

void AsyncWorker::Run()
{
    m_target->ThreadInit();
    EnsureConnected();

    while (true)
    {
        AsyncJobPtr job;
        {
            std::unique_lock<std::mutex> lock(m_lock);
            m_wake.wait(lock, [this]{ return m_stop || !m_queue.empty(); });
            if (m_stop)
                break;
            job = std::move(m_queue.front());
            m_queue.pop_front();
            m_inFlight = true;
        }

        RunJob(*m_target, *job, [this]{ return EnsureConnected(); });
        // The target may have reconnected by itself (the one retry after error 2006), so keep the id
        // the shutdown KILL would use current.
        m_connectionId = m_target->GetConnectionId();

        // Hand the result over before the job stops counting as in flight, so a successful
        // Flush() means every flushed job's completion is already queued for the main thread.
        m_onComplete(std::move(job));
        {
            std::lock_guard<std::mutex> lock(m_lock);
            m_inFlight = false;
        }
        m_idle.notify_all();
    }

    // Closes the connection and releases the client library's per-thread state on this thread.
    m_target->ThreadEnd();
    {
        std::lock_guard<std::mutex> lock(m_lock);
        m_exited = true;
    }
    m_idle.notify_all();
}

}
