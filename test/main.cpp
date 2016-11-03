#include <iostream>
#include <fstream>
#include "timer.hpp"
#include "smartdb/database.hpp"

void test_insert_table()
{
    smartdb::database db;
    bool ok = db.open("test.db");

    std::string sql = "DROP TABLE PersonTable";
    ok = db.execute(sql);

    sql = "CREATE TABLE if not exists PersonTable(id INTEGER NOT NULL, name Text, address Text)";
    ok = db.execute(sql);

    timer t;
    sql = "INSERT INTO PersonTable(id, name, address) VALUES(?, ?, ?)";
    const char* name = "Jack";
    std::string city = "Chengdu";

    // 预处理sql.
    ok = db.prepare(sql);

    // 开始事务.
    ok = db.begin();

    bool ret = true;
    for (int i = 1; i < 1000000; ++i)
    {
        // 绑定参数.
        /* ret = db.add_bind_value(std::forward_as_tuple(i, name, city)); */
        ret = db.add_bind_value(i, name, city);
        if (!ret)
        {
            std::cout << "Error message: " << db.get_error_message() << std::endl;
            break;
        }
    }
    if (ret)
    {
        // 提交事务.
        ok = db.commit();
    }
    else
    {
        // 回滚事务.
        ok = db.rollback();
    }

    // 100w 800~1000ms.
    std::cout << "Insert elapsed: " << t.elapsed() << std::endl;

    t.reset();
    // select.
    ok = db.execute("SELECT * FROM PersonTable");
    std::cout << "Record count: " << db.record_count() << std::endl;
    while (!db.is_end())
    {
        db.move_next();
    }
    // 100w 300~500ms.
    std::cout << "Select elapsed: " << t.elapsed() << std::endl;
    (void)ok;
}

void test_insert_table2()
{
    smartdb::database db;
    bool ok = db.open("test.db");

    std::string sql = "DROP TABLE PersonTable2";
    ok = db.execute(sql);

    sql = "CREATE TABLE if not exists PersonTable2(id INTEGER NOT NULL, name Text, address Text, headerImage BLOB)";
    ok = db.execute(sql);

    // 读取一张图片.
    std::ifstream fin;
    fin.open("./1.jpg", std::ios::binary);
    ok = fin.is_open();
    char buf[50 * 1025] = {"\0"};
    fin.read(buf, sizeof(buf));
    std::string image = std::string(buf, fin.gcount());
    smartdb::db_blob head_image;
    head_image.buf = image.c_str();
    head_image.size = image.length();

    sql = "INSERT INTO PersonTable2(id, name, address, headerImage) VALUES(?, ?, ?, ?)";
    for (int i = 0; i < 10; ++i)
    {
        /* ok = db.execute(sql, i, "Tom", nullptr, head_image); */
        ok = db.execute(sql, std::forward_as_tuple(i, "Tom", nullptr, head_image));
    }

    // update.
    sql = "UPDATE PersonTable2 SET address=? WHERE id=?";
    if (!db.execute(sql, "中国", 0))
    {
        std::cout << "Error message: " << db.get_error_message() << std::endl;
        return;
    }
    std::cout << "Update success, affected rows: " << db.affected_rows() << std::endl;

    // select.
    ok = db.execute("SELECT * FROM PersonTable2 WHERE id=?", 0);
    /* ok = db.execute("SELECT * FROM PersonTable2"); */
    while (!db.is_end())
    {
        try
        {
            std::cout << "id: " << db.get<sqlite3_int64>(0) << std::endl;
            std::cout << "name: " << db.get<std::string>(1) << std::endl;
            std::cout << "address: " << db.get<std::string>(2) << std::endl;
            std::cout << "image size: " << db.get<std::string>(3).size() << std::endl;
        }
        catch (std::exception& e)
        {
            std::cout << "Exception: " << e.what() << std::endl;
            return;
        }
        db.move_next();
    }

    ok = db.execute("SELECT COUNT(*) FROM PersonTable2");
    if (db.record_count() == 1 && !db.is_end())
    {
        std::cout << "COUNT(*): " << db.get<sqlite3_int64>(0) << std::endl;
    }
    (void)ok;
}

int main()
{
    test_insert_table();
    test_insert_table2();

    return 0;
}
