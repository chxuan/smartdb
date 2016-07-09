#include <iostream>
#include "Timer.hpp"
#include "smartdb/Database.hpp"

using namespace std;
void testCreateTable()
{
    smartdb::Database db;
	db.open("test.db");

	const string sqlcreat = "CREATE TABLE if not exists PersonTable(ID INTEGER NOT NULL, Name Text, Address Text);";
	if (!db.execute(sqlcreat))
		return;

	const string sqlinsert = "INSERT INTO PersonTable(ID, Name, Address) VALUES(?, ?, ?);";
	int id = 2;
	string name = "Peter";
	string city = "luzhou";
	/* if (!db.execute(sqlinsert, id, name, city)) */
	if (!db.executeTuple(sqlinsert, std::forward_as_tuple(id, name, city)))
		return;
    std::cout << "success" << std::endl;
}

int main()
{
#if 0
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
#endif
    testCreateTable();

    return 0;
}
