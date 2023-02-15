#ifndef __USART2_H
#define __USART2_H

#include "stm32f4xx.h"
#include "stdio.h"

void usart2_Init(void);
extern int usart2_flag;
extern char CH[300];

#endif
