// Modbus-RTU-Test.cpp : �������̨Ӧ�ó������ڵ㡣
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
//   ʹ��libmodbus�����Ӧ�ó���master(����)����������˳�����£�
//1. ��ʼ�� modbus ָ��
//2. ���ô�վID
//3. ��������
//4. ��ȡ���ּĴ��� / ����Ĵ��� / ��ɢ���� / ��Ȧ����
//5. д�����Ĵ��� / ����Ĵ��� / ��λ����
//6. �ر�����

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
				cout << "�������ٶ�ֵ��r/min��" << endl;
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
	modbus_down(mc);			//�ͷ�modbus����

 	system("pause");
	return 0;
}

//������Ӧ��com��slave
modbus_t *modbus_con(int slave_id)
{
	modbus_t* mc = modbus_new_rtu("COM6", 9600, 'N', 8, 2);   //��ͬ�Ķ˿�ֻ��ͬʱ��һ�� 
	if (mc == NULL)
	{
		fprintf(stderr, "Unable to allocate libmodbus contex\n");
		return NULL;
	}
	
	//modbus_set_debug(mb, 1);						//����1�ɿ���������Ϣ
	modbus_set_slave(mc, slave_id);						//����modbus�ӻ���ַ 
	if (modbus_connect(mc) == -1)					//��modbus����
	{
		fprintf(stderr, "Connection failed:%s\n", modbus_strerror(errno));
		return NULL;
	}
	return mc;
}

//�ŷ���������ʼ��
void servo_init(modbus_t *mb) {
	//p2-30д5
	int reg = modbus_write_register(mb, 0x023C, 5);
	//p0-17д7��P0-9����ٶ�ֵ
	reg = modbus_write_register(mb, 0x0022, 07);
	//P0-18д00 P0-10��Ż���������
	reg = modbus_write_register(mb, 0x0024, 00);

}

//��������ʱ��
void time_delay(modbus_t *mb)
{
	struct timeval t;
	t.tv_sec = 0;
	t.tv_usec = 1000000;	
	modbus_set_response_timeout(mb, (int)&t.tv_sec, (int)&t.tv_usec);
	modbus_set_response_timeout(mb, (int)&t.tv_sec, (int)&t.tv_usec);
}

//��ӡREG����
void show_data(uint16_t *reg, int num) {
	for (int i = 0; i < num; i++) {
		cout << "��" << i+1 << "���Ĵ���������Ϊ" << reg[i] << endl;
	}
}

//��ʾ��ؼĴ�������
void show_status(modbus_t* mb) {
	uint16_t tab_reg[2];
	while (1) {
		int regs = modbus_read_registers(mb, 0x0012, 2, tab_reg);

		cout << "״̬��ؼĴ���1������Ϊ:" << tab_reg[1] << tab_reg[0] << endl;

		regs = modbus_read_registers(mb, 0x0014, 2, tab_reg);

		cout << "״̬��ؼĴ���2������Ϊ:" << tab_reg[1] << tab_reg[0] << endl;
		if (_kbhit()) {
			break;
		}
	}
}

//�ر�modbus
void modbus_down(modbus_t *mb) {
	modbus_close(mb);  //�ر�modbus����
	modbus_free(mb);   //�ͷ�modbus��Դ��ʹ����libmodbus��Ҫ�ͷŵ�

}

//�ٶȿ��Ƴ�ʼ��
void servo_speed_init(modbus_t *mb) {
	//DI6 DI7 DI8 ����
	int regs1, regs2, regs3, regs4, regs5, regs6;
	regs1 = modbus_write_register(mb, 0x021E, 0);
	regs2 = modbus_write_register(mb, 0x0220, 0);
	regs3 = modbus_write_register(mb, 0x0222, 0);
	if (regs1 == -1 || regs2 == -1 || regs3 == -1) {
		cout << "��ʼ��ʧ�ܣ�DI6\7\8����ʧ�ܣ�" << endl;
		return;
	}
	//DI���Ź滮  DI1����ΪSON  DI3����ΪSPD0  DI4�Ĺ���ΪSPD1
	regs4 = modbus_write_register(mb, 0x0214, 0x0101);
	if (regs4 == -1) {
		cout << "��ʼ��ʧ�ܣ�DI1���ܶ���ʧ�ܣ�" << endl;
		return;
	}
	regs4 = modbus_write_register(mb, 0x0218, 0x0114);
	if (regs4 == -1) {
		cout << "��ʼ��ʧ�ܣ�DI3���ܶ���ʧ�ܣ�" << endl;
		return;
	}
	regs4 = modbus_write_register(mb, 0x021A, 0x0115);
	if (regs4 == -1) {
		cout << "��ʼ��ʧ�ܣ�DI4���ܶ���ʧ�ܣ�" << endl;
		return;
	}
	//����DI������P4-07����
	regs5 = modbus_write_register(mb, 0x030C, 0x01FF);
	if (regs5 == -1) {
		cout << "��ʼ��ʧ�ܣ�DI������Դ����ʧ�ܣ�" << endl;
		return;
	}

	//ѡ���ٶȼĴ���ΪSP3(P1-11 0116H.SPD0=1 SPD1=1,��DI3=1,DI4=1)
	modify_DIn(mb, 3, 1);
	modify_DIn(mb, 4, 1);


}

//������Ӧ��DIn��ֵ
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
		cout << "��DI" << reg_num << "Ϊ" << bit << "�ɹ�" << endl;
	}

}

//�����ٶȿ���ģʽ�ٶȼĴ���SP3(0116H)
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
		cout << "�����ٶȳ�����Χ" << endl;
	}
	
	int regs = modbus_write_registers(mb, 0x0116, 2, tab_write);
	if (regs == 2) {
		cout << "��ǰ�����ٶ�" << speed << "r/min" << endl;
	}
	else {
		return;
	}
}

//�ŷ����� DI1��1
void servo_start(modbus_t *mb)
{
	modify_DIn(mb, 1, 1);
}

//�ŷ��ر� DI1��0
void servo_stop(modbus_t* mb)
{
	modify_DIn(mb, 1, 0);
}

