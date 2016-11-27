#ifndef _DB_QUERY_H
#define _DB_QUERY_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>
#include "define.hpp"

namespace smartdb
{

class db_query
{
public:
    db_query() = default;
    db_query(std::vector<std::vector<db_variant>>& buf, int& code) : buf_(buf), code_(code) {}

    bool is_select(sqlite3_stmt* statement)
    {
        col_count_ = sqlite3_column_count(statement);
        return col_count_ == 0 ? false : true;
    }
    
    bool read_table(sqlite3_stmt* statement)
    {
        buf_.clear();
        while (true)
        {
            code_ = sqlite3_step(statement);
            if (code_ == SQLITE_DONE)
            {
                break;
            }

            if (!read_row(statement))
            {
                sqlite3_reset(statement);
                buf_.clear();
                return false;
            }
        }

        sqlite3_reset(statement);
        return true;
    }

private:
    bool read_row(sqlite3_stmt* statement)
    {
        std::vector<db_variant> rowBuf;
        rowBuf.reserve(col_count_);
        for (int i = 0; i < col_count_; ++i)
        {
            if (!read_value(statement, i, rowBuf))
            {
                return false;
            }
        }

        buf_.emplace_back(std::move(rowBuf));
        return true;
    }

    bool read_value(sqlite3_stmt* statement, int index, std::vector<db_variant>& buf)
    {
        int type = sqlite3_column_type(statement, index);
        switch (type)
        {
        case SQLITE_INTEGER: 
            {
                buf.emplace_back(sqlite3_column_int64(statement, index));
                break;
            }
        case SQLITE_FLOAT: 
            {
                buf.emplace_back(sqlite3_column_double(statement, index));
                break; 
            }
        case SQLITE_TEXT:
            {
                buf.emplace_back(std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, index))));
                break; 
            }
        case SQLITE_BLOB: 
            {
                buf.emplace_back(std::string(reinterpret_cast<const char*>(sqlite3_column_blob(statement, index)), 
                                             sqlite3_column_bytes(statement, index)));
                break;
            }
        case SQLITE_NULL:
            {
                buf.emplace_back(std::string(""));
                break; 
            }
        default: 
            {
                std::cout << "Invaild type: " << type << std::endl;
                return false;
            }
        }

        return true;
    }

private:
    std::vector<std::vector<db_variant>>& buf_;
    int& code_;
    int col_count_ = 0;
};

}
#endif
