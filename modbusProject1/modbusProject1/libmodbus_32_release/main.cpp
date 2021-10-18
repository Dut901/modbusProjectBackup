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





int _tmain(int argc, _TCHAR* argv[])	
{		
	Tools tool;
	modbus_t* mc = NULL;
	uint16_t tab_reg[8] = {0};
	mc = tool.modbus_con(3);
	//modbus_set_debug(mc, 1);
	tool.time_delay(mc);
	tool.servo_init(mc);
	Initialize();
	tool.servo_speed_init(mc);

	PID_position pid1(0.035,0, 0);
	int speedPID = 0;
	int speedLast = 0;
	int flag = 0;
	int* speed_P = &speedPID;
	int* speed_L = &speedLast;
	int i = 0;
	while (1) {
		char const* kb_char;
		if (_kbhit()) {
			cin.getline(kb,_MAX_PATH) ;
			switch (s_mapStringValues[kb]) {
			case evStringValue1:
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
				show_status(mc);
				break;
			case pos:
				tool.show_position(mc);
				break;
			case pid:
				tool.set_speed(mc, 0);				
				cout << "请输入位置值（脉冲0-10000000）" << endl;
				int position;
				
				int speed_incre;
				cin >> position;
				tool.servo_start(mc);
				while (1) {
					Sleep(20);

					//P
					/*speedPID = positionPID(position, mc);
					tool.set_speed(mc, speedPID);*/

					//PI 增量型
					speedPID = positionPIDP(position, mc, pid1);
					//speedPID += speed_incre;
					if (speedPID > 600) {
						speedPID = 600;
					}
					else if (speedPID < -600) {
						speedPID = -600;
					}
					flag = netControl(speed_P, speed_L);
					if (flag == 1) {
						tool.set_speed(mc, speedPID);
						i++;
					}
					else if (flag == 2) {
						tool.servo_stop(mc);
						tool.show_position(mc);
						cout << i << endl;
						i = 0;
						break;				
					}
					//tool.set_speed(mc, speedPID);
					if (_kbhit()) {
						break;
					}
				}
				break;
			case evEnd:
				goto endCon;
			}
		}
	
	
	
	
	}
	endCon:
	tool.modbus_down(mc);			//释放modbus连接
	endDbcon();
 	system("pause");
	return 0;
}


