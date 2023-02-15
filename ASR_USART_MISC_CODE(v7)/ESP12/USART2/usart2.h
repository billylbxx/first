#ifndef __USART2_H
#define __USART2_H

#include "stm32f4xx.h"
#include "stdio.h"

typedef struct
{
	unsigned char RxBuff[256];
	unsigned char RecFlag;
	unsigned short RecLen;	
}USART2_InitStructure;

extern USART2_InitStructure Usart2;

void Usart2_Init(u32 bound);
void Usart2_SendString(u8 *data);
void Usart2_SendPackage(u8 *data,u16 len);

#endif
