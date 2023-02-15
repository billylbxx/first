#ifndef _TIM_H
#define _TIM_H

#include "stm32f4xx.h"
#include "stdio.h"


extern unsigned int i,j,k;	

void tim_Init(int ARR,int PSC);
void delay_MS(int m);



#endif

