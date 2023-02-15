#ifndef __USART2_H
#define __USART2_H

#include "stm32f4xx.h"
#include "stdio.h"

typedef struct
{
	unsigned char RxBuff[256];
	unsigned char TxBuff[256];
	unsigned short FlagLen;
	unsigned char RecFlag;
}USART2_InitStructure;

extern USART2_InitStructure Usart2;

void Usart2_Init(u32 bound);
void SendUsart2Data(u8 *data);
void SendUsart2Package(u8 *data,u16 len);

#endif
