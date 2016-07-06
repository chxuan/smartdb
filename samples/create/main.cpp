#include <iostream>
#include "smartdb/Database.hpp"

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

    return 0;
}
