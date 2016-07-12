#ifndef _QUERY_H
#define _QUERY_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>
#include "Define.hpp"

namespace smartdb
{

class Query
{
public:
    Query() = default;
    Query(std::vector<std::vector<DBVariant>>& buf, int& code) : m_buf(buf), m_code(code) {}

    bool isSelect(sqlite3_stmt* statement)
    {
        m_colCount = sqlite3_column_count(statement);
        return m_colCount == 0 ? false : true;
    }
    
    void readTable(sqlite3_stmt* statement)
    {
        m_buf.clear();
        while (true)
        {
            m_code = sqlite3_step(statement);
            if (m_code == SQLITE_DONE)
            {
                break;
            }

            readRow(statement);
        }

        sqlite3_reset(statement);
    }

private:
    void readRow(sqlite3_stmt* statement)
    {
        std::vector<DBVariant> rowBuf;
        rowBuf.reserve(m_colCount);
        for (int i = 0; i < m_colCount; ++i)
        {
            readValue(statement, i, rowBuf);
        }
        m_buf.emplace_back(std::move(rowBuf));
    }

    bool readValue(sqlite3_stmt* statement, int index, std::vector<DBVariant>& buf)
    {
        int type = sqlite3_column_type(statement, index);
        return true;
    }

private:
    std::vector<std::vector<DBVariant>> m_buf;
    int m_code = 0;
    int m_colCount = 0;
    /* static std::unordered_map<int, std::function<void() */
};

};
#endif
