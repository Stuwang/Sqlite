#ifndef SQL_CONNECTION_H__
#define SQL_CONNECTION_H__

#include <functional>
#include <map>
#include <set>
#include "Statement.h"
#include "sql_define.h"

namespace sql {

class StatementId {
 public:
  StatementId(const char* file, int line) : number_(line), str_(file){};

  explicit StatementId(const char* unique_name)
      : number_(-1), str_(unique_name){};

  bool operator<(const StatementId& right) const;

 private:
  int number_;
  const char* str_;
};

#define SQL_FROM_HERE sql::StatementId(__FILE__, __LINE__)

class Connection {
 public:
  typedef std::function<void(int, const char*, Statement*)> ErrorCallback;

  Connection();
  ~Connection();

  bool Open(const char* path);

  bool OpenInMempry();

  bool isOpen() const { return !!db_; };

  void Close();

  void SetErrorCallback(const ErrorCallback& cb) { this->error_cb_ = cb; }
  bool hasErrorCallback() const { return !!error_cb_; };
  void ResetErrorCallback() { error_cb_ = ErrorCallback(); }

  bool BeginTransaction();
  void RollbackTransaction();
  bool CommitTransaction();

  bool Execute(const char* sql);

  int ExecuteAndReturnErrorCode(const char* sql);

  StatementRef GetCachedStatement(const StatementId& id, const char* sql);

  bool HasCachedStatement(const StatementId& id) const;

  StatementRef GetUniqueStatement(const char* sql);

  bool IsSQLValid(const char* sql);

  bool DoesTableExist(const char* table) const;
  bool DoesIndexExist(const char* index) const;

  bool DoesColumnExist(const char* table, const char* column) const;

  int GetErrorCode() const;

  const char* GetErrorMessage() const;

 private:
  friend class StatementImpl;

  void OnSqliteError(int, Statement*, const char*) const;

  bool OpenInternal(const char* file_name);
  void CloseInternal(bool forced);

  StatementRef GetStatementImpl(sql::Connection* tracking_db,
                                const char* sql) const;
  StatementRef GetUnTrackedStatement(const char* sql) const {
    return GetStatementImpl(NULL, sql);
  };

  bool DoesTableOrIndexExist(const char* name, const char* item) const;

  void StatementRefCreate(const StatementRef& ref);
  void StatementRefDelete(const StatementRef& ref);

  sqlite3* db_;
  bool in_memory_;

  ErrorCallback error_cb_;

  typedef std::map<StatementId, StatementRef> CachedStatementMap;
  CachedStatementMap statement_cahce_;

  typedef std::set<StatementRef> OpendStatementSet;
  OpendStatementSet open_statement_;
};

namespace internal {}

template <class... Args>
bool ExecuteSql(Connection* conn, const char* sql, Args&&... args) {
  Statement statement(conn->GetUniqueStatement(sql));
  if (!statement.is_valid()) {
    return false;
  }
  if (!SqlBind(statement, args...)) {
    return false;
  };
  return statement.Run();
}

}  // namespace sql

#endif
