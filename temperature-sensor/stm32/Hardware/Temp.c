#include "stm32f10x.h" // Device header
#include <math.h>

#define R       10000.0f    // 分压电阻 10k
#define R0      10000.0f    // NTC 25℃阻值
#define B       3950.0f     // B值
#define V_REF   3.3f

float ADC_GetTemp(uint16_t ADC_Value)
{
	float V,Rt,Temp;
	V=(ADC_Value/4095.0f)*V_REF;
	Rt=R * V / (V_REF - V);
	Temp = 1.0f / (1.0f/298.15f + log(Rt/R0)/B);
	return Temp;//开尔文温度
}
