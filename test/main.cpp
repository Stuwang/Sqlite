#include <iostream>
#include <fstream>

#include "Connection.h"
#include "Statement.h"
#include "Transaction.h"


static const char* KCreateTable = "\
CREATE TABLE student( \
	id int PRIMARY KEY,\
	name varchar(200),\
	age int\
);\
";
static const char* KInsertSql = "\
INSERT INTO student(id,name, age) values(?, ? , ? )";

static const char* KQuery = "select * from student;";

int main() {

	std::ios::sync_with_stdio(false);

	sql::Connection conn;
	if (!conn.Open("a.db")) {
		std::cout << "open error ";
		return -1;
	};

	conn.IsSQLValid(KCreateTable);
	conn.ExecuteAndReturnErrorCode(KCreateTable);

	{
		sql::Transaction trans(&conn);

		trans.Begin();

		sql::Statement statement(conn.GetCachedStatement(SQL_FROM_HERE, KInsertSql));

		for (size_t i = 0; i < 10* 10000; i++) {
			statement.BindInt(0, i);
			statement.BindCString(1, "wangyuan");
			statement.BindInt(2, 25);
			if (!statement.Run()) {
				break;
			};
			statement.Reset(true);
		}

		if (!trans.Commit()) {
			trans.Rollback();
		};
	}
	{
		std::fstream file;
		file.open("a.txt", std::ios::app);

		sql::Statement query(conn.GetUniqueStatement(KQuery));
		std::cout << query.ColumnCount() << std::endl;
		int count = 0;
		while (query.Step()) {
			count++;
			file
				<< "  id    :" << query.ColumnInt(0)
				<< "  name  :" << query.ColumnString(1)
				<< "  age   :" << query.ColumnInt(2)
				<< std::endl;
		}
		std::cout << "read rows :" << count << std::endl;
		file.close();
	}

	return 0;
}