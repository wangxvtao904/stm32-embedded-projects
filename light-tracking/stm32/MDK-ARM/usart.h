#ifndef __USART_H
#define	__USART_H
#include "stm32f1xx_hal.h"

void Usart1_Init(void);
extern UART_HandleTypeDef huart1;
extern volatile int g_dx;
extern volatile int g_dy;
extern volatile uint8_t g_haveTarget;
extern volatile uint8_t g_newData;


#endif

