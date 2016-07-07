#ifndef _DEFINE_H
#define _DEFINE_H

#include <string>

const std::string Begin = "BEGIN";
const std::string Commit = "COMMIT";
const std::string Rollback = "ROLLBACK";

namespace smartdb
{

struct Blob
{
    const char* buf = nullptr;
    int size = 0;
};

};

#endif
