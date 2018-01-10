#include <cstring>

#include "Connection.h"

namespace sql {

bool StatementId::operator<(const StatementId& right) const {
	if (number_ != right.number_)
		 return number_ < right.number_;
	return strcmp(str_, right.str_) < 0;
}

Connection::Connection()
	: db_(NULL)
	, in_memory_(false)
{}

Connection::~Connection() {
	Close();
}

bool Connection::Open(const char* path) {
	return OpenInternal(path);
}

bool Connection::OpenInMempry() {
	in_memory_ = true;
	return OpenInternal(":memory:");
}

void Connection::Close() {
	CloseInternal(false);
}

bool Connection::BeginTransaction() {
	Statement begin(GetCachedStatement(SQL_FROM_HERE, "BEGIN TRANSACTION"));
	return begin.Run();
}

void Connection::RollbackTransaction() {
	Statement rollback(GetCachedStatement(SQL_FROM_HERE, "ROLLBACK"));
	rollback.Run();
}

bool Connection::CommitTransaction() {
	Statement commit(GetCachedStatement(SQL_FROM_HERE, "COMMIT"));
	return commit.Run();
}

bool Connection::Execute(const char * sql) {
	if (!db_) {
		//DLOG_IF(FATAL, !poisoned_) << "Illegal use of connection without a db";
		return false;
	}

	int error = ExecuteAndReturnErrorCode(sql);
	if (error != SQLITE_OK)
		OnSqliteError(error, NULL, sql);

	// This needs to be a FATAL log because the error case of arriving here is
	// that there's a malformed SQL statement. This can arise in development if
	// a change alters the schema but not all queries adjust.  This can happen
	// in production if the schema is corrupted.
	if (error == SQLITE_ERROR) {
		//DLOG(FATAL) << "SQL Error in " << sql << ", " << GetErrorMessage();
	}
	return error == SQLITE_OK;
}

int Connection::ExecuteAndReturnErrorCode(const char * sql) {
	int rc = SQLITE_OK;
	while ((rc == SQLITE_OK) && *sql) {
		sqlite3_stmt *stmt = NULL;
		const char *leftover_sql;

		rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, &leftover_sql);
		sql = leftover_sql;

		if (rc != SQLITE_OK)
			break;

		if (!stmt)
			continue;

		// Save for use after statement is finalized.
		const bool read_only = !!sqlite3_stmt_readonly(stmt);

		while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {}

		rc = sqlite3_finalize(stmt);
		if (rc == SQLITE_OK) {
			//RecordOneEvent(Connection::EVENT_STATEMENT_SUCCESS);
		}

		// sqlite3_exec() does this, presumably to avoid spinning the parser for
		// trailing whitespace.
		// TODO(shess): Audit to see if this can become a DCHECK.
		while (iswspace(*sql)) {
			sql++;
		}
	}
	return rc;
}

bool Connection::HasCachedStatement(const StatementId & id) const {
	return statement_cahce_.find(id) == statement_cahce_.end();
}

StatementRef Connection::GetCachedStatement(const StatementId & id, const char * sql) {
	CachedStatementMap::iterator it = statement_cahce_.find(id);
	if (it != statement_cahce_.end()) {
		DCHECK(it->second->is_valid());
		sqlite3_reset(it->second->stmt());
		return it->second;
	}
	StatementRef statement = GetUniqueStatement(sql);
	if (statement->is_valid()) {
		statement_cahce_[id] = statement;
	}
	return statement;
}

StatementRef Connection::GetUniqueStatement(const char * sql) {

	return GetStatementImpl(this, sql);
}

bool Connection::IsSQLValid(const char * sql) {
	if (!db_) {
		// log
		return false;
	}

	sqlite3_stmt* stmt = NULL;
	if (sqlite3_prepare_v2(db_, sql, -1, &stmt, NULL) != SQLITE_OK)
		return false;

	sqlite3_finalize(stmt);
	return true;
}

bool Connection::DoesTableExist(const char * table) const {
	return DoesTableOrIndexExist(table, "table");
}

bool Connection::DoesIndexExist(const char * index) const {
	return DoesTableOrIndexExist(index, "index");
}

bool Connection::DoesTableOrIndexExist(const char * name, const char * type) const {
	const char* kSql =
		"SELECT name FROM sqlite_master WHERE type=? AND name=? COLLATE NOCASE";
	Statement statement(GetUnTrackedStatement(kSql));
	if (!statement.is_valid()) {
		return false;
	}

	statement.BindCString(0, type);
	statement.BindCString(1, name);
	return statement.Step();
}

bool Connection::DoesColumnExist(const char * table, const char * column) const {
	std::string sql = "PRAGMA TABLE_INFO(";
	sql.append(table);
	sql.append(")");

	Statement statement(GetUnTrackedStatement(sql.c_str()));

	if (!statement.is_valid()) {
		return false;
	}

	while (statement.Step()) {
		if (column == statement.ColumnString(1)) {
			return true;
		}
	}
	return false;
}

int Connection::GetErrorCode() const {
	if (!db_) {
		return SQLITE_ERROR;
	}
	return sqlite3_errcode(db_);
}

const char * Connection::GetErrorMessage() const {
	if (!db_) {
		return "sql::Connection has no connection.";
	}
	return sqlite3_errmsg(db_);
}

void Connection::OnSqliteError(int rc, Statement * statement, const char * sql) const {
	if (!sql && statement) {
		sql = statement->GetSQLStatement();
	}
	if (!sql) {
		sql = "-- unknow.";
	}
	// log

	if (error_cb_) {
		error_cb_(rc, statement);
	}
}

bool Connection::OpenInternal(const char * file_name)
{
	if (db_) {
		// should log
		return false;
	}
	sqlite3_initialize();
	int err = sqlite3_open(file_name, &db_);
	if (SQLITE_OK != err) {
		err = sqlite3_extended_errcode(db_);
		OnSqliteError(err, NULL, "-- sqlite3_open()");
		Close();
		return false;
	}
	return true;
}

void Connection::CloseInternal(bool forced) {
	// at first , we close statement
	int rc = sqlite3_close(db_);
	if (SQLITE_OK != rc) {
		// log

	}
	db_ = NULL;
}

StatementRef Connection::GetStatementImpl(sql::Connection * tracking_db, const char * sql) const {
	if (!db_) {
		return StatementRef{ new StatementImpl(NULL,NULL) };
	}
	sqlite3_stmt* stmt = NULL;
	int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, NULL);
	if (SQLITE_OK != rc) {
		if (SQLITE_ERROR == rc) {
			// log
			//sqlite3_errmsg(db_);
		}
		OnSqliteError(rc, NULL, sql);
		return StatementRef{ new StatementImpl(NULL,NULL) };
	}
	return StatementRef{ new StatementImpl(tracking_db,stmt) };
}


void Connection::StatementRefCreate(const StatementRef& ref) {
	DCHECK((open_statement_.find(ret) == open_statement_.end()));
	open_statement_.insert(ref);
}

void Connection::StatementRefDelete(const StatementRef & ref) {
	DCHECK((open_statement_.find(ret) != open_statement_.end()));
	open_statement_.erase(ref);
}

}
