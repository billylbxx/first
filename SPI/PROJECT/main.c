#include "stm32f4xx.h"
#include "led.h"               
#include "Beep.h"
#include "key.h"
#include "uart.h"
#include "motor.h"
#include "exti.h"
#include "systick.h"
#include "tim.h"
#include "tim7.h"
#include "tim3.h"
#include "ADC.h"
#include "SPI.h"
#include <string.h>
//void delay(int m);
void fff(USART_TypeDef* USARTx, char*string);
void tt(char*sp);
void qaz(char*str);
int main()
{
	
//		u8 key1_flag=1;1
//	u8 key2_flag=1;
//	u8 m=0;
//	u8 Data;
//	char a[11]="wdvndj\r\n";
//char s[50];
//char s[50]={0x20,0x19,0,0,0x04,0x0d};
//int updown=0;
//int flag=0,i;
	
const unsigned char* tp[6]={gImage_IMG00000,gImage_IMG00001,gImage_IMG00006,gImage_IMG00012,gImage_IMG00013,gImage_IMG00019};
	
Led_Init();
Beep_Init();
key_Init();
Uart_Init();
Motor_Init();
Systick_Init(168);
//tim6_Init(50000,84);
//tim7_Init(50000,84);
//tim3_Init(500,840);
//tim2_Init(500,840);
//exti_Init();
ADC1_Init();
lcd_Init();
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//同一组优先级别中，不同的抢占级别之间，其中一抢占级别正在做事，另外抢占级别不能打断他
LCD_Fill(0, 0, 240, 240, LCD_RED);
//LCD_showstring(0,120,"xqsdffxjxxkgyvhhhuiij",LCD_GRED,LCD_RED,32);
//LCD_showdata(1,10,'C',LCD_GRED,LCD_RED,32);
 //LCD_CHINESE(0,0,"信",LCD_GRED,LCD_RED,32);
//LCD_CHINESES(88,38,"小新",LCD_GRED,LCD_RED,32);
//LCD_CHINESES(177,100,"信盈达小新hsfhf456",LCD_GRED,LCD_RED,32);
//show_pic(0,0,240,240,gImage_qqqq);
	while(1)
	{
 //     show_pic(0,0,180,100,gImage_IMG00000);
//		Delay_ms(100);
//    show_pic(0,0,180,100,gImage_IMG00001);
//		Delay_ms(100);
//		show_pic(0,0,180,100,gImage_IMG00006);
//		Delay_ms(100);
//		show_pic(0,0,180,100,gImage_IMG00012);
//		Delay_ms(100);
//		show_pic(0,0,180,100,gImage_IMG00013);
//		Delay_ms(100);
//		show_pic(0,0,180,100,gImage_IMG00019);
//		Delay_ms(100);
//		for(int i=0;i<6;i++)
//		{
//		show_pic(30,70,180,100,tp[i]);
//			Delay_ms(110);
//		}
	}
}


//void tt(char*sp)
//{
//	for(int i=0; ;i++)
//	{
//		while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE)==RESET);
//		sp[i]=(char)USART_ReceiveData(USART1);
//		if(sp[i]=='\r'||sp[i]=='\n')
//	{
//		sp[i]='\0';
//		break;
//	}
//	}
//	if(strcmp(sp,"LED1ON")==0)
//	{
//		LED1=0;
//	}
//	else if(strcmp(sp,"LED1OFF")==0)
//	{
//		LED1=1;
//	}
//}

//void qaz(char*str)
//{
//	for(int i=0; ;i++)
//	{
//		while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE)==RESET);
//		str[i]=(char)USART_ReceiveData(USART1);
//		if(str[i]=='\r'||str[i]=='\n')
//		{
//			str[i]='\0';
//			break;
//		}
//	}
//		if(strcmp(str,"开灯")==0)
//		{
//		  LED2=0;
//		}
//		else if(strcmp(str,"关灯")==0)
//		{
//			LED2=1;
//		}
//}

//void fff(USART_TypeDef* USARTx, char string[])
//{
//int i=0;
//	while(string[i]!='\0')
//	{
//		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
//		USART_SendData(USART1,string[i]);
//		i++;
//	}
//}


//void delay(int m)
//{
//	int i,j;
//	for(i=0;i<10000;i++)
//	{
//		for(j=0;j<m;j++)
//		{
//			;
//		}
//	}
//}

