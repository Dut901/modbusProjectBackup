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
#include <thread>
#include "include/coordinate.h"

using namespace std;


string getCurDate();
string DecIntToHexStr(long long num);
int transData(string str);


//更改相应的DIn的值
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
		cout << "置DI" << reg_num << "为" << bit << "成功" << endl;
	}

}

Tools::Tools()
{
}

//连接相应的com和slave
modbus_t* Tools::modbus_con(int slave_id)
{
	modbus_t* mc = modbus_new_rtu("COM6", 115200, 'N', 8, 2);   //相同的端口只能同时打开一个 
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

modbus_t* Tools::modbus_con_y(int slave_id)
{
	modbus_t* mc = modbus_new_rtu("COM7", 115200, 'N', 8, 2);   //相同的端口只能同时打开一个 
	if (mc == NULL)
	{
		fprintf(stderr, "Unable to allocate libmodbus contex\n");
		return NULL;
	}

	//modbus_set_debug(mc, 1);						//设置1可看到调试信息
	modbus_set_slave(mc, slave_id);						//设置modbus从机地址 
	if (modbus_connect(mc) == -1)					//打开modbus连接
	{
		fprintf(stderr, "Connection failed:%s\n", modbus_strerror(errno));
		return NULL;
	}
	return mc;
}

//伺服驱动器初始化
void Tools::servo_init(modbus_t* mb) {
	//p2-30写5
	int reg = modbus_write_register(mb, 0x023C, 5);
	//p0-17写7，P0-9存放速度值
	reg = modbus_write_register(mb, 0x0022, 07);
	//P0-18写00 P0-10存放回授脉冲数
	reg = modbus_write_register(mb, 0x0024, 00);

}

//设置连接时限
void Tools::time_delay(modbus_t* mb)
{
	struct timeval t;
	t.tv_sec = 0;
	t.tv_usec = 1000000;
	modbus_set_response_timeout(mb, (int)&t.tv_sec, (int)&t.tv_usec);
	modbus_set_response_timeout(mb, (int)&t.tv_sec, (int)&t.tv_usec);
}

//打印REG数据
void Tools::show_data(uint16_t* reg, int num) {
	for (int i = 0; i < num; i++) {
		cout << "第" << i + 1 << "个寄存器的数据为" << reg[i] << endl;
	}
}



//关闭modbus
void Tools::modbus_down(modbus_t* mb) {
	modbus_close(mb);  //关闭modbus连接
	modbus_free(mb);   //释放modbus资源，使用完libmodbus需要释放掉

}

//速度控制初始化
void Tools::servo_speed_init(modbus_t* mb) {
	//DI6 DI7 DI8 置零
	int regs1, regs2, regs3, regs4, regs5;
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



//设置速度控制模式速度寄存器SP3(0116H)
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
void Tools::servo_start(modbus_t* mb)
{
	modify_DIn(mb, 1, 1);
}

//伺服关闭 DI1置0
void Tools::servo_stop(modbus_t* mb)
{

	modify_DIn(mb, 1, 0);
}

//打印当前位置
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
	cout << mb->slave <<"当前位置为" << pos_now << endl;
}

//获取当前时间
string getCurDate()
{
	SYSTEMTIME systime;
	GetLocalTime(&systime);//本地时间
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

//并发线程向数据库写
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

//显示监控寄存器数据
void show_status(modbus_t* mb) {
	uint16_t tab_reg[2];
	int16_t tab_reg3[2];
	uint16_t tab_reg2[2];
	string trans;
	string table_name = "";
	long long int decimal;
	string pulse, speed;
	char sql[200] = "";
	RETCODE ret = databaseCon();
	if (mb->slave == 3) {
		table_name = "dbo.servo_status_x";
	}
	else if (mb->slave == 4) {
		table_name = "dbo.servo_status_y";
	}
	string sqlStr = "select max(id) from " + table_name;
	strcpy_s(sql, sqlStr.c_str());
	Status res = dbSqlSelect(sql, ret);
	int maxid = res.id[0];
	while (1) {
		//Sleep(1000);
		maxid++;
		int regs = modbus_read_registers(mb, 0x0012, 2, tab_reg);
		tab_reg3[1] = tab_reg[1];
		if (tab_reg3[1] < 0) {
			speed = to_string(((float)(tab_reg[0] - 65536) / 10));
		}
		else {
			speed = to_string(((float)(tab_reg[0]) / 10));
		}		
		//cout << "状态监控寄存器1的数据为:" << speed << "clock" << getCurDate() << endl;
		regs = modbus_read_registers(mb, 0x0014, 2, tab_reg2);
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
		//cout << "状态监控寄存器2的数据为:" << pulse << "clock" << getCurDate() << endl;
		
		string sql1 = "insert into " + table_name + " values(" + to_string(maxid) + "," + speed + "," + pulse + "," + "0,0,'" + getCurDate() + "')";
		strcpy_s(sql, sql1.c_str());
		dbSqlEdit(sql, ret);

		if (_kbhit()) {
			break;
		}
	}
}

//32位寄存器高16为数据变换
int transData(string str)
{
	str = str + "0000";

	int res = strtol(str.c_str() , nullptr, 16);
	return res;
}



//s十进制转16进制string
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

//增量PID
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

//位置PID
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

//事件驱动策略
int netControl(int *speed_now, int *speed_last) {
	double percent = 0.0;
	int err = 0;
	int flag;//是否传输标志位
	if (*speed_last != 0) {
		if (*speed_last == *speed_now) {
			flag = 0;
		}
		else {
			flag = 1;
			*speed_last = *speed_now;
		}
		/*err = *speed_now - *speed_last;
		percent = float(err) / *speed_last;
		if (-0.1 < percent && percent < 0.1) {
			flag = 0;
		}
		else {
			flag = 1;
			*speed_last = *speed_now;
		}*/
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

//位置给定
void set_position(modbus_t* mb, int pulse)
{
	Tools tool;
	tool.servo_start(mb);
	int i = 0;
	int speedPID;
	int speedLast = 0;
	int flag = 0;
	int* speed_P = &speedPID;
	int* speed_L = &speedLast;
	PID_position pid1(0.01, 0, 0);

	
	
	while (1) {
		Sleep(20);

		


		//PI 增量型
		speedPID = positionPIDP(pulse, mb, pid1);

		

		//speedPID += speed_incre;
		if (speedPID > 600) {
			speedPID = 600;
		}
		else if (speedPID < -600) {
			speedPID = -600;
		}
		
		flag = netControl(speed_P, speed_L);
		if (flag == 1) {
			//saveDataToDatabase(mb, pulse, speedPID);
			tool.set_speed(mb, speedPID);
			i++;
		}
		else if (flag == 2) {
			//saveDataToDatabase(mb, pulse, 0);
			tool.servo_stop(mb);
			tool.show_position(mb);
			i = 0;
			break;
		}
		else if (flag == 0) {
			//saveDataToDatabase(mb, pulse, speedLast);
		}
		if (_kbhit()) {
			break;
		}
	}
}

//去除str中的ch
string stringTrans(string str, char ch) {
	while (str.find(ch) != -1) {
		str.erase(str.find(ch), 1);
	}
	return str;
}
//按分隔符分割
void SplitString(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

//将运行数据保存至数据库 pulse_in 给定脉冲  speed_in 速度信号
void saveDataToDatabase(modbus_t* mb, int pulse_in, float speed_in) {
	uint16_t tab_reg[2];
	int16_t tab_reg3[2];
	uint16_t tab_reg2[2];
	string trans;
	string table_name;
	long long int decimal;
	string pulse, speed;
	char sql[500] = "";
	RETCODE ret = databaseCon();
	modbus_t* modb = (modbus_t*)mb;
	if (mb->slave == 3) {
		table_name = "dbo.servo_status_x";
	}
	else if (mb->slave == 4) {
		table_name = "dbo.servo_status_y";
	}
	string sqlStr = "select max(id) from " + table_name;
	strcpy_s(sql, sqlStr.c_str());
	Status res = dbSqlSelect(sql, ret);
	int maxid = res.id[0];
	maxid++;
	int regs = modbus_read_registers(modb, 0x0012, 2, tab_reg);
	tab_reg3[1] = tab_reg[1];
	if (tab_reg3[1] < 0) {
		speed = to_string(((float)(tab_reg[0] - 65536) / 10));
	}
	else {
		speed = to_string(((float)(tab_reg[0]) / 10));
	}
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
	string sql1 = "insert into " + table_name +" values(" + to_string(maxid) + "," + speed + "," + pulse + "," + to_string(speed_in) + "," + to_string(pulse_in) + ",'" + getCurDate() + "')";
	strcpy_s(sql, sql1.c_str());
	dbSqlEdit(sql, ret);
}

//创建线程控制xy电机到达位置
void xyPosition(modbus_t* mbx, modbus_t* mby, uint32_t pulse_pos[2]) {
	thread x(show_status,mbx);
	x.detach();
	/*thread y(show_status, mby);
	y.detach();*/
	thread t(set_position, mbx, pulse_pos[0]);
	t.detach();
	thread n(set_position, mby, pulse_pos[1]);
	n.detach();

	//GetExitCodeThread(t,);

}

//根据坐标控制位置
void xyCoo(modbus_t* mbx, modbus_t* mby, string cooString) {
	uint32_t position_pulse[2];
	vector<string> v;
	cooString = stringTrans(cooString, '(');
	cooString = stringTrans(cooString, ')');
	SplitString(cooString, v, ","); //可按多个字符来分隔;
	Coordinate cooStruct;
	cooStruct.x = atof(v[0].c_str());
	cooStruct.y = atof(v[1].c_str());
	cooAnalysis(cooStruct, position_pulse);
	xyPosition(mbx, mby, position_pulse);
	v.clear();
}

