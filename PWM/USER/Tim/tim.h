#ifndef __TIM_H
#define __TIM_H
#include "io_bit.h" 

extern int sec,min,hour;
void tim6_Init(int ARR,int PSC);
#endif
