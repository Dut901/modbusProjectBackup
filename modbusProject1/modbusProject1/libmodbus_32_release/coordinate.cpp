#include "include\coordinate.h"
//丝杠长度50cm,以两丝杠中心交点为原点，上方丝杠所在直线为x轴，下方丝杠所在直线为y轴，电机所在端为正方向.x∈(-50,50) y∈(-50,50) 单位刻度为0.5cm,对应电机为一圈，100000脉冲。

//根据坐标返回位置信息
void cooAnalysis(Coordinate des, uint32_t pulse[])
{
	pulse[0] = int(des.x * 100000);
	pulse[1] = int(des.y * 100000);	
}

