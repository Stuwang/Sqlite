#ifndef SQL_STATEMENT_H__
#define SQL_STATEMENT_H__

#include <vector>
#include <string>

#include "sql_define.h"

namespace sql {

class Connection;

class StatementRef{
	public:
		StatementRef(Connection* conn,sqlite3_stmt* stmt);
		~StatementRef();
		bool is_valid() const { return !!stmt_;};

		Connection* connection() const {return connection_; };
		sqlite3_stmt* stmt()const { return stmt_;};

		void Close();
	private:
		sqlite3_stmt* stmt_;
		Connection * connection_;

		DISABLE_ASSIGN(StatementRef);
};

class Statement{
	public:
		Statement();
		explicit Statement(StatementRef *ref);

		void Clear();

		bool is_valid() const { return statement_->is_valid() ;}

		bool Run();

		bool Step();

		bool Reset(bool clear bound_args);

		// bind values, 0 base index
		bool BindNull(int col);
		bool BindBool(int col,bool value);
		bool BindInt(int col,int value);
		bool BindInt64(int col,int64_t value);
		bool BindDouble(int col,double value);
		bool BindCString(int col,const char* value);
		bool BindString(int col,const std::string& value);
		bool BindBlob(int col,const void * blob,int size);

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
		bool	ColumnBlobAsString(int col,std::string* o_res) const ;
		bool	ColumnBlobAsVector(int col,std::vector<char>* o_res) const ;
		bool	ColumnBlobAsVector(int col,std::vector<unsigned char>* o_res) const ;

	public:
		StatementRef* statement_;
};

}

#endif
