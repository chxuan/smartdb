#include <iostream>
#include <fstream>
#include "Timer.hpp"
#include "smartdb/Database.hpp"

void testInsertTable()
{
    smartdb::Database db;
    bool ok = db.open("test.db");

    std::string sql = "DROP TABLE PersonTable";
    ok = db.execute(sql);

    sql = "CREATE TABLE if not exists PersonTable(id INTEGER NOT NULL, name Text, address Text)";
    ok = db.execute(sql);

    Timer t;
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
    std::cout << "Record count: " << db.recordCount() << std::endl;
    while (!db.isEnd())
    {
        db.moveNext();
    }
    // 100w 300~500ms.
    std::cout << "Select elapsed: " << t.elapsed() << std::endl;
    (void)ok;
}

void testInsertTable2()
{
    smartdb::Database db;
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
    smartdb::Blob headImage;
    headImage.buf = image.c_str();
    headImage.size = image.length();

    sql = "INSERT INTO PersonTable2(id, name, address, headerImage) VALUES(?, ?, ?, ?)";
    for (int i = 0; i < 10; ++i)
    {
        /* ok = db.execute(sql, i, "Tom", nullptr, headImage); */
        ok = db.execute(sql, std::forward_as_tuple(i, "Tom", nullptr, headImage));
    }

    // update.
    sql = "UPDATE PersonTable2 SET address=? WHERE id=?";
    if (!db.execute(sql, "中国", 0))
    {
        std::cout << "Error message: " << db.getErrorMessage() << std::endl;
        return;
    }
    std::cout << "Update success, affected rows: " << db.affectedRows() << std::endl;

    // select.
    ok = db.execute("SELECT * FROM PersonTable2 WHERE id=?", 0);
    /* ok = db.execute("SELECT * FROM PersonTable2"); */
    while (!db.isEnd())
    {
        try
        {
            std::cout << "id: " << db.getFiled<sqlite3_int64>(0) << std::endl;
            std::cout << "name: " << db.getFiled<std::string>(1) << std::endl;
            std::cout << "address: " << db.getFiled<std::string>(2) << std::endl;
            std::cout << "image size: " << db.getFiled<std::string>(3).size() << std::endl;
        }
        catch (std::exception& e)
        {
            std::cout << "Exception: " << e.what() << std::endl;
            return;
        }
        db.moveNext();
    }

    ok = db.execute("SELECT COUNT(*) FROM PersonTable2");
    if (db.recordCount() == 1 && !db.isEnd())
    {
        std::cout << "COUNT(*): " << db.getFiled<sqlite3_int64>(0) << std::endl;
    }
    (void)ok;
}

int main()
{
    testInsertTable();
    testInsertTable2();

    return 0;
}
