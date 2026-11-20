#ifndef __SERVO_H
#define	__SERVO_H
#include "stm32f1xx_hal.h"
#include <stdint.h>

void servo_Init(void);
void servo_SetPulse(uint16_t us);
void servo_SetPulse2(uint16_t us);

#endif
