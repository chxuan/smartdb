#ifndef _JSONUTIL_H
#define _JSONUTIL_H

#include <string>
#include <type_traits>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"

namespace smartdb
{

class JsonUtil
{
public:
    JsonUtil() : m_writer(m_buf) {}

    void startArray()
    {
        m_writer.StartArray();
    }

    void endArray()
    {
        m_writer.EndArray();
    }

    void startObject()
    {
        m_writer.StartObject();
    }

    void endObject()
    {
        m_writer.EndObject();
    }

    template<typename T>
    void writeJson(const std::string& key, T&& value)
    {
        m_writer.String(key.c_str());
        writeValue(std::forward<T>(value));
    }

    template<typename T>
    void writeJson(const char* key, T&& value)
    {
        m_writer.String(key);
        writeValue(std::forward<T>(value));
    }

    const char* getString() const
    {
        return m_buf.GetString();
    }

private:
    template<typename T>
    typename std::enable_if<std::is_same<T, int>::value>::type writeValue(T value)
    {
        m_writer.Int(value);
    }

    template<typename T>
    typename std::enable_if<std::is_same<T, unsigned int>::value>::type writeValue(T value)
    {
        m_writer.Uint(value);
    }

    template<typename T>
    typename std::enable_if<std::is_same<T, int64_t>::value>::type writeValue(T value)
    {
        m_writer.Int64(value);
    }

    template<typename T>
    typename std::enable_if<std::is_floating_point<T>::value>::type writeValue(T value)
    {
        m_writer.Double(value);
    }

    template<typename T>
    typename std::enable_if<std::is_same<T, bool>::value>::type writeValue(T value)
    {
        m_writer.Bool(value);
    }

    template<typename T>
    typename std::enable_if<std::is_pointer<T>::value>::type writeValue(T value)
    {
        m_writer.String(value);
    }

    template<typename T>
    typename std::enable_if<std::is_array<T>::value>::type writeValue(T value)
    {
        m_writer.String(value);
    }

private:
    rapidjson::Writer<rapidjson::StringBuffer> m_writer;
    rapidjson::StringBuffer m_buf;
    rapidjson::Document m_doc;
};

};

#endif
