#ifndef __UART_H
#define __UART_H
#include "io_bit.h" 

extern char buff[50];
extern int usart_flag;
void Uart_Init(void);
char Usart_Rec_data(void);
void Usart_Rec_string(char* sp);//使用这个函数需要把中断关闭
void Usart_Sec_date(char sp);
void Usart_Sec_string(u8* str);
#endif
