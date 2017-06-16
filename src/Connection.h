#ifndef SQL_CONNECTION_H__
#define SQL_CONNECTION_H__

#include <map>
#include <set>
#include "sql_define.h"

namespace sql {

class StatementRef;

class StatementId {
public:
	StatementId(const char* file, int line)
		: number_(line)
		, str_(file)
	{};

	explicit StatementId(const char* unique_name)
		: number_(-1)
		, str_(unique_name)
	{};

	bool operator<(const StatementId& right) const ;

private:
	int number_;
	const char* str_;
};

class Connection
{
public:
	Connection();
	~Connection();

	bool Open(const char* path);

	bool OpenInMempry();

	bool isOpen() const { return !!db_;};

	bool Close();

	bool BeginTransaction();
	void RollbackTransaction();
	bool CommitTransaction();

	bool Execute(const char* sql);

	int ExecuteAndReturnErrorCode(const char* sql);

	bool HasCachedStatement(const StatementId& id) const;

	// get statement


	bool DoesTableExist(const char* table) const ;

	bool DoesIndexExist(const char* index) const ;

	bool DoesViewExist(const char* view) const ;

	bool DoesColumnExist(const char* table, const char* column) const;

	int GetLastError() const ;

	const char* GetErrorMessage(int error) const ;


private:

	friend class StatementRef;

	bool OpenInternal(const char* file_name);
	bool DoesItemExist(const char* name, const char* item) const;

	void StatementRefCreate(StatementRef* ref);
	void StatementRefDelete(StatementRef* ref);

	sqlite3 *db_;
	bool in_memory_;

	typedef std::map<StatementId, StatementRef*> CachedStatementMap;
	CachedStatementMap statement_cahce_;

	typedef std::set<StatementRef*> OpendStatementSet;
	OpendStatementSet open_statement_;

};

}

#endif
