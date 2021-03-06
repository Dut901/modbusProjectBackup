#include <iostream>
#include "include/PID_incremtental.h"
//????PID
PID_incremental::PID_incremental() :kp(0), ki(0), kd(0), e_pre_1(0), e_pre_2(0), target(0), actual(0)
{
   /* A = kp + ki + kd;
    B = -2 * kd - kp;
    C = kd;
    e = target - actual;*/
    A = kp + ki;
    B = -kp;
    C = 0;
}
PID_incremental::PID_incremental(float p, float i, float d) :kp(p), ki(i), kd(d), e_pre_1(0), e_pre_2(0), target(0), actual(0)
{
    A = kp + ki;
    B = -kp;
    C = 0;
    e = target - actual;
}
float PID_incremental::pid_control(float tar, float act)
{
    float u_increment;
    target = tar;
    actual = act;
    e = (target - actual)/10000;
    u_increment = A * e + B * e_pre_1;
 //   e_pre_2 = e_pre_1;
    e_pre_1 = e;
    return u_increment;
}

void PID_incremental::pid_show()
{
    using std::cout;
    using std::endl;
    cout << "The infomation of this incremental PID controller is as following:" << endl;
    cout << "     Kp=" << kp << endl;
    cout << "     Ki=" << ki << endl;
    cout << "     Kd=" << kd << endl;
    cout << " target=" << target << endl;
    cout << " actual=" << actual << endl;
    cout << "      e=" << e << endl;
    cout << "e_pre_1=" << e_pre_1 << endl;
    cout << "e_pre_2=" << e_pre_2 << endl;
}