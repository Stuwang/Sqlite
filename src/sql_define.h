#ifndef SQL_SQL_DEFINE_H__
#define SQL_SQL_DEFINE_H__

#include "../sqlite/include/sqlite3.h"

#define DISABLE_COPY(Class)\
	Class(const Class&) = delete;

#define DISABLE_ASSIGN(Class)\
	Class& operator=(const Class&) = delete;

#define DISABLE_COPY_AND_ASSIGN(Class)\
	DISABLE_COPY(Class)\
	DISABLE_ASSIGN(Class)

template<int N,class T>
char (&_arraySizeHelper(T(&)[N]))[N];
define arraysize(arr) (sizeof(_arraySizeHelper(arr)))


#endif //SQL_SQL_DEFINE_H__
