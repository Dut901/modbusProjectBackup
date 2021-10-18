#pragma once
class PID_position
{
private:
    float kp;//比例系数
    float ki;//积分系数
    float kd;//微分系数
    float target;//目标值
    float actual;//实际值
    float e;//误差
    float e_pre;//上一次误差
    float integral;//积分项
public:
    PID_position();
    ~PID_position() {};
    PID_position(float p, float i, float d);
    float pid_control(float tar, float act);//执行PID控制
    void pid_show();//显示PID控制器的内部参数
};
