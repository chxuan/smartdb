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
int executeTuple2(sqlite3_stmt* statement, IndexTuple<Indexes...>&&, Tuple&& t)
{
     int code = bindParams(statement, 1, std::get<Indexes>(std::forward<Tuple>(t))...);
     /* int code = bindParams(statement, 1, std::get<Indexes>(t...); */
     if (code != SQLITE_OK)
     {
        return code;
     }

     code = sqlite3_step(statement);
     sqlite3_reset(statement);
     return code;
}
/////////////////////////////////////////////////////
template<typename Tuple, std::size_t N>
struct TuplePrinter
{
    static void print(sqlite3_stmt* statement, const Tuple& t)
    {
        TuplePrinter<Tuple, N - 1>::print(statement, t);
        bindValue(statement, N, std::get<N - 1>(t));
        std::cout << ", " << std::get<N - 1>(t);
        std::cout << N;
        /* std::cout << index; */
    }
};

template<typename Tuple>
struct TuplePrinter<Tuple, 1>
{
    static void print(sqlite3_stmt* statement, const Tuple& t)
    {
        std::cout << std::get<0>(t);
        bindValue(statement, 1, std::get<0>(t));
        std::cout << 1;
        /* std::cout << index; */
    }
};

template<typename... Args>
void executeTupleImpl(sqlite3_stmt* statement, const std::tuple<Args...>& t)
{
    TuplePrinter<decltype(t), sizeof...(Args)>::print(statement, t);
    std::cout << std::endl;
    sqlite3_step(statement);
    sqlite3_reset(statement);
}

#if 0
template<typename Tuple>
void testTuple(Tuple&& t)
{
    printTuple(0, std::forward<Tuple>(t));
}
#endif

};

#endif
