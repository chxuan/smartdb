#include <assert.h>
#include <iostream>
#include "Timer.hpp"
#include "smartdb/Database.hpp"

void testInsertTable()
{
    smartdb::Database db;
    assert(db.open("test.db"));

    std::string sql = "DROP TABLE PersonTable";
    db.execute(sql);

    sql = "CREATE TABLE if not exists PersonTable(ID INTEGER NOT NULL, Name Text, Address Text)";
    assert(db.execute(sql));

    Timer t;
    sql = "INSERT INTO PersonTable(ID, Name, Address) VALUES(?, ?, ?)";
    std::string name = "Jack";
    std::string city = "Chengdu";

    // 预处理sql
    assert(db.prepare(sql));

    // 开始事务
    assert(db.begin());

    bool ret = true;
    for (int i = 1; i < 1000000; ++i)
    {
        // 绑定参数
        /* ret = db.addBindValue(std::forward_as_tuple(i, name, city)); */
        ret = db.addBindValue(i, name, city);
        if (!ret)
        {
            std::cout << "Error message: " << db.getErrorMessage() << std::endl;
            break;
        }
    }
    if (ret)
    {
        // 提交事务
        assert(db.commit());
    }
    else
    {
        // 回滚事务
        assert(db.rollback());
    }

    // 100w 800~1000ms
    std::cout << "time: " << t.elapsed() << std::endl;
}

int main()
{
    testInsertTable();

    return 0;
}
