set(STATICLIBNAME sqlite3)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2")
aux_source_directory(. DIR_SRCS)
add_library(${STATICLIBNAME} STATIC ${DIR_SRCS})

