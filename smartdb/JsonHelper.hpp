#ifndef _JSONHELPER_H
#define _JSONHELPER_H

#include "sqlite3/sqlite3.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"

namespace smartdb
{

class JsonHelper
{
public:
    JsonHelper(rapidjson::StringBuffer& buf, int& code) : m_code(code), m_writer(buf) {}

    bool executeJson(sqlite3_stmt* statement, const rapidjson::Value& val)
    { 
    }

private:
    int& m_code;
    rapidjson::Writer<rapidjson::StringBuffer> m_writer;
};

};

#endif
