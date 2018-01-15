#include <iostream>
#include <fstream>

#include "Connection.h"
#include "Statement.h"
#include "Transaction.h"
#include "MetaTable.h"

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
	if (!conn.OpenInMempry()) {
		std::cout << "error open :" << conn.GetErrorMessage();
		return -1;
	};

	if (!conn.Execute(KCreateTable)) {
		std::cout << "error create table:" << conn.GetErrorMessage();
		return -1;
	};

	if (!sql::ExecuteSql(&conn, "DROP  TABLE  student")) {
		std::cout << "error drop table:" << conn.GetErrorMessage();
		return -1;
	}

	if (!sql::ExecuteSql(&conn, KCreateTable)) {
		std::cout << "error create table:" << conn.GetErrorMessage();
		return -1;
	}

	{
		sql::Transaction trans(&conn);
		if (!trans.Begin()) {
			std::cout << __LINE__ << " " << conn.GetErrorMessage() << std::endl;
			return -1;
		};

		sql::Statement insert(conn.GetCachedStatement(SQL_FROM_HERE, KInsert));

		for (int i = 0; i < 500; ++i) {
			if (false) {
				if (!insert.BindInt(0, i)) {
					std::cout << __LINE__ << " " << conn.GetErrorMessage() << i
						<< std::endl;
					return -1;
				};

				if (!insert.BindCString(1, "wangyuan")) {
					std::cout << conn.GetErrorMessage() << std::endl;
					return -1;
				};

				if (!insert.BindInt(2, 23)) {
					std::cout << conn.GetErrorMessage() << std::endl;
					return -1;
				};
			} else {
				if (!sql::SqlBind(insert, i, "wangyuan", 23)) {
					std::cout << conn.GetErrorMessage() << std::endl;
					return -1;
				}
			}

			if (!insert.Run()) {
				std::cout << "error " << std::endl;
				break;
			}

			insert.Reset(true);
		}

		if (!trans.Commit()) {
			std::cout << "commit error " << conn.GetErrorMessage();
			trans.Rollback();
			return -1;
		};
	}

	{
		sql::Statement query(conn.GetCachedStatement(SQL_FROM_HERE, KQuery));

		std::fstream file;
		file.open("a.txt", std::ios::app);

		while (query.Step()) {
			std::cout
				//file
				<< " id   :" << query.ColumnInt(0)
				<< " name :" << query.ColumnString(1)
				<< " age  :" << query.ColumnInt(2)
				<< std::endl;
		}

		file.close();
	}

	{
		// meta table
		sql::MetaTable meta;
		if (!meta.Init(&conn)) {
			std::cout << "create meta table error " << std::endl;
		};
		{
			using DataType = int;
			DataType data = 18;
			meta.SetValue("age", data);
			DataType o_data = 0;
			meta.GetValue("age", &o_data);
			std::cout << "get data " << o_data << std::endl;
		}
		{
			using DataType = int64_t;
			DataType data = 16;
			meta.SetValue("age", data);
			DataType o_data = 0;
			meta.GetValue("age", &o_data);
			std::cout << "get data " << o_data << std::endl;
		}
		{
			using DataType = std::string;
			DataType data = "123456";
			meta.SetValue("age", data);

			if (sql::ExecuteSql(&conn, "DELETE FROM meta WHERE key=?", "age")) {
				std::cout << "delete ok" << std::endl;
			}

			DataType o_data;
			meta.GetValue("age", &o_data);
			std::cout << "get data " << o_data << std::endl;
		}
		meta.DeleteKey("age");

	}

	conn.Close();

	return 0;
}