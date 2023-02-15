#ifndef _TIM2_H
#define _TIM2_H

#include "stm32f4xx.h"
#include "stdio.h"





void tim2_Init(int ARR,int PSC);


void Servo_SetAngle(unsigned char angle);

#endif
