#pragma once
class PID_position
{
private:
    float kp;//����ϵ��
    float ki;//����ϵ��
    float kd;//΢��ϵ��
    float target;//Ŀ��ֵ
    float actual;//ʵ��ֵ
    float e;//���
    float e_pre;//��һ�����
    float integral;//������
public:
    PID_position();
    ~PID_position() {};
    PID_position(float p, float i, float d);
    float pid_control(float tar, float act);//ִ��PID����
    void pid_show();//��ʾPID���������ڲ�����
};
