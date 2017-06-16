#include <cstring>

#include "Conection.h"

namespace sql{

bool StatementId::operator<(const StatementId& right) const {
	return number_ < right.number_ &&
		strcmp(str_,right.str_) < 0;
};

bool Connection::Open(const char* path){
	return OpenInternal(path);
};

bool Connection::OpenInMempry(){
	return OpenInternal(":memory");
};


bool Connection::BeginTransaction(){
	
};

void Connection::RollbackTransaction(){

};

bool Connection::CommitTransaction(){
	
};


}
