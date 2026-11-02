#pragma once

#include "Targets/ITarget.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

namespace SQL {

struct AsyncParam
{
    enum class Type { String, Int, Float, Null };
    Type type = Type::Null;
    std::string s;
    int32_t n = 0;
    float f = 0.0f;
};

// One NWNX_SQL_ExecuteAsync() call. Built on the main thread, run on the worker (or synchronously
// when async is disabled or the queue is full), then handed back to the main thread. Nothing in it
// refers to game objects except callbackTarget, which is only an id and is only looked up on the
// main thread.
struct AsyncJob
{
    int32_t id = 0;
    Query query;
    std::vector<AsyncParam> params;
    std::string callbackScript;
    ObjectID callbackTarget = 0;
    std::chrono::steady_clock::time_point queuedAt;

    // Results, written by whoever runs the job.
    bool ranSync = false;
    bool success = false;
    ResultSet results;
    int affectedRows = -1;
    std::string error;
    std::chrono::nanoseconds waitTime{0};  // queued -> started
    std::chrono::nanoseconds execTime{0};  // started -> finished
};
using AsyncJobPtr = std::shared_ptr<AsyncJob>;

// Runs a job on a target: prepare, bind the parameters, execute, collect the rows.
// reconnect() is called when the target reports it isn't connected; it returns true once connected.
// Follows the sync rules: preparing is retried after a reconnect (it has no side effects), and
// executing is only retried by the target itself, once, on "server has gone away" (2006).
void RunJob(ITarget& target, AsyncJob& job, const std::function<bool()>& reconnect);

// A single thread with its own target (connection) and its own FIFO queue.
class AsyncWorker
{
public:
    // onComplete is called on the worker thread after each job; it must not touch game state.
    AsyncWorker(std::unique_ptr<ITarget> target, std::function<void(AsyncJobPtr)> onComplete);
    ~AsyncWorker();

    // Queues a job. Returns false when the queue is at maxDepth or the worker is shutting down;
    // the caller then runs the job itself.
    bool Enqueue(AsyncJobPtr job, size_t maxDepth);

    // Waits until the queue is empty and no job is running. False on timeout.
    bool Flush(std::chrono::milliseconds timeout);

    size_t GetDepth();
    uint64_t GetConnectionId() const { return m_connectionId; }

    // Stops accepting jobs and drains the queue for up to timeout. If a job is still running after
    // that, killInFlight(connectionId) is called to break it off. Returns the jobs that never ran.
    // Sets joined to false if the thread couldn't be joined (it is then detached and this object
    // must be leaked, not destroyed).
    std::deque<AsyncJobPtr> Shutdown(std::chrono::milliseconds timeout,
        const std::function<void(uint64_t)>& killInFlight, bool& joined);

private:
    void Run();
    bool EnsureConnected();

    std::unique_ptr<ITarget> m_target;
    std::function<void(AsyncJobPtr)> m_onComplete;

    std::mutex m_lock;
    std::condition_variable m_wake;  // worker: new job, or stop
    std::condition_variable m_idle;  // waiters: a job finished, or the worker exited
    std::condition_variable m_stopSignal; // reconnect backoff: stop
    std::deque<AsyncJobPtr> m_queue;
    bool m_inFlight = false;
    bool m_accepting = true;
    bool m_exited = false;
    std::atomic<bool> m_stop{false};
    std::atomic<uint64_t> m_connectionId{0};

    std::thread m_thread;
};

}
