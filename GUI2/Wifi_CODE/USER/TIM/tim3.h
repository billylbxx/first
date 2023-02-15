#ifndef _TIM3_H
#define _TIM3_H

#include "stm32f4xx.h"
#include "stdio.h"





void tim3_Init(int ARR,int PSC);
void LED3_PWM(int CCR);
void FAN_PWM(int CCR);


#endif
