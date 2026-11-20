#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "OLED.h"
#include "Sensor.h"
#include "Temp.h"
#include "Key.h"
float i;

int main(void)
{
	OLED_Init();
	Sensor_Init();
	Key_Init();
	
	OLED_ShowString(2,1,"ADC:");
    OLED_ShowString(3,1,"Temp:     K");
	OLED_ShowString(4,1,"Temp:     C");
	 while(1)
    {
		if(Key_GetNum()==1)
		{
			OLED_Clear();
			__wfi();
			OLED_Init();
			OLED_ShowString(2,1,"ADC:");
			OLED_ShowString(3,1,"Temp:     K");
			OLED_ShowString(4,1,"Temp:     C");
		}
		i=ADC_GetTemp(ADC_GetValue())-273.15f;
		if(flag==1)
		{
		GPIO_ResetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_10);
		OLED_ShowString(1,4,"WARNING!");
		flag=0;
		}
			
		if(ADC_GetValue()>1860)
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_10);
			OLED_ShowString(1,4,"        ");
		}
		OLED_ShowNum(2,7,ADC_GetValue(),4);
		OLED_ShowNum(3,7,ADC_GetTemp(ADC_GetValue()),3);
		OLED_ShowNum(4,7,i,2);
		
	}
}
