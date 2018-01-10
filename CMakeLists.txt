
# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

SET(CMAKE_CXX_COMPILER "clang++")
message(STATUS "COMPILER:clang++")

# if(CMAKE_COMPILER_IS_GNUCXX)
add_compile_options(-std=c++11)
message(STATUS "optional:-std=c++11")

# add_compile_options(-Werror -Wall)
# message(STATUS "optional:-Werror -Wall")

add_compile_options(-o2)
message(STATUS "optional:-o2")

add_compile_options(-pthread)
message(STATUS "optional:-pthread")

# message(STATUS "CMAKE_BINARY_DIR")
# message(STATUS "${CMAKE_BINARY_DIR}")
# message(STATUS "CMAKE_BINARY_DIR END")


# if("${CMAKE_BUILD_TYPE}" equal "debug")
# 	add_compile_options(-g)
# 	message(STATUS "optional:-g")
# else()

# test module path
# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
# message(STATUS "${CMAKE_MODULE_PATH}")
# endif(CMAKE_COMPILER_IS_GNUCXX)

# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

# 项目信息
project (sql)

set(SQLITE3_SRCS

# sqlite3/shell.c
sqlite3/sqlite3.c
sqlite3/sqlite3.h
sqlite3/sqlite3ext.h
	)

set(SRCS

src/Connection.cpp
src/Connection.h
src/sql_define.h
src/Statement.cpp
src/Statement.h
src/Transaction.cpp
src/Transaction.h
)
# include_directories(sqlite3)
include_directories(
${CMAKE_CURRENT_SOURCE_DIR}/sqlite/include
src
)

link_directories(
${CMAKE_CURRENT_SOURCE_DIR}/sqlite/lib
)

message(${CMAKE_CURRENT_SOURCE_DIR}/sqlite/lib)

# add_library(sqlite3 STATIC ${SQLITE3_SRCS})

# add_library (sql STATIC ${SRCS})

add_executable(run 
	test/main.cpp 
	${SRCS}
	${CMAKE_CURRENT_SOURCE_DIR}/sqlite/lib/libsqlite3.a
)
target_link_libraries(run sqlite3 pthread)
