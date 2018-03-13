#include "MetaTable.h"

namespace sql {

MetaTable::MetaTable() : db_(NULL) {}

MetaTable::~MetaTable() {}

bool MetaTable::Init(Connection* conn) {
  DCHECK(conn);
  db_ = conn;

  if (!db_->DoesTableExist("meta")) {
    if (!db_->Execute("CREATE TABLE meta"
                      "(key LONGVARCHAR NOT NULL UNIQUE PRIMARY KEY, value "
                      "LONGVARCHAR)")) {
      return false;
    }
  }
  return true;
}

bool MetaTable::SetValue(const char* key, const std::string& value) {
  Statement s;
  PrepareSetStatement(&s, key);
  s.BindString(1, value);
  return s.Run();
}

bool MetaTable::SetValue(const char* key, int value) {
  Statement s;
  PrepareSetStatement(&s, key);
  s.BindInt(1, value);
  return s.Run();
}

bool MetaTable::SetValue(const char* key, int64_t value) {
  Statement s;
  PrepareSetStatement(&s, key);
  s.BindInt64(1, value);
  return s.Run();
}

bool MetaTable::GetValue(const char* key, std::string* value) {
  Statement s;
  if (!PrepareGetStatement(&s, key)) {
    return false;
  }

  *value = s.ColumnString(0);
  return true;
}

bool MetaTable::GetValue(const char* key, int* value) {
  Statement s;
  if (!PrepareGetStatement(&s, key)) {
    return false;
  }

  *value = s.ColumnInt(0);
  return true;
}

bool MetaTable::GetValue(const char* key, int64_t* value) {
  Statement s;
  if (!PrepareGetStatement(&s, key)) {
    return false;
  }

  *value = s.ColumnInt64(0);
  return true;
}

bool MetaTable::DeleteKey(const char* key) {
  DCHECK(db_);
  Statement s(
      db_->GetCachedStatement(SQL_FROM_HERE, "DELETE FROM meta WHERE key=?"));
  s.BindCString(0, key);
  return s.Run();
}

void MetaTable::PrepareSetStatement(Statement* statement, const char* key) {
  DCHECK(db_ && statement);
  statement->ResetStatementRef(db_->GetCachedStatement(
      SQL_FROM_HERE, "INSERT OR REPLACE INTO meta (key,value) VALUES (?,?)"));
  statement->BindCString(0, key);
}

bool MetaTable::PrepareGetStatement(Statement* statement, const char* key) {
  DCHECK(db_ && statement);
  statement->ResetStatementRef(db_->GetCachedStatement(
      SQL_FROM_HERE, "SELECT value FROM meta WHERE key=?"));
  statement->BindCString(0, key);
  return statement->Step();
}

}  // namespace sql