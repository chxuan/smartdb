cmake_minimum_required(VERSION 2.8)
project(sqlite)

set(STATICLIBNAME sqlite3)

aux_source_directory(. DIR_SRCS)
add_library(${STATICLIBNAME} STATIC ${DIR_SRCS})

