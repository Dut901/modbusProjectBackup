#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <odbcss.h>
#include <conio.h>
#include <tchar.h>
#include <stdlib.h>
#include <sal.h>
#include <iostream>
#include "include/database.h"


using namespace std;
#define MAXBUFLEN 255
SQLHENV henv = SQL_NULL_HENV;
SQLHDBC hdbc1 = SQL_NULL_HDBC;
SQLHSTMT hstmt1 = SQL_NULL_HSTMT;
UCHAR szDSN[15] = "Database2"; //����Դ����
UCHAR userID[6] = "sa";//���ݿ��û�ID
UCHAR passWORD[29] = "123456";//�û�����






RETCODE databaseCon()
{
	//1.��������Դ  
	//1.�������   

	RETCODE retcode;
	RETCODE retcode1;
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	/*retcode1 = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	retcode1 = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);*/
	//2.���Ӿ��    
	retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc1);
	retcode = SQLConnect(hdbc1, szDSN, SQL_NTS, userID, SQL_NTS, passWORD, SQL_NTS);
	/*retcode1 = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc1);
	retcode1 = SQLConnect(hdbc1, szDSN, SQL_NTS, userID, SQL_NTS, passWORD, SQL_NTS);*/
	//�ж������Ƿ�ɹ�   
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
	{
		printf("����ʧ��!\n");
	}
	else
	{
		printf("���ӳɹ�!\n");
		
	}
	//�ж������Ƿ�ɹ�   
	/*if ((retcode1 != SQL_SUCCESS) && (retcode1 != SQL_SUCCESS_WITH_INFO))
	{
		printf("����ʧ��!\n");
	}
	else
	{
		printf("���ӳɹ�!\n");

	}*/
	return retcode;
}

void dbSqlSePr(char *sql, RETCODE retcode) {
	/*
		1.����һ�������(statement handle)
		2.����SQL���
		3.ִ�����
		4.�������
		*/
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);
	retcode = SQLExecDirect(hstmt1, (SQLCHAR*)sql, strlen(sql));
	char* sid = new char[256];
	char* sspeed = new char[256];
	char* spulse = new char[256];
	char* stime = new char[256];
	char* sspeed_in = new char[256];
	char* spulse_in = new char[256];
	SQLLEN columnlen = 0;

	SQLBindCol(hstmt1, 1, SQL_CHAR, sid, 256, &columnlen);
	SQLBindCol(hstmt1, 2, SQL_CHAR, sspeed, 256, &columnlen);
	SQLBindCol(hstmt1, 3, SQL_CHAR, spulse, 256, &columnlen);
	SQLBindCol(hstmt1, 4, SQL_CHAR, sspeed_in, 256, &columnlen);
	SQLBindCol(hstmt1, 5, SQL_CHAR, spulse_in, 256, &columnlen);
	SQLBindCol(hstmt1, 6, SQL_CHAR, stime, 256, &columnlen);

	if (retcode < 0)
	{
		cout << "û��ִ�����" << endl;
	}
	retcode = SQLFetch(hstmt1);

	while (retcode == SQL_ROW_SUCCESS || retcode == SQL_ROW_SUCCESS_WITH_INFO)
	{
		if (retcode == SQL_ROW_SUCCESS || retcode == SQL_ROW_SUCCESS_WITH_INFO)
		{
			cout << sid << "\t" << sspeed << "\t" << spulse << "\t" << stime << endl;
			retcode = SQLFetch(hstmt1);
		}
	}
	//�ͷ������ 
	retcode = SQLCloseCursor(hstmt1);
	retcode = SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);
}

void dbSqlEdit(char* sql, RETCODE retcode) {
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);
	SQLINTEGER rowcount[1];
	try {
		retcode = SQLExecDirect(hstmt1, (SQLCHAR*)sql, strlen(sql));
		retcode = SQLRowCount(hstmt1, rowcount);
		if (retcode == SQL_ROW_SUCCESS || retcode == SQL_ROW_SUCCESS_WITH_INFO)
		{
			//printf("�ɹ���%d���ܵ�Ӱ��\n", rowcount[0]);
			retcode = SQLFetch(hstmt1);

		}
		else {
			throw "failure";
		}
		retcode = SQLCloseCursor(hstmt1);
		retcode = SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);
	}
	catch (exception exc) {
		exit(100);
	}

}

Status dbSqlSelect(char* sql, RETCODE retcode) {
	/*
		1.����һ�������(statement handle)
		2.����SQL���
		3.ִ�����
		4.�������
		*/
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);
	retcode = SQLExecDirect(hstmt1, (SQLCHAR*)sql, strlen(sql));
	Status stainfo;
	SQLLEN columnlen = 0;
	int i = 0;

	long int sid;
	float sspeed;
	long int spulse;
	char stime[256];
	float sspeed_in;
	long int spulse_in;

	SQLBindCol(hstmt1, 1, SQL_C_SLONG, &sid, 256, &columnlen);
	SQLBindCol(hstmt1, 2, SQL_C_FLOAT, &sspeed, 256, &columnlen);
	SQLBindCol(hstmt1, 3, SQL_C_FLOAT, &spulse, 256, &columnlen);
	SQLBindCol(hstmt1, 4, SQL_C_FLOAT, &sspeed_in, 256, &columnlen);
	SQLBindCol(hstmt1, 5, SQL_C_FLOAT, &spulse_in, 256, &columnlen);
	SQLBindCol(hstmt1, 6, SQL_CHAR, &stime, 256, &columnlen);

	if (retcode < 0)
	{
		cout << "û��ִ�����" << endl;
	}
	retcode = SQLFetch(hstmt1);

	while (retcode == SQL_ROW_SUCCESS || retcode == SQL_ROW_SUCCESS_WITH_INFO)
	{
		if (retcode == SQL_ROW_SUCCESS || retcode == SQL_ROW_SUCCESS_WITH_INFO)
		{
			stainfo.id[i] = sid;
			stainfo.speed[i] = sspeed;
			stainfo.pulse[i] = spulse;
			stainfo.speed_in[i] = sspeed_in;
			stainfo.pulse_in[i] = spulse_in;
			stainfo.time[i] = stime;
			i++;
			retcode = SQLFetch(hstmt1);
		}
	}
	//�ͷ������ 
	retcode = SQLCloseCursor(hstmt1);
	retcode = SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);
	return stainfo;
}



void endDbcon() {

	//3.�Ͽ����ݿ�����
		/*
		1. �Ͽ����ݿ�����
		2.�ͷ����Ӿ��.
		3.�ͷŻ������(���������Ҫ���������������������)
		*/
	SQLDisconnect(hdbc1);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc1);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
}

