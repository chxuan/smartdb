#include <iostream>
#include "Timer.hpp"
#include "smartdb/Database.hpp"

using namespace std;
void testCreateTable()
{
    smartdb::Database db;
	db.open("test.db");

	const string sqlcreat = "CREATE TABLE if not exists PersonTable(ID INTEGER NOT NULL, Name Text, Address Text);";
	/* const string sqlcreat = "CREATE TABLE if not exists PersonTable(ID INTEGER NOT NULL);"; */
	if (!db.execute(sqlcreat))
		return;

    Timer t;
	const string sqlinsert = "INSERT INTO PersonTable(ID, Name, Address) VALUES(?, ?, ?);";
	/* const string sqlinsert = "INSERT INTO PersonTable(ID) VALUES(?);"; */
	string name = "Peter";
	string city = "Chegndu";
#if 1
	if (!db.prepare(sqlinsert))
    {
        std::cout << db.getErrorMessage() << std::endl;
        std::cout << "error" << std::endl;
		return;
    }

    db.begin();
    int ret;
    for (int i = 1; i < 1000000; ++i)
    {
        /* std::tuple<int, std::string, std::string> aaa = std::make_tuple(i, name, city); */
        /* ret = db.addBindValue(aaa); */
        ret = db.addBindValue(std::forward_as_tuple(i, name, city));
        /* ret = db.addBindValue(i, name, city); */
        /* ret = db.addBindValue(std::forward_as_tuple(i)); */
        /* ret = db.addBindValue(i); */
        if (!ret)
        {
            std::cout << db.getErrorMessage() << std::endl;
            break;
        }
    }
    if (ret)
    {
        std::cout << "commit" << std::endl;
        db.commit();
    }
    else
    {
        std::cout << "rollback" << std::endl;
        db.rollback();
    }
#else
	if (!db.executeTuple(sqlinsert, std::forward_as_tuple(10000000, name, city)))
    {
        std::cout << "failed" << std::endl;
        std::cout << db.getErrorString() << std::endl;
        std::cout << db.getErrorMessage() << std::endl;
    }
#endif
    std::cout << "success" << std::endl;
    std::cout << "time: " << t.elapsed() << std::endl;
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
