#ifndef GPIO_H
#define GPIO_H

#include "main.h"

#define LED1_TOGGLE() GPIOC->ODR ^= (1<<10)
#define LED2_TOGGLE() GPIOC->ODR ^= (1<<12)
#define LED3_TOGGLE() GPIOD->ODR ^= (1<<2)

void Led_Init(void);
void Key_Init(void);
u8 Key_Scan(u8 mode);
#endif
