#ifndef __UART4_H
#define __UART4_H

#include "io_bit.h" 
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct
{
	unsigned char RxBuff[256];
	unsigned char RecFlag;
	unsigned short RecLen;	
}UART4_TypeDef;

extern UART4_TypeDef Uart4;

void Uart4_Init(unsigned int bound);
void Uart4_SendString(char *str);
void Uart4_SendPackage(unsigned char *str, unsigned short len);

#endif
