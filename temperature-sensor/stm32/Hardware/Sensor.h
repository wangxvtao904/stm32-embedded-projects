#ifndef __SENSOR_H
#define __SENSOR_H
void Sensor_Init(void);
uint16_t ADC_GetValue(void);
void ADC1_2_IRQHandler(void);
float Sensor_Temp(uint16_t ADC_Value);
extern volatile uint8_t flag;
#endif
