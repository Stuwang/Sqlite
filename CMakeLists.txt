
# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)



if(CMAKE_COMPILER_IS_GNUCXX)
	SET(CMAKE_CXX_COMPILER "clang++")
	message(STATUS "COMPILER:clang++")

	add_compile_options(-std=c++11)
	message(STATUS "optional:-std=c++11")

	add_compile_options(-Werror -Wall)
	message(STATUS "optional:-Werror -Wall")

	add_compile_options(-o2)
	message(STATUS "optional:-o2")

	add_compile_options(-pthread)
	message(STATUS "optional:-pthread")
endif(CMAKE_COMPILER_IS_GNUCXX)

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

include_directories(src)

if(WIN32)
	include_directories(sqlite3)
	add_library(sqlite3 STATIC ${SQLITE3_SRCS})
	add_library (sql STATIC ${SRCS})
	add_executable(run test/main.cpp)
	target_link_libraries(run sql sqlite3)
else(WIN32)
	include_directories(
		${CMAKE_CURRENT_SOURCE_DIR}/sqlite/include
	)
	link_directories(
		${CMAKE_CURRENT_SOURCE_DIR}/sqlite/lib
	)
	add_executable(run 
		test/main.cpp 
		${SRCS}
		${CMAKE_CURRENT_SOURCE_DIR}/sqlite/lib/libsqlite3.a
	)
	target_link_libraries(run sqlite3 pthread)
endif(WIN32)


