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
#include <cassert>
#include <iomanip>
#include <sstream> 

using namespace std;

string getCurDate();
string DecIntToHexStr(long long num);
int transData(string str);

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



//�ر�modbus
void Tools::modbus_down(modbus_t* mb) {
	modbus_close(mb);  //�ر�modbus����
	modbus_free(mb);   //�ͷ�modbus��Դ��ʹ����libmodbus��Ҫ�ͷŵ�

}

//�ٶȿ��Ƴ�ʼ��
void Tools::servo_speed_init(modbus_t* mb) {
	//DI6 DI7 DI8 ����
	int regs1, regs2, regs3, regs4, regs5;
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

//��ӡ��ǰλ��
void Tools::show_position(modbus_t* mb)
{
	uint16_t tab_reg[2];
	int16_t tab_reg1[2];
	string trans;
	int decimal;
	int regs = modbus_read_registers(mb, 0x0014, 2, tab_reg);
	tab_reg1[1] = tab_reg[1];
	if (tab_reg1[1] < 0) {
		tab_reg1[1] = -tab_reg1[1];
		trans = DecIntToHexStr(tab_reg1[1]);
		decimal = -transData(trans);
	}
	else {
		trans = DecIntToHexStr(tab_reg1[1]);
		decimal = transData(trans);
	}
	//low = tab_reg[0] + 65536;
	int pos_now = decimal + tab_reg[0];
	cout << "��ǰλ��Ϊ" << pos_now << endl;
}

//��ȡ��ǰʱ��
string getCurDate()
{
	SYSTEMTIME systime;
	GetLocalTime(&systime);//����ʱ��
	string year, month, day, hour, min, sec, misec, time;
	char buffer[16];
	sprintf_s(buffer, "%d", systime.wYear);
	year = buffer;
	sprintf_s(buffer, "%d", systime.wMonth);
	month = buffer;
	sprintf_s(buffer, "%d", systime.wDay);
	day = buffer;
	sprintf_s(buffer,"%d", systime.wHour);
	hour = buffer;
	sprintf_s(buffer, "%d", systime.wMinute);
	min = buffer;
	sprintf_s(buffer, "%d", systime.wSecond);
	sec = buffer;
	sprintf_s(buffer, "%d", systime.wMilliseconds);
	misec = buffer;
	time = year + "-" + month + "-" + day + " " + hour + ":" + min + ":" + sec + "." + misec;
	return time;
}

//�����߳������ݿ�д
/*void* save_servo_status(void* mb) {
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	uint16_t tab_reg[2];
	uint16_t tab_reg2[2];
	string sql;
	databaseCon();
	modbus_t* modb = (modbus_t*)mb;
	int i = 0;
	while (1) {
		Sleep(2000);
		int regs = modbus_read_registers(modb, 0x0012, 2, tab_reg);		
		regs = modbus_read_registers(modb, 0x0014, 2, tab_reg2);
		sql = "insert into dbo.servo_status values(" + to_string(i) + "," + to_string(tab_reg[1]) + to_string(tab_reg[0]) + "," + to_string(tab_reg2[1]) + to_string(tab_reg2[0]) + "," + getCurDate() + ")";
		if (_kbhit()) {
			pthread_exit(NULL);
		}
	}
	return NULL;
}*/

//��ʾ��ؼĴ�������
void show_status(void* mb) {
	uint16_t tab_reg[2];
	int16_t tab_reg3[2];
	uint16_t tab_reg2[2];
	string trans;
	long long int decimal;
	string pulse, speed;
	modbus_t* modb = (modbus_t*)mb;
	databaseCon();
	char sql[200] = "select max(id) from dbo.servo_status";
	Status res = dbSqlSelect(sql);
	int maxid = res.id[0];
	while (1) {
		//Sleep(1000);
		maxid++;
		int regs = modbus_read_registers(modb, 0x0012, 2, tab_reg);
		tab_reg3[1] = tab_reg[1];
		if (tab_reg3[1] < 0) {
			speed = to_string(((float)(tab_reg[0] - 65536) / 10));
		}
		else {
			speed = to_string(((float)(tab_reg[0]) / 10));
		}		
		cout << "״̬��ؼĴ���1������Ϊ:" << speed << "clock" << getCurDate() << endl;
		regs = modbus_read_registers(modb, 0x0014, 2, tab_reg2);
		tab_reg3[1] = tab_reg2[1];
		if (tab_reg3[1] < 0) {
			tab_reg3[1] = -tab_reg3[1];
			trans = DecIntToHexStr(tab_reg3[1]);
			decimal = -transData(trans);
		}
		else {
			trans = DecIntToHexStr(tab_reg3[1]);
			decimal = transData(trans);
		}
		pulse = to_string(decimal + tab_reg2[0]);
		cout << "״̬��ؼĴ���2������Ϊ:" << pulse << "clock" << getCurDate() << endl;
		
		string sql1 = "insert into dbo.servo_status values(" + to_string(maxid) + "," + speed + "," + pulse + ",'" + getCurDate() + "')";
		strcpy_s(sql, sql1.c_str());
		dbSqlEdit(sql);

		if (_kbhit()) {
			break;
		}
	}
}

//32λ�Ĵ�����16Ϊ���ݱ任
int transData(string str)
{
	str = str + "0000";

	int res = strtol(str.c_str() , nullptr, 16);
	return res;
}

//sʮ����ת16����string
string DecIntToHexStr(long long num)
{
	string str;
	long long Temp = num / 16;
	int left = num % 16;
	if (Temp > 0)
		str += DecIntToHexStr(Temp);
	if (left < 10)
		str += (left + '0');
	else
		str += ('A' + left - 10);
	return str;
}

//P
int positionPID(int position, modbus_t *mb) {
	uint16_t tab_reg[2];
	string trans;
	int decimal;
	int pos_now = 0;
	int pos_err = 0;
	int kp = 1;
	int speed = 0;
	int regs = modbus_read_registers(mb, 0x0014, 2, tab_reg);
	trans = DecIntToHexStr(tab_reg[1]);
	decimal = transData(trans);
	pos_now = decimal + tab_reg[0];
	pos_err = position - pos_now;
	speed = pos_err/10000 * kp;
	if (speed > 600 ) {
		speed = 600;
	}
	else if (speed < -600) {
		speed = -600;
	}
	else if (speed >= 0 && speed <10) {
		speed = 1;
	
	}
	else if (speed > -10 && speed <= 0) {
		speed = -1;
	}
	return speed;
}

//����PID
int positionPIDC(int position, modbus_t* mb, PID_incremental pid1) {
	uint16_t tab_reg[2];
	string trans;
	int decimal;
	int pos_now = 0;
	int pos_err = 0;
	int kp = 1;
	float speed_incre = 0.0;
	int regs = modbus_read_registers(mb, 0x0014, 2, tab_reg);
	trans = DecIntToHexStr(tab_reg[1]);
	decimal = transData(trans);
	pos_now = decimal + tab_reg[0];
	speed_incre = pid1.pid_control(position, pos_now);
	return int(speed_incre);
}

//λ��PID
int positionPIDP(int position, modbus_t* mb, PID_position pid1) {
	uint16_t tab_reg[2];
	int16_t tab_reg1;
	string trans;
	int decimal;
	int pos_now = 0;
	int pos_err = 0;
	int kp = 1;
	float speed_incre = 0.0;
	int regs = modbus_read_registers(mb, 0x0014, 2, tab_reg);
	tab_reg1 = tab_reg[1];
	if (tab_reg1 < 0) {
		tab_reg1 = -tab_reg1;
		trans = DecIntToHexStr(tab_reg1);
		decimal = -transData(trans);
	}
	else {
		trans = DecIntToHexStr(tab_reg1);
		decimal = transData(trans);
	}
	pos_now = decimal + tab_reg[0];
	speed_incre = pid1.pid_control(position, pos_now);
	return int(speed_incre);
}

//�¼���������
int netControl(int *speed_now, int *speed_last) {
	double percent = 0.0;
	int err = 0;
	int flag;//�Ƿ����־λ
	if (*speed_last != 0) {
		/*if (*speed_last == *speed_now) {
			flag = 0;
		}
		else {
			flag = 1;
			*speed_last = *speed_now;
		}*/
		err = *speed_now - *speed_last;
		percent = float(err) / *speed_last;
		cout << percent << endl;
		if (-0.1 < percent && percent < 0.1) {
			flag = 0;
		}
		else {
			flag = 1;
			*speed_last = *speed_now;
		}
	}else if(*speed_now == 0){
		flag = 2;
		*speed_last = 0;
	}
	else{
		flag = 1;
		*speed_last = *speed_now;
	}
	return flag;	
}
