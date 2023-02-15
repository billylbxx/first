#ifndef __LED_H
#define __LED_H

#include "io_bit.h" 

#define LED1 PCout(4)
#define LED2 PBout(0)
#define LED3 PCout(5)

void Led_Init(void);

#endif

