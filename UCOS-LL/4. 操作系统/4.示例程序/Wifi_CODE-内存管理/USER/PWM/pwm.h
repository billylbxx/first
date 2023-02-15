#ifndef __PWM_H
#define __PWM_H

#include "io_bit.h" 

void Led2Pwm_Init(unsigned short arr,unsigned short psc);
void HollowPwm_Init(unsigned short arr,unsigned short psc);
void SteeringPwm_Init(unsigned short arr,unsigned short psc);

void SetLedBrightness(char value);
void SetHollowSpeed(char value);
void SetServoAngle(unsigned char angle);

#endif
