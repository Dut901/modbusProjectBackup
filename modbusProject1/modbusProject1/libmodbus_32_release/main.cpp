#include "include/modbus-rtu.h"
#include <iostream>
#include "include/modbus.h"
#include "stdio.h"
#include "tchar.h"
#include "modbus-private.h"
#include "string.h"
#include "conio.h"
#include <map>
#include "database.h"
#include "include/database.h"
#include "include/tools.h"
using namespace std;
#pragma comment(lib,"modbus.lib")


typedef std::uint64_t hash_t;
constexpr hash_t prime = 0x100000001B3ull;
constexpr hash_t basis = 0xCBF29CE484222325ull;


static enum StringValue {
	evNotDefined,
	evStringValue1,
	evStringValue2,
	evStringValue3,
	status,
	evEnd
};

// Map to associate the strings with the enum values
static std::map<std::string, StringValue> s_mapStringValues;

// User input
static char kb[_MAX_PATH];

// Intialization
static void Initialize();
//switch变量初始化
void Initialize()
{
	s_mapStringValues["speed"] = evStringValue1;
	s_mapStringValues["start"] = evStringValue2;
	s_mapStringValues["stop"] = evStringValue3;
	s_mapStringValues["status"] = status;
	s_mapStringValues["end"] = evEnd;

	cout << "s_mapStringValues contains "
		<< s_mapStringValues.size()
		<< " entries." << endl;
}


int _tmain(int argc, _TCHAR* argv[])	
{


	//Database db;
	//db.dataCon();
	Tools tool;
	modbus_t* mc = NULL;
	uint16_t tab_reg[8] = {0};
	mc = tool.modbus_con(3);
	//modbus_set_debug(mc, 1);
	tool.time_delay(mc);
	tool.servo_init(mc);

	Initialize();

	while (1) {
		char const* kb_char;
		if (_kbhit()) {
			cin.getline(kb,_MAX_PATH) ;
			switch (s_mapStringValues[kb]) {
			case evStringValue1:
				tool.servo_speed_init(mc);
				cout << "请输入速度值（r/min）" << endl;
				int speed;
				cin >> speed;
				tool.set_speed(mc, speed);
				break;
			case evStringValue2:
				tool.servo_start(mc);
				break;
			case evStringValue3:
				tool.servo_stop(mc);
				break;
			case status:
				tool.show_status(mc);
				break;
			case evEnd:
				goto endCon;
			}
		}
	
	
	
	
	}
	endCon:
	tool.modbus_down(mc);			//释放modbus连接

 	system("pause");
	return 0;
}


