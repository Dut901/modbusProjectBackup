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
	cout << "��ʼ��" << endl;
}

int Database::dataCon() {
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//���뻷�����
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//�������ݿ����Ӿ��

	//vc 6.0 ʹ�ã� SQLCHAR*
	ret = SQLConnect(hdbc, (SQLWCHAR*)"Database1", SQL_NTS, (SQLWCHAR*)"sa", SQL_NTS, (SQLWCHAR*)"123456", SQL_NTS);
	/*data_testΪ���õ�ODBC����Դ���ƣ���������Լ������ý����޸�*/
	if (!(ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO))
	{
		printf("�������ݿ�ʧ��!\n");
		return -1;
	}
	else {
		printf("�������ݳɹ�!\n");
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
		printf("���\t����\t��ַ\t����\n");
		while (SQLFetch(hstmt) != SQL_NO_DATA)
		{
			SQLGetData(hstmt, 1, SQL_C_CHAR, str1, 50, &len_str1);   //��ȡ��һ������
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
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//�ͷ����Ӿ��
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//�ͷŻ������
}

