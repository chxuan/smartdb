set(OUTPUTNAME create)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -O2 -std=c++14")

aux_source_directory(. DIR_SRCS)

include_directories(${PROJECT_SOURCE_DIR})
link_directories(${PROJECT_SOURCE_DIR}/sqlite3)

add_executable(${OUTPUTNAME} ${DIR_SRCS})

target_link_libraries(${OUTPUTNAME} libsqlite3.a)
target_link_libraries(${OUTPUTNAME} libpthread.so)
target_link_libraries(${OUTPUTNAME} libdl.so)
