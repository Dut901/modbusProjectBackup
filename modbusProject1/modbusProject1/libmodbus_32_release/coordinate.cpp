#include "include\coordinate.h"
//˿�ܳ���50cm,����˿�����Ľ���Ϊԭ�㣬�Ϸ�˿������ֱ��Ϊx�ᣬ�·�˿������ֱ��Ϊy�ᣬ������ڶ�Ϊ������.x��(-50,50) y��(-50,50) ��λ�̶�Ϊ0.5cm,��Ӧ���ΪһȦ��100000���塣

//�������귵��λ����Ϣ
void cooAnalysis(Coordinate des, uint32_t pulse[])
{
	pulse[0] = int(des.x * 100000);
	pulse[1] = int(des.y * 100000);	
}

