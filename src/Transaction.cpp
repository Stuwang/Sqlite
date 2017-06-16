#include "Transaction.h"
#include "Connection.h"


namespace sql {

Transaction::Transaction(Connection* conn)
	: connection_(conn)
	, is_open_(false) {
}

Transaction::~Transaction() {
	if (is_open_) {
		connection_->RollbackTransaction();
	}
}

bool Transaction::Begin() {
	DCHECK(!is_open_);
	is_open_ = connection_->BeginTransaction();
	return is_open_;
}

bool Transaction::Commit() {
	DCHECK(is_open_);
	is_open_ = connection_->CommitTransaction();
	return !is_open_;
}

void Transaction::Rollback() {
	DCHECK(is_open_);
	connection_->RollbackTransaction();
	is_open_ = false;
}

}
