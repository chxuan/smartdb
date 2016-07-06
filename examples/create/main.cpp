#include <iostream>
#include "smartdb/Database.hpp"
#include "smartdb/JsonUtil.hpp"

void testJsonUtil()
{
    smartdb::JsonUtil json;
    json.startArray();
    for (int i = 0; i < 10; ++i)
    {
        json.startObject();
        json.writeJson("id", i);
        json.writeJson("name", "Jack");
        json.writeJson("address", "Chengdu");
        json.endObject();
    }
    json.endArray();
    std::cout << json.getString() << std::endl;
}

int main()
{
    smartdb::Database db;
    bool ok = db.open("test.db");
    if (!ok)
    {
        std::cout << "Open db failed" << std::endl;
        return 1;
    }
    std::cout << "Open db success" << std::endl;

    ok = db.close();
    if (!ok)
    {
        std::cout << "Close db failed" << std::endl;
    }
    std::cout << "Close db success" << std::endl;

    testJsonUtil();


    return 0;
}
