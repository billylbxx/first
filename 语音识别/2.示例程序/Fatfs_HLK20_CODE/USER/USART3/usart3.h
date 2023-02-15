#ifndef __USART3_H
#define __USART3_H

#include "io_bit.h" 
#include <stdio.h>
#include <string.h>

typedef struct
{
	unsigned char RxBuff[256];
	unsigned char RecFlag;
	unsigned short RecLen;	
}USART3_TypeDef;

extern USART3_TypeDef Usart3;

void Usart3_Init(unsigned int bound);
void Usart3_SendString(unsigned char *str);
void Usart3_SendPackage(unsigned char *data, unsigned short len);

#endif
