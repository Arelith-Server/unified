#pragma once

#if defined(NWNX_SQL_MYSQL_SUPPORT)

#include "mysql/mysql.h"
#include "mysql/errmsg.h"
#include "Targets/ITarget.hpp"

namespace SQL {

class MySQL final : public ITarget
{
public:
    MySQL();
    virtual ~MySQL() override;

    virtual void Connect() override;
    virtual bool IsConnected() override;
    virtual bool PrepareQuery(const Query& query) override;
    virtual std::optional<ResultSet> ExecuteQuery() override;
    virtual void PrepareInt(int32_t position, int32_t value) override;
    virtual void PrepareFloat(int32_t position, float value) override;
    virtual void PrepareString(int32_t position, const std::string& value) override;
    virtual void PrepareBinary(int32_t position, const std::vector<uint8_t>& value) override;
    virtual void PrepareNULL(int32_t position) override;
    virtual int  GetAffectedRows() override;
    virtual std::string GetLastError(bool bClear = false) override;
    virtual int32_t GetPreparedQueryParamCount() override;
    virtual void DestroyPreparedQuery() override;


private:
    void NoteConnectionError(unsigned int error);
    std::optional<ResultSet> Execute(bool bRetryIfGone);

    MYSQL m_mysql;
    MYSQL_STMT *m_stmt;
    Query m_query;
    // Set by Connect() and cleared when a call fails with a connection error. Checking this instead of
    // sending "SELECT 1" before every query saves two round trips per query.
    bool m_connected;
    bool m_initialised;
    std::vector<MYSQL_BIND> m_params;
    size_t m_paramCount;
    std::string m_lastError;

    // No std::variant available, and C++ really doesn't like strings in unions.
    struct Variant { float f; int32_t n; std::string s; std::vector<uint8_t> b;
        Variant() { s = ""; }
    };
    std::vector<Variant> m_paramValues;
    int affectedRows;
};

}

#endif
