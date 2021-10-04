#include<stdio.h>
#include <iostream>
#include<windows.h>
#include<sql.h>
#include<sqlext.h>
#include<sqltypes.h>
#include "database.h"
#include "include\database.h"
using namespace std;

SQLRETURN ret;
SQLHENV henv;
SQLHDBC hdbc;
SQLHSTMT hstmt;



Database::Database()
{
	cout << "初始化" << endl;
}

int Database::dataCon() {
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//申请环境句柄
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//申请数据库连接句柄

	//vc 6.0 使用： SQLCHAR*
	ret = SQLConnect(hdbc, (SQLWCHAR*)"Database1", SQL_NTS, (SQLWCHAR*)"sa", SQL_NTS, (SQLWCHAR*)"123456", SQL_NTS);
	/*data_test为配置的ODBC数据源名称，这里根据自己的配置进行修改*/
	if (!(ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO))
	{
		printf("连接数据库失败!\n");
		return -1;
	}
	else {
		printf("连接数据成功!\n");
	}
}

int Database::getData()
{
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

	SQLWCHAR sql2[] = L"select num,name,addr,age from test_db.dbo.test";


	ret = SQLExecDirect(hstmt, sql2, SQL_NTS);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		SQLCHAR str1[20], str2[20], str3[20], str4[20];
		//	VC 6.0
		SQLINTEGER len_str1, len_str2, len_str3, len_str4;
		//	SQLLEN len_str1, len_str2, len_str3;
		printf("编号\t姓名\t地址\t年龄\n");
		while (SQLFetch(hstmt) != SQL_NO_DATA)
		{
			SQLGetData(hstmt, 1, SQL_C_CHAR, str1, 50, &len_str1);   //获取第一列数据
			SQLGetData(hstmt, 2, SQL_C_CHAR, str2, 50, &len_str2);
			SQLGetData(hstmt, 3, SQL_C_CHAR, str3, 50, &len_str3);
			SQLGetData(hstmt, 4, SQL_C_CHAR, str4, 50, &len_str4);
			printf("%s\t%s\t%s\t%s\n", str1, str2, str3, str4);
		}
	}
	return 0;
}

void Database::freeCon()
{
}

void freeCon() {
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄
}

