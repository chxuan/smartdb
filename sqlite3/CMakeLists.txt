cmake_minimum_required(VERSION 2.8)
project(sqlite)

set(STATICLIBNAME sqlite3)
set(OUTPUT_PATH "${PROJECT_SOURCE_DIR}/lib")

aux_source_directory(. DIR_SRCS)
add_library(${STATICLIBNAME} STATIC ${DIR_SRCS})

