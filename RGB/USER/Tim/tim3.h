#ifndef __TIM3_H
#define __TIM3_H
#include "io_bit.h" 

void tim3_Init(int ARR,int PSC);
void LED3_PWM(int CRR);
void MOTOR_PWM(int CRR1);
void tim2_Init(int ARR,int PSC);
void tim12_Init(int ARR,int PSC);
void RGB_PWM(int CRR);
#endif
