#include "stm32f1xx_hal.h"
#include "sysclock.h" 
#include "servo.h" 
#include "usart.h"

#define Kp 0.5f
int main(void)
{	
	HAL_Init();
	SystemClock_Config();
	GPIO_Init();
    servo_Init();
    Usart1_Init();
    uint16_t pulseX = 1500;   // 水平轴(左右),开机先回中位
    uint16_t pulseY = 1500;   // 垂直轴(上下)
    servo_SetPulse(pulseX);
    servo_SetPulse2(pulseY);
    while (1)
    {
        if(g_newData==1)
        {
            g_newData=0;
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);   // 调试:收到新数据就翻转LED
            if(g_haveTarget==1)
            {
                if(g_dx>15||g_dx<-15)//如果偏差大于15个像素才调整舵机
                {
                    int p = (int)pulseX+(int)(Kp*g_dx);//增量式:在当前位置上修正
                    if (p > 2000) p = 2000;
                    if (p < 1000) p = 1000;
                    pulseX = (uint16_t)p;
                }
                if(g_dy>15||g_dy<-15)
                {
                    int p = (int)pulseY-(int)(Kp*g_dy);//垂直轴安装方向相反,减号
                    if (p > 2000) p = 2000;
                    if (p < 1000) p = 1000;
                    pulseY = (uint16_t)p;
                }
            }
            servo_SetPulse(pulseX);
            servo_SetPulse2(pulseY);
        }
    }                  
}
