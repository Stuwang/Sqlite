#include <iostream>

#include "Connection.h"
#include "Statement.h"
#include "Transaction.h"

static const char* KCreateTable = "\
CREATE TABLE student( \
ID int PRIMARY KEY,\
name varchar(64),\
age	int\
)";

static const char* KInsert = "\
INSERT INTO student(ID,name,age) values( ? , ? , ? )";

static const char* KQuery = "SELECT * FROM student;";

int main(int argc, char const *argv[])
{
	sql::Connection conn;
	if(!conn.OpenInMempry()){
		std::cout << "error open :" << conn.GetErrorMessage();
		return -1;
	};

	if(!conn.Execute(KCreateTable)){
		std::cout << "error create table:" << conn.GetErrorMessage();
		return -1;	
	};

	{
		sql::Transaction trans(&conn);
		if(!trans.Begin()){
			std::cout << __LINE__ << " " << conn.GetErrorMessage() << std::endl;
			return -1;
		};

		sql::Statement insert(conn.GetCachedStatement(SQL_FROM_HERE,KInsert));

		for (int i = 0; i < 500; ++i){
			if(!insert.BindInt(0,i)){
				std::cout << __LINE__ << " " << conn.GetErrorMessage() << i 
				<<std::endl;
				return -1;
			};
			
			if(!insert.BindCString(1,"wangyuan")){
				std::cout << conn.GetErrorMessage() << std::endl;	
				return -1;
			};

			if(!insert.BindInt(2,23)){
				std::cout << conn.GetErrorMessage() << std::endl;		
				return -1;
			};

			if(!insert.Run()){
				std::cout << "error " << std::endl;
				break;
			}

			insert.Reset(true);
		}

		if(!trans.Commit()){
			std::cout << "commit error " << conn.GetErrorMessage();
			trans.Rollback();
			return -1;
		};
	}

	{
		sql::Statement query(conn.GetCachedStatement(SQL_FROM_HERE,KQuery));
		while(query.Step()){
			std::cout 
				<< " id   :" << query.ColumnInt(0)
				<< " name :" << query.ColumnString(1)
				<< " age  :" << query.ColumnInt(2)
				<< std::endl;
		}
	}

	conn.Close();

	return 0;
}