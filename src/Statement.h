#ifndef SQL_STATEMENT_H__
#define SQL_STATEMENT_H__

#include <vector>
#include <string>
#include <memory>

#include "sql_define.h"

namespace sql {

class Connection;

enum class ColType {
	COLUMN_TYPE_INTEGER = SQLITE_INTEGER,
	COLUMN_TYPE_FLOAT = SQLITE_FLOAT,
	COLUMN_TYPE_TEXT = SQLITE_TEXT,
	COLUMN_TYPE_BLOB = SQLITE_BLOB,
	COLUMN_TYPE_NULL = SQLITE_NULL,
};

class StatementRef {
public:
	StatementRef(Connection* conn, sqlite3_stmt* stmt);
	
	bool is_valid() const { return !!stmt_;};

	Connection* connection() const {return connection_; };
	sqlite3_stmt* stmt()const { return stmt_;};

	void Close();
private:
	~StatementRef();
	friend class std::shared_ptr<StatementRef>;


	sqlite3_stmt* stmt_;
	Connection * connection_;

	DISABLE_COPY_AND_ASSIGN(StatementRef);
};

class Statement {
public:
	Statement();
	explicit Statement(StatementRef *ref);

	void Clear();

	bool is_valid() const { return ref_->is_valid() ;}

	bool Successed() const { return successed_;};

	bool Run();

	bool Step();

	void Reset(bool clear_bound_args);

	ColType ColumnType(int col) const ;
	ColType DeclaredColumnType(int col) const ;

	// bind values, 0 base index
	bool BindNull(int col);
	bool BindBool(int col, bool value);
	bool BindInt(int col, int value);
	bool BindInt64(int col, int64_t value);
	bool BindDouble(int col, double value);
	bool BindCString(int col, const char* value);
	bool BindString(int col, const std::string& value);
	bool BindBlob(int col, const void * blob, int size);

	// get column count
	int ColumnCount() const ;

	// get value , 0 base index
	bool	ColumnBool(int col)const;
	int		ColumnInt(int col) const ;
	int64_t ColumnInt64(int col) const ;
	double	ColumnDouble(int col) const ;
	std::string	ColumnString(int col) const ;

	// blobs
	const void* ColumnBlob(int col) const ;
	int		ColumnByteLength(int col) const;
	bool	ColumnBlobAsString(int col, std::string* o_res) const ;
	bool	ColumnBlobAsVector(int col, std::vector<char>* o_res) const ;
	bool	ColumnBlobAsVector(int col,
	                           std::vector<unsigned char>* o_res) const ;

private:

	// use for step and run
	int CheckStepError(int error) ;

	// use for bind
	bool CheckBindOk(int error) const ;

	std::shared_ptr<StatementRef> ref_;
	bool steped_;
	bool successed_;

	DISABLE_COPY_AND_ASSIGN(Statement);
};

}

#endif
