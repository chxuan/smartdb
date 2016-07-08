#ifndef _TUPLE_H
#define _TUPLE_H

#include <tuple>
#include "sqlite3/sqlite3.h"
#include "BindParames.hpp"

namespace smartdb
{

template<int...>
struct IndexTuple {};

template<int N, int... Indexes>
struct MakeIndexes : MakeIndexes<N - 1, N - 1, Indexes...> {};

template<int... Indexes>
struct MakeIndexes<0, Indexes...>
{
    using type = IndexTuple<Indexes...>;
};

template<int... Indexes, typename Tuple>
int executeTuple(sqlite3_stmt* statement, IndexTuple<Indexes...>&& in, Tuple&& t)
{
    (void)in;
     int code = bindParams(statement, 1, std::get<Indexes>(std::forward<Tuple>(t))...);
     if (code != SQLITE_OK)
     {
        return code;
     }

     code = sqlite3_step(statement);
     sqlite3_reset(statement);
     return code;
}

};

#endif
