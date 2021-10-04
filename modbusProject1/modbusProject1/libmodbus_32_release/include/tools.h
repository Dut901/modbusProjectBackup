#include "modbus.h"
class Tools {
public :
	Tools();
	modbus_t* modbus_con(int slave_id);
	void time_delay(modbus_t* mb);
	void show_data(uint16_t* reg, int num);
	void modbus_down(modbus_t* mb);
	void servo_init(modbus_t* mb);
	void servo_speed_init(modbus_t* mb);
	void servo_start(modbus_t* mb);
	void set_speed(modbus_t* mb, int speed_num);
	void servo_stop(modbus_t* mb);
	void show_status(modbus_t* mb);
};
