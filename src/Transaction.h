#ifndef SQL_TRANSACTION_H__
#define SQL_TRANSACTION_H__

#include "sql_define.h"

namespace sql {

class Connection;

class Transaction
{
public:
	explicit Transaction(Connection* conn);
	~Transaction();

	bool Begin();

	bool Commit();

	void Rollback();

	bool is_open() const { return is_open_;};
private:
	Connection* connection_;
	bool is_open_;

	DISABLE_COPY_AND_ASSIGN(Transaction);
};
}

#endif