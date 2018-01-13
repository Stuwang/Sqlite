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

	bool is_open() const { return state_ == TRANSACTION_AFTER_BEGIN;}
private:
	Connection* connection_;

	enum TransactionState{
		TRANSACTION_INIT,
		TRANSACTION_AFTER_BEGIN,
		TRANSACTION_COMMIT_FAIL,
		TRANSACTION_COMMIT_SUCCESS,
		TRANSACTION_COMMIT_ROWBACK,
	};

	TransactionState state_;

	DISABLE_COPY_AND_ASSIGN(Transaction);
};

}

#endif