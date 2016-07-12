#ifndef _DATABASE_H
#define _DATABASE_H

#include <assert.h>
#include <string>
#include <memory>
#include <tuple>
#include <functional>
#include <unordered_map>
#include <type_traits>
#include "Traits.hpp"
#include "BindParames.hpp"
#include "TupleHelper.hpp"
#include "Query.hpp"

namespace smartdb
{

class Database
{
public:
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    Database() : m_query(m_buf, m_code) {}

    ~Database()
    {
        close();
    }

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
        /* return m_code == SQLITE_OK; */
        if (m_code != SQLITE_OK)
        {
            return false;
        }

        if (m_query.isSelect(m_statement))
        {
            if (!m_query.readTable(m_statement))
            {
                return false;
            }
            moveFirst();
        }
        return true;
    }

    template<typename... Args>
    bool execute(const std::string& sql, Args&&... args)
    {
        if (!prepare(sql))
        {
            return false;
        }

        /* return addBindValue(std::forward<Args>(args)...); */
        if (!addBindValue(std::forward<Args>(args)...))
        {
            return false;
        }

        if (m_query.isSelect(m_statement))
        {
            if (!m_query.readTable(m_statement))
            {
                return false;
            }
            moveFirst();
        }
        return true;
    }

    template<typename Tuple>
    typename std::enable_if<is_tuple<Tuple>::value, bool>::type execute(const std::string& sql, Tuple&& t)
    {
        if (!prepare(sql))        
        {
            return false;
        }

        /* return addBindValue(std::forward<Tuple>(t)); */
        if (!addBindValue(std::forward<Tuple>(t)))
        {
            return false;
        }

        if (m_query.isSelect(m_statement))
        {
            if (!m_query.readTable(m_statement))
            {
                return false;
            }
            moveFirst();
        }
        return true;
    }

    bool prepare(const std::string& sql)
    {
        m_code = sqlite3_prepare_v2(m_dbHandle, sql.c_str(), -1, &m_statement, nullptr);
        return m_code == SQLITE_OK;
    }

    template<typename... Args>
    bool addBindValue(Args&&... args)
    {
        m_code = bindParams(m_statement, 1, std::forward<Args>(args)...);
        if (m_code != SQLITE_OK)
        {
            return false;
        }

        m_code = sqlite3_step(m_statement);
        sqlite3_reset(m_statement);
        return m_code == SQLITE_DONE;
    }

    template<typename Tuple>
    typename std::enable_if<is_tuple<Tuple>::value, bool>::type addBindValue(Tuple&& t)
    {
        m_code = addBindTuple(m_statement, std::forward<Tuple>(t)); 
        if (m_code != SQLITE_OK)
        {
            return false;
        }

        m_code = sqlite3_step(m_statement);
        sqlite3_reset(m_statement);
        return m_code == SQLITE_DONE;
    }

    template<typename T>
    T& getFiled(int index)
    {
        return boost::get<T>((*m_iter)[index]);
    }

    void moveFirst()
    {
        m_iter = m_buf.begin();
    }

    void moveNext()
    {
        ++m_iter;
    }

    bool isEnd()
    {
        return m_iter == m_buf.end(); 
    }

    std::size_t recordCount()
    {
        return m_buf.size();
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

    int affectedRows()
    {
        return sqlite3_changes(m_dbHandle);
    }

    int getErrorCode() const
    {
        return m_code; 
    }

    const char* getErrorString() const
    {
        return sqlite3_errstr(m_code);
    }

    const char* getErrorMessage() const
    {
        return sqlite3_errmsg(m_dbHandle);
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
    std::vector<std::vector<DBVariant>> m_buf;
    std::vector<std::vector<DBVariant>>::iterator m_iter = m_buf.end();
    Query m_query;
};

};

#endif
