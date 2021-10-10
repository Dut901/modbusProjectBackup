#include "modbus.h"
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
};
void show_status(void* mb);