#ifndef _DEFINE_H
#define _DEFINE_H

#include <string>

const std::string Begin = "begin";
const std::string Commit = "commit";
const std::string Rollback = "rollback";

namespace smartdb
{

struct Blob
{
    const char* buf = nullptr;
    int size = 0;
};

};

#endif
