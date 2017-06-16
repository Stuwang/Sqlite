#include "Statement.h"
#include "Connection.h"

namespace sql {

StatementRef::StatementRef(Connection* conn,
                           sqlite3_stmt* stmt)
    : stmt_(stmt)
    , connection_(connection)
{
    if (connection_) {
        connection_->StatementRefCreate(this);
    }

}

void StatementRef::Close() {
    if (stmt_) {
        sqlite3_finalize(stmt_);
        stmt_ = nullptr;
    }
    connection_ = nullptr;
}

StatementRef::~StatementRef() {
    if (connection_) {
        connection_->StatementRefDelete(this);
    }
    Close();
}

// Statement

Statement::Statement()
    : ref_()
    , steped_(false)
    , succedded_(false)
{
}

explicit Statement::Statement(StatementRef *ref)
    : ref_(ref)
    , steped_(false)
    , succedded_(false)
{
}

void Statement::Clear() {

}

bool Statement::Run() {
    DCHECK(!steped_);
    return CheckStepError(sqlite3_step(ref_->stmt())) == SQLITE_DONE;
}

bool Statement::Step() {
    DCHECK(!steped_);
    return CheckStepError(sqlite3_step(ref_->stmt())) == SQLITE_ROW;
}

void Statement::Reset(bool clear_bound_args) {

    if (clear_bound_args)
        sqlite3_clear_bindings(ref_->stmt());
    const int rc = sqlite3_reset(ref_->stmt());
    if (rc == SQLITE_OK && ref_->connection()) {

    }

    succeeded_ = false;
    stepped_ = false;
}

ColType Statement::ColumnType(int col) const {
    return static_cast<ColType>(sqlite3_column_type(ref_->stmt(), col));
}

ColType Statement::DeclaredColumnType(int col) const {
    std::string column_type =
        // here should cast string to lower case
        // base::ToLowerASCII(
        sqlite3_column_decltype(ref_->stmt(), col)
        // )
        ;

    if (column_type == "integer")
        return COLUMN_TYPE_INTEGER;
    else if (column_type == "float")
        return COLUMN_TYPE_FLOAT;
    else if (column_type == "text")
        return COLUMN_TYPE_TEXT;
    else if (column_type == "blob")
        return COLUMN_TYPE_BLOB;

    return COLUMN_TYPE_NULL;
}

// bind values, 0 base index
bool Statement::BindNull(int col) {
    DCHECK(!steped_);
    return CheckBindOk(sqlite3_bind_null(ref_->stmt(), col + 1));
}
bool Statement::BindBool(int col, bool value) {
    return BindInt(col, value ? 1 : 0);
}
bool Statement::BindInt(int col, int value) {
    DCHECK(!steped_);
    return CheckBindOk(sqlite3_bind_int(ref_->stmt(), col + 1, value));
}
bool Statement::BindInt64(int col, int64_t value) {
    DCHECK(!steped_);
    return CheckBindOk(sqlite3_bind_int64(ref_->stmt(), col + 1, value));
}
bool Statement::BindDouble(int col, double value) {
    DCHECK(!steped_);
    return CheckBindOk(sqlite3_bind_double(ref_->stmt(), col + 1, value));
}
bool Statement::BindCString(int col, const char* value) {
    DCHECK(!steped_);
    return CheckBindOk(
               sqlite3_bind_text(ref_->stmt(),
                                 col + 1,
                                 value,
                                 -1,
                                 SQLITE_TRANSIENT));
}
bool Statement::BindString(int col, const std::string& value) {
    DCHECK(!steped_);
    return CheckBindOk(
               sqlite3_bind_text(ref_->stmt(),
                                 col + 1,
                                 value.data(),
                                 value.size(),
                                 SQLITE_TRANSIENT));
}
bool Statement::BindBlob(int col, const void * blob, int size) {
    DCHECK(!steped_);
    return CheckBindOk(
               sqlite3_bind_blob(ref_->stmt(),
                                 col + 1,
                                 blob,
                                 size,
                                 SQLITE_TRANSIENT));
}

// get column count
int Statement::ColumnCount() const {
    return sqlite3_column_count(ref_->stmt());
}

// get value , 0 base index
bool Statement::ColumnBool(int col)const {
    DCHECK(is_valid());
    return !!ColumnInt(col);
}

int Statement::ColumnInt(int col) const {
    DCHECK(is_valid());
    return sqlite3_column_int(ref_->stmt(), col);
}

int64_t Statement::ColumnInt64(int col) const {
    DCHECK(is_valid());
    return sqlite3_column_int64(ref_->stmt(), col);
}
double Statement::ColumnDouble(int col) const {
    DCHECK(is_valid());
    return sqlite3_column_double(ref_->stmt(), col);
}

std::string Statement::ColumnString(int col) const {
    DCHECK(is_valid());
    const char* str = reinterpret_cast<const char*>(
                          sqlite3_column_text(ref_->stmt(), col));
    int len = sqlite3_column_bytes(ref_->stmt(), col);

    std::string result;
    if (str && len > 0)
        result.assign(str, len);
    return result;
}

// blobs
const void* Statement::ColumnBlob(int col) const {
    DCHECK(is_valid());
    return sqlite3_column_blob(ref_->stmt(), col);
}

int Statement::ColumnByteLength(int col) const {
    DCHECK(is_valid());
    return sqlite3_column_bytes(ref_->stmt(), col);
}

bool Statement::ColumnBlobAsString(int col, std::string* o_res) const {
    DCHECK(is_valid());
    const void *p = reinterpret_cast<const char*>(ColumnBlob(col));
    int size = ColumnByteLength(col);
    o_res->resize(len);
    if ( p && size && o_res->size() != len)
        return false;
    o_res->assign(reinterpret_cast<const char*>(p), szie);
    return true;
}

bool Statement::ColumnBlobAsVector(int col, std::vector<char>* o_res) const {
    DCHECK(is_valid());
    const void *p = reinterpret_cast<const char*>(ColumnBlob(col));
    int size = ColumnByteLength(col);
    if (p && size > 0) {
        o_res->resize(size);
        memcpy(&(*o_res)[0], p, len);
    }
    return true;
}
bool Statement::ColumnBlobAsVector(
    int col,
    std::vector<unsigned char>* o_res) const {
    return ColumnBlobAsVector(
               col, reinterpret_cast<std::vector<char>*>(o_res));
}

int Statement::CheckStepError(int error) {
    succeeded_ = (err == SQLITE_OK ||
                  err == SQLITE_ROW ||
                  err == SQLITE_DONE);
    steped_ = true;

    if (!succeeded_ && ref_.get() && ref_->connection() ) {
        // should report error to connection
    }
    return error;
};

bool Statement::CheckBindOk(int error) const {
    if (error == SQLITE_RANGE) {
        // LOG BIND OUT OF RANGE
    }
    return error == SQLITE_OK;
};

}