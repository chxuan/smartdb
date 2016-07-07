#ifndef _DATABASE_H
#define _DATABASE_H

#include <string>
#include <memory>
#include <type_traits>
#include "sqlite3/sqlite3.h"

namespace smartdb
{

class Database
{
public:
    Database() = default;
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    bool open(const std::string& databaseName)
    {
        int ret = sqlite3_open(databaseName.c_str(), &m_dbHandle);
        return ret == SQLITE_OK;
    }

    bool close()
    {
        if (m_dbHandle == nullptr)
        {
            return true;
        }

        sqlite3_finalize(m_statement);
        int ret = closeDBHandle();
        m_statement = nullptr;
        m_dbHandle = nullptr;
        return ret == SQLITE_OK;
    }

    bool exec(const std::string& sql)
    {
        int ret = sqlite3_exec(m_dbHandle, sql.c_str(), nullptr, nullptr, nullptr);
        return ret == SQLITE_OK;
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
};

};

#endif
