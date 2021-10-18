#include "include/PID_position.h"
#include <iostream>
//Œª÷√PID
PID_position::PID_position() :kp(0), ki(0), kd(0), target(0), actual(0), integral(0)
{
    e = target - actual;
    e_pre = e;
}
PID_position::PID_position(float p, float i, float d) : kp(p), ki(i), kd(d), target(0), actual(0), integral(0)
{
    e = target - actual;
    e_pre = e;
}
float PID_position::pid_control(float tar, float act)
{
    float u;
    target = tar;
    actual = act;
    e = (target - actual);
    integral += e;
    u = kp * e + ki * integral;
    e_pre = e;
    return u;
}
void PID_position::pid_show()
{
    using std::cout;
    using std::endl;
    cout << "The infomation of this position PID controller is as following:" << endl;
    cout << "       Kp=" << kp << endl;
    cout << "       Ki=" << ki << endl;
    cout << "       Kd=" << kd << endl;
    cout << " integral=" << integral << endl;
    cout << "   target=" << target << endl;
    cout << "   actual=" << actual << endl;
    cout << "        e=" << e << endl;
    cout << "    e_pre=" << e_pre << endl;
}