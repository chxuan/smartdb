cmake_minimum_required(VERSION 2.8)
project(test)

set(OUTPUTNAME test)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -O2 -std=c++11")

aux_source_directory(. DIR_SRCS)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/..)
include_directories($ENV{BOOST_INCLUDE_PATH})

link_directories(${PROJECT_SOURCE_DIR}/../sqlite3)
link_directories($ENV{BOOST_LIB_PATH})

add_executable(${OUTPUTNAME} ${DIR_SRCS})

target_link_libraries(${OUTPUTNAME} libsqlite3.a)
target_link_libraries(${OUTPUTNAME} libpthread.so)
target_link_libraries(${OUTPUTNAME} libdl.so)
