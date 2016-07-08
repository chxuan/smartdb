#include <iostream>
#include "Timer.hpp"
#include "smartdb/Database.hpp"
#include "smartdb/JsonUtil.hpp"

void testJsonUtil()
{
    Timer t;
    smartdb::JsonUtil json;
    json.startArray();
    for (int i = 0; i < 1000000; ++i)
    {
        json.startObject();
        json.writeJson("id", i);
        json.writeJson("name", "Jack");
        json.writeJson("address", "Chengdu");
        json.endObject();
    }
    json.endArray();

    // 100w 100~170ms
    std::cout << "testJsonUtil time: " << t.elapsed() << "ms" << std::endl;
}

using namespace std;
void testCreateTable()
{
    smartdb::Database db;
	db.open("test.db");

	const string sqlcreat = "CREATE TABLE if not exists PersonTable(ID INTEGER NOT NULL, Name Text, Address BLOB);";
	if (!db.Excecute(sqlcreat))
		return;

	const string sqlinsert = "INSERT INTO PersonTable(ID, Name, Address) VALUES(?, ?, ?);";
	int id = 2;
	string name = "Peter";
	string city = "zhuhai";
	blob bl = { city.c_str(), city.length() + 1 };
	if (!db.Excecute(sqlinsert, id, "Peter", nullptr))
		return;

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
