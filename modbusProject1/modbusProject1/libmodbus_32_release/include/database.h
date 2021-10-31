#include <string>
using namespace std;
struct Status
{
	long int id[256];
	float speed[256];
	long int pulse[256];
	float speed_in[256];
	long int pulse_in[256];
	string time[256];
};
typedef signed short RETCODE;
RETCODE databaseCon();
void dbSqlSePr(char* sql, RETCODE retcode);
void dbSqlEdit(char* sql, RETCODE retcode);
Status dbSqlSelect(char* sql, RETCODE retcode);
void endDbcon();

