#ifndef SQL_TRANSACTION_H__
#define SQL_TRANSACTION_H__

namespace sql {

class Connection;

class Transaction
{
public:
	Transaction(Connection* conn);
	~Transaction();

	bool Begin();

	bool Commit();

	void Rollback()

	bool is_open() const { return is_open_;}
private:
	Connection* connection_;
	bool is_open_;

	DISABLE_COPY_AND_ASSIGN(Transaction);
};
}

#endif