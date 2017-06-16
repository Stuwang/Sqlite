SET(CMAKE_CXX_COMPILER "clang++")
message(STATUS "COMPILER:clang++")

# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# if(CMAKE_COMPILER_IS_GNUCXX)
add_compile_options(-std=c++11)
message(STATUS "optional:-std=c++11")

add_compile_options(-Werror -Wall)
message(STATUS "optional:-Werror -Wall")

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

aux_source_directory(src SRCS)

add_library (test STATIC ${SRCS})


