#pragma once

#include "nwnx.hpp"

#include <queue>
#include <string>
#include <vector>
#include <optional>

namespace SQL {

using Query = std::string;
using Result = std::string;
using ResultRow = std::vector<Result>;
using ResultSet = std::queue<ResultRow>; // Queue is row, vector is column

struct ITarget
{
    virtual ~ITarget() { }
    virtual void Connect() = 0;
    virtual bool IsConnected() = 0;
    virtual bool PrepareQuery(const Query& query) = 0;
    virtual std::optional<ResultSet> ExecuteQuery() = 0;
    virtual void PrepareInt(int32_t position, int32_t value) = 0;
    virtual void PrepareFloat(int32_t position, float value) = 0;
    virtual void PrepareString(int32_t position, const std::string& value) = 0;
    virtual void PrepareBinary(int32_t position, const std::vector<uint8_t>& value) = 0;
    virtual void PrepareNULL(int32_t position) = 0;
    virtual int  GetAffectedRows() = 0;
    virtual std::string GetLastError(bool bClear = false) = 0;
    virtual int32_t GetPreparedQueryParamCount() = 0;
    virtual void DestroyPreparedQuery() = 0;

    // Used by the async worker, which owns its own target instance on its own thread.
    // Called on that thread before its first and after its last use of the target.
    virtual void ThreadInit() { }
    virtual void ThreadEnd() { }
    // Server-side id of the current connection (0 if unknown), so it can be killed at shutdown.
    virtual uint64_t GetConnectionId() { return 0; }
};

}
