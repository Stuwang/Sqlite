#ifndef SQL_SQL_DEFINE_H__
#define SQL_SQL_DEFINE_H__

#include "sqlite3.h"
#include <cassert>

#define DISABLE_COPY(Class)\
	Class(const Class&) = delete;

#define DISABLE_ASSIGN(Class)\
	Class& operator=(const Class&) = delete;

#define DISABLE_COPY_AND_ASSIGN(Class)\
	DISABLE_COPY(Class)\
	DISABLE_ASSIGN(Class)

template<int N, class T>
char (&_arraySizeHelper(T(&)[N]))[N];
#define arraysize(arr) (sizeof(_arraySizeHelper(arr)))

#if 0
#define DCHECK(expression)\
do{									\
	if(!(expression)){				\
		assert(false,#expression);	\
	}								\
} while (false);
#else
#define DCHECK(expression)
#endif // 


#endif //SQL_SQL_DEFINE_H__
