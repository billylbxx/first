#ifndef _LED_H
#define _LED_H

#include "stm32f4xx.h"
#include "io_bit.h"

#define LED1 PCout(4)
#define LED2 PCout(5)
#define LED3 PBout(0)

void Len_Init(void);

#endif
