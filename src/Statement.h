#ifndef SQL_STATEMENT_H__
#define SQL_STATEMENT_H__

#include <memory>
#include <string>
#include <vector>

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

class StatementImpl {
 public:
  StatementImpl(Connection* conn, sqlite3_stmt* stmt);

  bool is_valid() const { return !!stmt_; };

  Connection* connection() const { return connection_; };
  sqlite3_stmt* stmt() const { return stmt_; };

  void Close();

  ~StatementImpl();

 private:
  sqlite3_stmt* stmt_;
  Connection* connection_;

  DISABLE_COPY_AND_ASSIGN(StatementImpl)
};

typedef StatementImpl* StatementRef;

class Statement {
 public:
  Statement();
  explicit Statement(const StatementRef& ref);

  void Clear();

  bool is_valid() const { return ref_ && ref_->is_valid(); }

  bool Successed() const { return successed_; };

  bool Run();

  bool Step();

  void Reset(bool clear_bound_args);

  void ResetStatementRef(const StatementRef& ref);

  const char* GetSQLStatement() const { return sqlite3_sql(ref_->stmt()); }

  ColType ColumnType(int col) const;
  ColType DeclaredColumnType(int col) const;

  // bind values, 0 base index
  bool BindNull(int col);
  bool BindBool(int col, bool value);
  bool BindInt(int col, int value);
  bool BindInt64(int col, int64_t value);
  bool BindDouble(int col, double value);
  bool BindCString(int col, const char* value);
  bool BindString(int col, const std::string& value);
  bool BindBlob(int col, const void* blob, int size);

  // get column count
  int ColumnCount() const;

  // get value , 0 base index
  bool ColumnBool(int col) const;
  int ColumnInt(int col) const;
  int64_t ColumnInt64(int col) const;
  double ColumnDouble(int col) const;
  std::string ColumnString(int col) const;

  // blobs
  const void* ColumnBlob(int col) const;
  int ColumnByteLength(int col) const;
  bool ColumnBlobAsString(int col, std::string* o_res) const;
  bool ColumnBlobAsVector(int col, std::vector<char>* o_res) const;
  bool ColumnBlobAsVector(int col, std::vector<unsigned char>* o_res) const;

 private:
  // use for step and run
  int CheckStepError(int error);

  // use for bind
  bool CheckBindOk(int error) const;

  StatementRef ref_;
  bool steped_;
  bool successed_;

  DISABLE_COPY_AND_ASSIGN(Statement);
};

namespace internal {

inline bool BindValue(Statement& statement, int index, bool value) {
  return statement.BindBool(index, value);
}
inline bool BindValue(Statement& statement, int index, int value) {
  return statement.BindInt(index, value);
}
inline bool BindValue(Statement& statement, int index, int64_t value) {
  return statement.BindInt64(index, value);
}
inline bool BindValue(Statement& statement, int index, double value) {
  return statement.BindDouble(index, value);
}
inline bool BindValue(Statement& statement, int index, const char* value) {
  return statement.BindCString(index, value);
}
inline bool BindValue(Statement& statement, int index,
                      const std::string& value) {
  return statement.BindString(index, value);
}

inline bool SqlBindHelper(Statement& statement, int index) { return true; }

template <class T, class... Args>
bool SqlBindHelper(Statement& statement, int index, T&& value, Args&&... args) {
  if (BindValue(statement, index, value)) {
    return SqlBindHelper(statement, index + 1, args...);
  } else {
    statement.Reset(true);
    return false;
  }
}

}  // namespace internal

template <class... Args>
bool SqlBind(Statement& statement, Args&&... args) {
  return internal::SqlBindHelper(statement, 0, args...);
}

}  // namespace sql

#endif
