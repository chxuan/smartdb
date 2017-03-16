#ifndef _DATABASE_H
#define _DATABASE_H

#include <assert.h>
#include <string>
#include <memory>
#include <tuple>
#include <functional>
#include <unordered_map>
#include <type_traits>
#include "traits.hpp"
#include "bind_parames.hpp"
#include "tuple_helper.hpp"
#include "db_query.hpp"

namespace smartdb
{

class database
{
public:
    database(const database&) = delete;
    database& operator=(const database&) = delete;
    database() : query_(buf_, code_) {}
    ~database()
    {
        close();
    }

    bool open_file_db(const std::string& databaseName)
    {
        code_ = sqlite3_open(databaseName.c_str(), &db_handle_);
        return code_ == SQLITE_OK;
    }

    bool open_memory_db()
    {
        code_ = sqlite3_open(memory_db_str.c_str(), &db_handle_);
        return code_ == SQLITE_OK;
    }

    bool close()
    {
        if (db_handle_ == nullptr)
        {
            return true;
        }

        sqlite3_finalize(statement_);
        code_ = close_db_handle();
        statement_ = nullptr;
        db_handle_ = nullptr;
        return code_ == SQLITE_OK;
    }

    bool execute(const std::string& sql)
    {
        if (!prepare(sql))
        {
            return false;
        }

        if (!execute())
        {
            return false;
        }
        return try_select();
    }

    template<typename... Args>
    bool execute(const std::string& sql, Args&&... args)
    {
        if (!prepare(sql))
        {
            return false;
        }

        if (!add_bind_value(std::forward<Args>(args)...))
        {
            return false;
        }
        return try_select();
    }

    template<typename Tuple>
    typename std::enable_if<is_tuple<Tuple>::value, bool>::type execute(const std::string& sql, Tuple&& t)
    {
        if (!prepare(sql))        
        {
            return false;
        }

        if (!add_bind_value(std::forward<Tuple>(t)))
        {
            return false;
        }
        return try_select();
    }

    bool try_select()
    {
        if (query_.is_select(statement_))
        {
            if (!query_.read_table(statement_))
            {
                return false;
            }
            move_first();
        }
        return true;
    }

    bool prepare(const std::string& sql)
    {
        code_ = sqlite3_prepare_v2(db_handle_, sql.c_str(), -1, &statement_, nullptr);
        return code_ == SQLITE_OK;
    }

    template<typename... Args>
    bool add_bind_value(Args&&... args)
    {
        code_ = bind_params(statement_, 1, std::forward<Args>(args)...);
        if (code_ != SQLITE_OK)
        {
            return false;
        }
        return execute();
    }

    template<typename Tuple>
    typename std::enable_if<is_tuple<Tuple>::value, bool>::type add_bind_value(Tuple&& t)
    {
        code_ = add_bind_tuple(statement_, std::forward<Tuple>(t)); 
        if (code_ != SQLITE_OK)
        {
            return false;
        }
        return execute();
    }

    template<typename T>
    T& get(int index)
    {
        return boost::get<T>((*iter_)[index]);
    }

    void move_first()
    {
        iter_ = buf_.begin();
    }

    void move_next()
    {
        if (!is_end())
        {
            ++iter_;
        }
    }

    bool is_end()
    {
        if (iter_ == buf_.end())
        {
            buf_.clear();
            move_first();
            return true;
        }
        return false;
    }

    std::size_t record_count()
    {
        return buf_.size();
    }

    bool begin()
    {
        code_ = sqlite3_exec(db_handle_, begin_str.c_str(), nullptr, nullptr, nullptr);
        return code_ == SQLITE_OK;
    }

    bool commit()
    {
        code_ = sqlite3_exec(db_handle_, commit_str.c_str(), nullptr, nullptr, nullptr);
        return code_ == SQLITE_OK;
    }

    bool rollback()
    {
        code_ = sqlite3_exec(db_handle_, rollback_str.c_str(), nullptr, nullptr, nullptr);
        return code_ == SQLITE_OK;
    }

    int affected_rows()
    {
        return sqlite3_changes(db_handle_);
    }

    int get_error_code() const
    {
        return code_; 
    }

    const char* get_error_string() const
    {
        return sqlite3_errstr(code_);
    }

    const char* get_error_message() const
    {
        return sqlite3_errmsg(db_handle_);
    }

private:
    int close_db_handle()
    {
        int ret = sqlite3_close(db_handle_);
        while (ret == SQLITE_BUSY)
        {
            sqlite3_stmt* stmt = sqlite3_next_stmt(db_handle_, nullptr);
            if (stmt == nullptr)
            {
                break;
            }

            ret = sqlite3_finalize(stmt);
            if (ret == SQLITE_OK)
            {
                ret = sqlite3_close(db_handle_);
            }
        }

        return ret;
    }

    bool execute()
    {
        code_ = sqlite3_step(statement_);
        sqlite3_reset(statement_);
        // 当执行insert、update、drop等操作时成功返回SQLITE_DONE.
        // 当执行select操作时，成功返回SQLITE_ROW.
        return (code_ == SQLITE_DONE || code_ == SQLITE_ROW || code_ == SQLITE_OK);
    }

private:
    sqlite3* db_handle_ = nullptr;
    sqlite3_stmt* statement_ = nullptr;
    int code_ = 0;
    std::vector<std::vector<db_variant>> buf_;
    std::vector<std::vector<db_variant>>::iterator iter_ = buf_.end();
    db_query query_;
};

}

#endif
