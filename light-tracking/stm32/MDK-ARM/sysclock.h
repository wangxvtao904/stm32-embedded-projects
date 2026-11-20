#ifndef __SYSCLOCK_H
#define __SYSCLOCK_H
#include "stm32f1xx_hal.h"

void GPIO_Init(void);

void Error_Handler(void);//出错的时候使用

void SystemClock_Config(void);

#endif
