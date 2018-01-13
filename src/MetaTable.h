#ifndef META_TABLE_H__
#define META_TABLE_H__

#include "Connection.h"
#include "Statement.h"
#include "Transaction.h"

namespace sql{

class MetaTable{
public:
	MetaTable();
	~MetaTable();

	bool Init(Connection* conn);

	bool SetValue(const char* key,const std::string& value);
	bool SetValue(const char* key,int value);
	bool SetValue(const char* key,int64_t value);

	bool GetValue(const char* key,std::string* value);
	bool GetValue(const char* key,int* value);
	bool GetValue(const char* key,int64_t* value);

	bool DeleteKey(const char* key);

private:
	void PrepareSetStatement(Statement* statement,const char* key);
	bool PrepareGetStatement(Statement* statement,const char* key);

	Connection* db_;

	DISABLE_COPY_AND_ASSIGN(MetaTable);
};

};

#endif