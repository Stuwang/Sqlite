#include "Transaction.h"
#include "Connection.h"


namespace sql {

Transaction::Transaction(Connection* conn)
	: connection_(conn)
	, state_(TRANSACTION_INIT) {
}

Transaction::~Transaction() {
	if (state_ == TRANSACTION_COMMIT_FAIL) {
		connection_->RollbackTransaction();
		state_ = TRANSACTION_COMMIT_ROWBACK;
	}
}

bool Transaction::Begin() {
	DCHECK(state_ == TRANSACTION_INIT);
	if(connection_->BeginTransaction()){
		state_ = TRANSACTION_AFTER_BEGIN;
		return true;
	};
	return false;
}

bool Transaction::Commit() {
	DCHECK(state_ == TRANSACTION_AFTER_BEGIN);
	if(connection_->CommitTransaction()){
		state_ = TRANSACTION_COMMIT_SUCCESS;
		return true;
	}else{
		state_ = TRANSACTION_COMMIT_FAIL;
		return false;
	}
}

void Transaction::Rollback() {
	DCHECK(state_ == TRANSACTION_COMMIT_FAIL);
	connection_->RollbackTransaction();
	state_ = TRANSACTION_COMMIT_ROWBACK;
}

}
