#include "stm32f1xx_hal.h"
#include "servo.h"

static TIM_HandleTypeDef htim2;
void servo_Init(void)
{
	GPIO_InitTypeDef gpio={0};
	TIM_OC_InitTypeDef oc={0};
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_TIM2_CLK_ENABLE();
	
	gpio.Pin=GPIO_PIN_0|GPIO_PIN_1;
	gpio.Mode=GPIO_MODE_AF_PP;
	gpio.Speed=GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA,&gpio);
	
	htim2.Instance=TIM2;
	htim2.Init.Prescaler=72-1;
	htim2.Init.CounterMode=TIM_COUNTERMODE_UP;
	htim2.Init.Period=20000-1;
	htim2.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	htim2.Init.RepetitionCounter=0;
	HAL_TIM_PWM_Init(&htim2);

	oc.OCMode=TIM_OCMODE_PWM1;//cnt>CCR 时 电平反转为低电平
	oc.Pulse=1500;
	oc.OCPolarity=TIM_OCPOLARITY_HIGH;
    oc.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim2, &oc, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);

	oc.Pulse=1500;
	HAL_TIM_PWM_ConfigChannel(&htim2, &oc, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
//| 参数           | 在哪      | 决定什么 |
//| ---           | ---       | ---                                |
//| `Prescaler`   | 定时器句柄 | 每个计数**多长**(1μs)               |
//| `Period`(ARR) | 定时器句柄 | **一圈数多少个数**(20000 个 = 20ms) |
//| `Pulse`(CCR)  | 通道结构体 | **高电平占多少个数**(1500 个 = 1.5ms) |

}
void servo_SetPulse(uint16_t us)
{
	if(us>2500)
	{
		us=2500;
	}
	if(us<500)
	{
		us=500;
	}
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, us);
}

void servo_SetPulse2(uint16_t us)
{
	if(us>2500)
	{
		us=2500;
	}
	if(us<500)
	{
		us=500;
	}
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, us);
}
