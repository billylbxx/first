#ifndef __USART1_H
#define __USART1_H

#include "io_bit.h" 
#include <stdio.h>
#include <string.h>

typedef struct
{
	unsigned char RxBuff[1024];
	unsigned char RecFlag;
	unsigned short RecLen;	
}USART1_TypeDef;

extern USART1_TypeDef Usart1;

void Usart1_Init(unsigned int bound);
void Usart1_SendString(unsigned char *str);
void Usart1_SendPackage(unsigned char *data, unsigned short len);

#endif
