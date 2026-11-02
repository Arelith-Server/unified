#pragma once

#include "nwnx.hpp"
#include "Targets/ITarget.hpp"
#include "AsyncWorker.hpp"

#include <deque>
#include <memory>
#include <mutex>

using ArgumentStack = NWNXLib::ArgumentStack;

namespace SQL {

class SQL : public NWNXLib::Plugin
{
public:
    SQL(NWNXLib::Services::ProxyServiceList* services);
    virtual ~SQL();

    ArgumentStack PrepareQuery                  (ArgumentStack&& args);
    ArgumentStack ExecutePreparedQuery          (ArgumentStack&& args);
    ArgumentStack ReadyToReadNextRow            (ArgumentStack&& args);
    ArgumentStack ReadNextRow                   (ArgumentStack&& args);
    ArgumentStack ReadDataInActiveRow           (ArgumentStack&& args);
    ArgumentStack PreparedInt                   (ArgumentStack&& args);
    ArgumentStack PreparedString                (ArgumentStack&& args);
    ArgumentStack PreparedFloat                 (ArgumentStack&& args);
    ArgumentStack PreparedObjectId              (ArgumentStack&& args);
    ArgumentStack PreparedObjectFull            (ArgumentStack&& args);
    ArgumentStack PreparedNULL                  (ArgumentStack&& args);
    ArgumentStack ReadFullObjectInActiveRow     (ArgumentStack&& args);
    ArgumentStack GetAffectedRows               (ArgumentStack&& args);
    ArgumentStack GetDatabaseType               (ArgumentStack&& args);
    ArgumentStack DestroyPreparedQuery          (ArgumentStack&& args);
    ArgumentStack GetLastError                  (ArgumentStack&& args);
    ArgumentStack GetPreparedQueryParamCount    (ArgumentStack&& args);

    ArgumentStack PushAsyncParam                (ArgumentStack&& args);
    ArgumentStack PushAsyncParamInt             (ArgumentStack&& args);
    ArgumentStack PushAsyncParamFloat           (ArgumentStack&& args);
    ArgumentStack PushAsyncParamNULL            (ArgumentStack&& args);
    ArgumentStack ExecuteAsync                  (ArgumentStack&& args);
    ArgumentStack GetAsyncJobId                 (ArgumentStack&& args);
    ArgumentStack GetAsyncSuccess               (ArgumentStack&& args);
    ArgumentStack GetAsyncAffectedRows          (ArgumentStack&& args);
    ArgumentStack GetAsyncLastError             (ArgumentStack&& args);
    ArgumentStack AsyncReadyToReadNextRow       (ArgumentStack&& args);
    ArgumentStack AsyncReadNextRow              (ArgumentStack&& args);
    ArgumentStack AsyncReadDataInActiveRow      (ArgumentStack&& args);
    ArgumentStack AsyncFlush                    (ArgumentStack&& args);
    ArgumentStack GetAsyncQueueDepth            (ArgumentStack&& args);

    // Main thread: runs the callbacks of every finished async job.
    void RunAsyncCompletions();

    ITarget* GetTarget() { return m_target.get(); }

private:
    bool Reconnect(int32_t attempts = 1);
    std::unique_ptr<ITarget> CreateTarget();
    void RunAsyncJobSync(const AsyncJobPtr& job);
    void PostAsyncCompletion(AsyncJobPtr job); // any thread

    std::unique_ptr<ITarget> m_target;
    Query m_activeQuery;
    ResultSet m_activeResults;
    ResultRow m_activeRow;
    int32_t m_nextQueryId;
    bool m_queryMetrics;
    bool m_queryPrepared;
    bool m_utf8;
    std::string m_databaseType;

    // Async worker state. m_asyncWorker is null when async is disabled or unsupported.
    std::unique_ptr<AsyncWorker> m_asyncWorker;
    size_t m_asyncQueueLimit;
    std::chrono::milliseconds m_asyncShutdownTimeout;
    int32_t m_nextAsyncJobId;
    std::vector<AsyncParam> m_asyncParams;    // pushed for the next ExecuteAsync
    AsyncJobPtr m_asyncCurrent;               // the job whose callback is running
    ResultRow m_asyncActiveRow;

    std::mutex m_asyncCompletedLock;
    std::deque<AsyncJobPtr> m_asyncCompleted; // finished, callback not yet run
    bool m_asyncPumpQueued;
};

}

