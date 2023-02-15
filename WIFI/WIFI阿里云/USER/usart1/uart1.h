#ifndef _USART1_H_
#define _USART1_H_

#include "stm32f4xx.h"
#include "stdio.h"
#include "string.h"


extern u8 uart1_flag,uart_buf[128];

void uart1_init(u32 Baud);
void USART1_SendByte(u8 data);
void USART1_SendStr(u8 *str);
u8 USART1_RecByte(void);
void USART1_RecStr(u8 *buf);
void SendUsart1Package(u8 *data,u16 len);
#endif
