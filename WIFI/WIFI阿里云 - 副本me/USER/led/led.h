#ifndef _LED_H_
#define _LED_H_
#include "stm32f4xx.h"
#include "io_bit.h"

#define led1 PCout(4)
#define led2 PCout(5)
#define led3 PBout(0)

#define LED1 PCout(4)
#define LED2 PCout(5)
#define LED3 PBout(0)
void Led_Init(void);

#endif




