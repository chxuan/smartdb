#ifndef _DATABASE_H
#define _DATABASE_H

#include <string>
#include <memory>
#include <type_traits>
#include "sqlite3/sqlite3.h"

namespace smartdb
{

static const std::string Begin = "BEGIN";
static const std::string Commit = "COMMIT";
static const std::string Rollback = "ROLLBACK";

class Database
{
public:
    Database() = default;
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    bool open(const std::string& databaseName)
    {
        m_code = sqlite3_open(databaseName.c_str(), &m_dbHandle);
        return m_code == SQLITE_OK;
    }

    bool close()
    {
        if (m_dbHandle == nullptr)
        {
            return true;
        }

        sqlite3_finalize(m_statement);
        m_code = closeDBHandle();
        m_statement = nullptr;
        m_dbHandle = nullptr;
        return m_code == SQLITE_OK;
    }

    bool execute(const std::string& sql)
    {
        m_code = sqlite3_exec(m_dbHandle, sql.c_str(), nullptr, nullptr, nullptr);
        return m_code == SQLITE_OK;
    }

    template<typename... Args>
    bool execute(const std::string& sql, Args&&... args)
    {
        if (!prepare(sql))
        {
            return false;
        }

        return executeArgs(std::forward<Args>(args)...);
    }

    bool prepare(const std::string& sql)
    {
        m_code = sqlite3_prepare_v2(m_dbHandle, sql.c_str(), -1, &m_statement, nullptr);
        return m_code == SQLITE_OK;
    }

    template<typename... Args>
    bool executeArgs(Args&&... args)
    {

        m_code = sqlite3_step(m_statement);
        sqlite3_reset(m_statement);
        return m_code == SQLITE_DONE;
    }

    bool begin()
    {
        return execute(Begin);
    }

    bool commit()
    {
        return execute(Commit);
    }

    bool rollback()
    {
        return execute(Rollback);
    }

    int getErrorCode() const
    {
        return m_code; 
    }
private:
    int closeDBHandle()
    {
        int ret = sqlite3_close(m_dbHandle);
        while (ret == SQLITE_BUSY)
        {
            sqlite3_stmt* stmt = sqlite3_next_stmt(m_dbHandle, nullptr);
            if (stmt == nullptr)
            {
                break;
            }

            ret = sqlite3_finalize(stmt);
            if (ret == SQLITE_OK)
            {
                ret = sqlite3_close(m_dbHandle);
            }
        }

        return ret;
    }

private:
    sqlite3* m_dbHandle = nullptr;
    sqlite3_stmt* m_statement = nullptr;
    int m_code = 0;
};

};

#endif
