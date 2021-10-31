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
#include <vector>
#include "include/coordinate.h"

using namespace std;
#pragma comment(lib,"modbus.lib")





int _tmain(int argc, _TCHAR* argv[])	
{	
	Tools tool;
	modbus_t* mc = NULL;
	modbus_t* mcy = NULL;
	uint16_t tab_reg[8] = {0};
	mc = tool.modbus_con(3);
	mcy = tool.modbus_con_y(4);
	//modbus_set_debug(mc, 1);
	tool.time_delay(mc);
	tool.servo_init(mc);
	tool.time_delay(mcy);
	tool.servo_init(mcy);
	Initialize();
	tool.servo_speed_init(mc);
	tool.servo_speed_init(mcy);

	string cooString;
	while (1) {
		char const* kb_char;
		if (_kbhit()) {
			cin.getline(kb,_MAX_PATH) ;
			switch (s_mapStringValues[kb]) {
			case evStringValue1:
				cout << "�������ٶ�ֵ��r/min��" << endl;
				int speed;
				cin >> speed;
				tool.set_speed(mcy, speed);
				tool.set_speed(mc, speed);
				break;
			case evStringValue2:
				tool.servo_start(mcy);
				tool.servo_start(mc);
				break;
			case evStringValue3:
				tool.servo_stop(mcy);
				tool.servo_stop(mc);
				break;
			case status:
				show_status(mcy);
				show_status(mc);
				break;
			case pos:
				tool.show_position(mc);
				tool.show_position(mcy);
				break;
			case pid:
				tool.set_speed(mcy, 0);				
				cout << "������λ��ֵ������-5000000-5000000��" << endl;
				int position;				
				int speed_incre;
				cin >> position;
				set_position(mcy, position);
				break;
			case coo:
				cout << "���������꣨x,y��" << endl;				
				cin >> cooString;
				xyCoo(mc, mcy, cooString);
				/*tool.show_position(mc);
				tool.show_position(mcy);*/		
				break;
			case evEnd:
				goto endCon;
			}
		}
	
	
	
	
	}
	endCon:
	tool.modbus_down(mc);			//�ͷ�modbus����
	endDbcon();
 	system("pause");
	return 0;
}


