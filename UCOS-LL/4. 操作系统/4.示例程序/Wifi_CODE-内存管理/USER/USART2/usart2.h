#ifndef __USART2_H
#define __USART2_H

#include "io_bit.h" 
#include <stdio.h>
#include <string.h>

typedef struct
{
	unsigned char RxBuff[256];
	unsigned char RecFlag;
	unsigned short RecLen;	
}USART2_TypeDef;

extern USART2_TypeDef Usart2;

void Usart2_Init(unsigned int bound);
void Usart2_SendString(unsigned char *str);
void Usart2_SendPackage(unsigned char *data, unsigned short len);

#endif
