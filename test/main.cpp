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

    sql = "CREATE TABLE if not exists PersonTable(id INTEGER NOT NULL, name Text, address Text)";
    assert(db.execute(sql));

    Timer t;
    sql = "INSERT INTO PersonTable(id, name, address) VALUES(?, ?, ?)";
    const char* name = "Jack";
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
    std::cout << "Insert elapsed: " << t.elapsed() << std::endl;
}

void testInsertTable2()
{
    smartdb::Database db;
    assert(db.open("test.db"));

    std::string sql = "DROP TABLE PersonTable2";
    db.execute(sql);

    sql = "CREATE TABLE if not exists PersonTable2(id INTEGER NOT NULL, name Text, address Text)";
    assert(db.execute(sql));

    sql = "INSERT INTO PersonTable2(id, name, address) VALUES(?, ?, ?)";

    Timer t;
    for (int i = 0; i < 1000; ++i)
    {
        /* assert(db.execute(sql, i, "Tom", nullptr)); */
        assert(db.execute(sql, std::forward_as_tuple(i, "Tom", nullptr)));
    }

    // 1000 2~4s
    std::cout << "Insert2 elapsed: " << t.elapsed() << std::endl;

    // update
    sql = "UPDATE PersonTable2 SET address=? WHERE id=?";
    if (!db.execute(sql, "China", 0))
    {
        std::cout << "Error message: " << db.getErrorMessage() << std::endl;
        return;
    }
    std::cout << "Update success, affected rows: " << db.affectedRows() << std::endl;
}

int main()
{
    testInsertTable();
    testInsertTable2();

    return 0;
}
