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
#include "include\tools.h"
using namespace std;

void getCurDate();

//������Ӧ��DIn��ֵ
void modify_DIn(modbus_t* mb, int reg_num, int bit) {
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

Tools::Tools()
{
}

//������Ӧ��com��slave
modbus_t* Tools::modbus_con(int slave_id)
{
	modbus_t* mc = modbus_new_rtu("COM6", 115200, 'N', 8, 2);   //��ͬ�Ķ˿�ֻ��ͬʱ��һ�� 
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
void Tools::servo_init(modbus_t* mb) {
	//p2-30д5
	int reg = modbus_write_register(mb, 0x023C, 5);
	//p0-17д7��P0-9����ٶ�ֵ
	reg = modbus_write_register(mb, 0x0022, 07);
	//P0-18д00 P0-10��Ż���������
	reg = modbus_write_register(mb, 0x0024, 00);

}

//��������ʱ��
void Tools::time_delay(modbus_t* mb)
{
	struct timeval t;
	t.tv_sec = 0;
	t.tv_usec = 1000000;
	modbus_set_response_timeout(mb, (int)&t.tv_sec, (int)&t.tv_usec);
	modbus_set_response_timeout(mb, (int)&t.tv_sec, (int)&t.tv_usec);
}

//��ӡREG����
void Tools::show_data(uint16_t* reg, int num) {
	for (int i = 0; i < num; i++) {
		cout << "��" << i + 1 << "���Ĵ���������Ϊ" << reg[i] << endl;
	}
}

//��ʾ��ؼĴ�������
void Tools::show_status(modbus_t* mb) {
	uint16_t tab_reg[2];
	while (1) {
		int regs = modbus_read_registers(mb, 0x0012, 2, tab_reg);

		cout << "״̬��ؼĴ���1������Ϊ:" << tab_reg[1] << tab_reg[0] << "clock";
		getCurDate();

		regs = modbus_read_registers(mb, 0x0014, 2, tab_reg);

		cout << "״̬��ؼĴ���2������Ϊ:" << tab_reg[1] << tab_reg[0] << "clock" ;
		getCurDate();
		if (_kbhit()) {
			break;
		}
	}
}

//�ر�modbus
void Tools::modbus_down(modbus_t* mb) {
	modbus_close(mb);  //�ر�modbus����
	modbus_free(mb);   //�ͷ�modbus��Դ��ʹ����libmodbus��Ҫ�ͷŵ�

}

//�ٶȿ��Ƴ�ʼ��
void Tools::servo_speed_init(modbus_t* mb) {
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



//�����ٶȿ���ģʽ�ٶȼĴ���SP3(0116H)
void Tools::set_speed(modbus_t* mb, int speed) {
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
	}
	else {
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
void Tools::servo_start(modbus_t* mb)
{
	modify_DIn(mb, 1, 1);
}

//�ŷ��ر� DI1��0
void Tools::servo_stop(modbus_t* mb)
{

	modify_DIn(mb, 1, 0);
}

void getCurDate()
{
	SYSTEMTIME systime;
	GetLocalTime(&systime);//����ʱ��
	cout << systime.wHour << ":" << systime.wMinute << ":" << systime.wSecond << ":" << systime.wMilliseconds << endl;
}
