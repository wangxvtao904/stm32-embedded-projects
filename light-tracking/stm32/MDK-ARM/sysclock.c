#include "stm32f1xx_hal.h"
#include "sysclock.h"

void GPIO_Init(void)
{
	GPIO_InitTypeDef gpio={0};
	__HAL_RCC_GPIOC_CLK_ENABLE();
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);
	
	gpio.Mode=GPIO_MODE_OUTPUT_PP;
	gpio.Pin=GPIO_PIN_13;
	gpio.Pull=GPIO_NOPULL;
	gpio.Speed=GPIO_SPEED_FREQ_LOW;
	
	HAL_GPIO_Init(GPIOC,&gpio);
}
void Error_Handler(void)//出错的时候使用
{
	__disable_irq();//关闭一切中断
	while(1)//死循环
	{
	}
}
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};
    osc.OscillatorType = RCC_OSCILLATORTYPE_HSE;//选用外部晶振
    osc.HSEState       = RCC_HSE_ON;//HSE打开外部晶振
    osc.HSEPredivValue = RCC_HSE_PREDIV_DIV1;//晶振信号进PLL之前不分频
    osc.PLL.PLLState   = RCC_PLL_ON;//倍频器开
    osc.PLL.PLLSource  = RCC_PLLSOURCE_HSE;//pll倍频器的输入源是HSE
    osc.PLL.PLLMUL     = RCC_PLL_MUL9;//9倍 
	//HAL_RCC_OscConfig(&osc);
	//HAL_StatusTypeDef status=HAL_RCC_OscConfig&osc);
	//if(status!=HAL_OK)
	//{
		//Error_Handler();
	//}
	
    if (HAL_RCC_OscConfig(&osc) != HAL_OK)//先调用配置函数 然后判断返回值 等价于上面的
    {
        Error_Handler();
    }
	clk.ClockType =RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2; //开启AHB APB1 APB2 还有系统时钟
	clk.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
	clk.APB1CLKDivider=RCC_HCLK_DIV2;
	clk.APB2CLKDivider=RCC_HCLK_DIV1;
	clk.AHBCLKDivider=RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}
//| CPU 频率        | 需要的等待周期 |
//| ---             | ---           |
//| 0 ~ 24 MHz      | 0             |FLASH_LATENCY_0
//| 24 ~ 48 MHz     | 1             |FLASH_LATENCY_1
//| **48 ~ 72 MHz** | **2**         |FLASH_LATENCY_2← 我们在这一段
