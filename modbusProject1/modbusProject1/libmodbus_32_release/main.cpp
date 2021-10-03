// Modbus-RTU-Test.cpp : 定义控制台应用程序的入口点。
//LuckyRen 2020.10.16 

#include "include/modbus-rtu.h"
#include <iostream>
#include "include/modbus.h"
#include "stdio.h"
#include "tchar.h"
#include "modbus-private.h"
#include "string.h"
#include "conio.h"
#include <map>
using namespace std;
#pragma comment(lib,"modbus.lib")
//   使用libmodbus库进行应用程序master(主机)开发，基本顺序如下：
//1. 初始化 modbus 指针
//2. 设置从站ID
//3. 建立连接
//4. 读取保持寄存器 / 输入寄存器 / 离散输入 / 线圈输入
//5. 写单个寄存器 / 多个寄存器 / 多位数据
//6. 关闭连接

#define ADD_SERVO1 2;
#define ADD_SERVO2 3;
#define ADD_SERVO3 4;
#define ADD_SERVO4 5;
#define ADD_SERVO5 6;

typedef std::uint64_t hash_t;
constexpr hash_t prime = 0x100000001B3ull;
constexpr hash_t basis = 0xCBF29CE484222325ull;

modbus_t *modbus_con(int slave_id);
void time_delay(modbus_t *mb);
void show_data(uint16_t *reg, int num);
void modbus_down(modbus_t *mb);
void servo_init(modbus_t *mb);
void modify_DIn(modbus_t *mb, int reg_num, int bit);
void servo_speed_init(modbus_t *mb);
void servo_start(modbus_t *mb);
void set_speed(modbus_t *mb, int speed_num);
void servo_stop(modbus_t* mb);
void show_status(modbus_t* mb);



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
	modbus_t* mc = NULL;							
	uint16_t tab_reg[8] = {0};
	mc = modbus_con(3);
	//modbus_set_debug(mc, 1);
	time_delay(mc);
	servo_init(mc);

	Initialize();

	while (1) {
		char const* kb_char;
		if (_kbhit()) {
			cin.getline(kb,_MAX_PATH) ;
			switch (s_mapStringValues[kb]) {
			case evStringValue1:
				servo_speed_init(mc);
				cout << "请输入速度值（r/min）" << endl;
				int speed;
				cin >> speed;
				set_speed(mc, speed);
				break;
			case evStringValue2:
				servo_start(mc);
				break;
			case evStringValue3:
				servo_stop(mc);
				break;
			case status:
				show_status(mc);
				break;
			case evEnd:
				goto endCon;
			}
		}
	
	
	
	
	}
	endCon:
	modbus_down(mc);			//释放modbus连接

 	system("pause");
	return 0;
}

//连接相应的com和slave
modbus_t *modbus_con(int slave_id)
{
	modbus_t* mc = modbus_new_rtu("COM6", 9600, 'N', 8, 2);   //相同的端口只能同时打开一个 
	if (mc == NULL)
	{
		fprintf(stderr, "Unable to allocate libmodbus contex\n");
		return NULL;
	}
	
	//modbus_set_debug(mb, 1);						//设置1可看到调试信息
	modbus_set_slave(mc, slave_id);						//设置modbus从机地址 
	if (modbus_connect(mc) == -1)					//打开modbus连接
	{
		fprintf(stderr, "Connection failed:%s\n", modbus_strerror(errno));
		return NULL;
	}
	return mc;
}

//伺服驱动器初始化
void servo_init(modbus_t *mb) {
	//p2-30写5
	int reg = modbus_write_register(mb, 0x023C, 5);
	//p0-17写7，P0-9存放速度值
	reg = modbus_write_register(mb, 0x0022, 07);
	//P0-18写00 P0-10存放回授脉冲数
	reg = modbus_write_register(mb, 0x0024, 00);

}

//设置连接时限
void time_delay(modbus_t *mb)
{
	struct timeval t;
	t.tv_sec = 0;
	t.tv_usec = 1000000;	
	modbus_set_response_timeout(mb, (int)&t.tv_sec, (int)&t.tv_usec);
	modbus_set_response_timeout(mb, (int)&t.tv_sec, (int)&t.tv_usec);
}

//打印REG数据
void show_data(uint16_t *reg, int num) {
	for (int i = 0; i < num; i++) {
		cout << "第" << i+1 << "个寄存器的数据为" << reg[i] << endl;
	}
}

//显示监控寄存器数据
void show_status(modbus_t* mb) {
	uint16_t tab_reg[2];
	while (1) {
		int regs = modbus_read_registers(mb, 0x0012, 2, tab_reg);

		cout << "状态监控寄存器1的数据为:" << tab_reg[1] << tab_reg[0] << endl;

		regs = modbus_read_registers(mb, 0x0014, 2, tab_reg);

		cout << "状态监控寄存器2的数据为:" << tab_reg[1] << tab_reg[0] << endl;
		if (_kbhit()) {
			break;
		}
	}
}

//关闭modbus
void modbus_down(modbus_t *mb) {
	modbus_close(mb);  //关闭modbus连接
	modbus_free(mb);   //释放modbus资源，使用完libmodbus需要释放掉

}

//速度控制初始化
void servo_speed_init(modbus_t *mb) {
	//DI6 DI7 DI8 置零
	int regs1, regs2, regs3, regs4, regs5, regs6;
	regs1 = modbus_write_register(mb, 0x021E, 0);
	regs2 = modbus_write_register(mb, 0x0220, 0);
	regs3 = modbus_write_register(mb, 0x0222, 0);
	if (regs1 == -1 || regs2 == -1 || regs3 == -1) {
		cout << "初始化失败（DI6\7\8置零失败）" << endl;
		return;
	}
	//DI引脚规划  DI1功能为SON  DI3功能为SPD0  DI4的功能为SPD1
	regs4 = modbus_write_register(mb, 0x0214, 0x0101);
	if (regs4 == -1) {
		cout << "初始化失败（DI1功能定义失败）" << endl;
		return;
	}
	regs4 = modbus_write_register(mb, 0x0218, 0x0114);
	if (regs4 == -1) {
		cout << "初始化失败（DI3功能定义失败）" << endl;
		return;
	}
	regs4 = modbus_write_register(mb, 0x021A, 0x0115);
	if (regs4 == -1) {
		cout << "初始化失败（DI4功能定义失败）" << endl;
		return;
	}
	//所有DI引脚由P4-07控制
	regs5 = modbus_write_register(mb, 0x030C, 0x01FF);
	if (regs5 == -1) {
		cout << "初始化失败（DI数据来源定义失败）" << endl;
		return;
	}

	//选择速度寄存器为SP3(P1-11 0116H.SPD0=1 SPD1=1,即DI3=1,DI4=1)
	modify_DIn(mb, 3, 1);
	modify_DIn(mb, 4, 1);


}

//更改相应的DIn的值
void modify_DIn(modbus_t *mb, int reg_num, int bit) {
	uint16_t tab_regs[8];
	int regs = modbus_read_registers(mb, 0x040E, 1, tab_regs);
	switch (reg_num) {
		case 1:
			tab_regs[0] &= ~(0x0001 << 0);
			tab_regs[0] |= bit << 0;
			break;
		case 2:
			tab_regs[0] &= ~(0x0001 << 1);
			tab_regs[0] |= bit << 1;
			break;
		case 3:
			tab_regs[0] &= ~(0x0001 << 2);
			tab_regs[0] |= bit << 2;
			break;
		case 4:
			tab_regs[0] &= ~(0x0001 << 3);
			tab_regs[0] |= bit << 3;
			break;
		case 5:
			tab_regs[0] &= ~(0x0001 << 4);
			tab_regs[0] |= bit << 4;
			break;
		case 6:
			tab_regs[0] &= ~(0x0001 << 5);
			tab_regs[0] |= bit << 5;
			break;
		case 7:
			tab_regs[0] &= ~(0x0001 << 6);
			tab_regs[0] |= bit << 6;
			break;
		case 8:
			tab_regs[0] &= ~(0x0001 << 7);
			tab_regs[0] |= bit << 7;
			break;
	}
	regs = modbus_write_register(mb, 0x040E, tab_regs[0]);
	if (regs == 1) {
		cout << "置DI" << reg_num << "为" << bit << "成功" << endl;
	}

}

//设置速度控制模式速度寄存器SP3(0116H)
void set_speed(modbus_t *mb, int speed) {
	uint16_t tab_write[2];
	int speed_num = speed * 10;
	if (-50000 <= speed_num <= 50000) {
		tab_write[0] = speed_num;
		if (speed_num < 0) {
			tab_write[1] = -1;
		}
		else {
			tab_write[1] = 0;
		}	
	}else {
		cout << "输入速度超出范围" << endl;
	}
	
	int regs = modbus_write_registers(mb, 0x0116, 2, tab_write);
	if (regs == 2) {
		cout << "当前设置速度" << speed << "r/min" << endl;
	}
	else {
		return;
	}
}

//伺服启动 DI1置1
void servo_start(modbus_t *mb)
{
	modify_DIn(mb, 1, 1);
}

//伺服关闭 DI1置0
void servo_stop(modbus_t* mb)
{
	modify_DIn(mb, 1, 0);
}

