#include "Statement.h"
#include "Connection.h"

namespace sql{

StatementRef::StatementRef(Connection* conn,
    sqlite3_stmt* stmt)
    : stmt_(stmt)
    , connection_(connection)
{
    if(connection_){
        connection_->StatementRefCreate(this);
    }

}

void StatementRef::Close(){
    if(stmt_){
        sqlite3_finalize(stmt_);
        stmt_ = nullptr;
    }
    connection_ = nullptr;
};

StatementRef::~StatementRef(){
    if(connection_){
        connection_->StatementRefDelete(this);
    }
    Close();
};



}