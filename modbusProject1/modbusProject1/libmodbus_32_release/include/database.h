#include <string>
using namespace std;
struct Status
{
	long int id[256];
	float speed[256];
	long int pulse[256];
	string time[256];
};
void databaseCon();
void dbSqlSePr(char* sql);
void dbSqlEdit(char* sql);
Status dbSqlSelect(char* sql);
void endDbcon();

