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
#include "usart2.h"
#include "RGB.h"
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
	
//const unsigned char* tp[6]={gImage_IMG00000,gImage_IMG00001,gImage_IMG00006,gImage_IMG00012,gImage_IMG00013,gImage_IMG00019};
	
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
//lcd_Init();
usart2_Init();
RGB_Init();	
VIP:
		printf("AT\r\n");//usart2打印数据，发送指令到WIFI
		for(int i=0;i<999999;i++);
		if(usart2_flag ==1)//usart2发送指令到WIFI完成（WIFI设置好后返回数据OK到usart2,CH="OK"）
		{
			usart2_flag=0;
			USART1_SendStr(CH);//usart1发送CH到CH340（回显OK到串口）
			if(strstr(CH,"OK")==0)goto VIP;
		}
		printf("AT+CWMODE=3\r\n");
		for(int i=0;i<999999;i++);
		if(usart2_flag ==1)
		{
			usart2_flag=0;
			USART1_SendStr(CH);
			if(strstr(CH,"OK")==0)goto VIP;
		}
		printf("AT+CWSAP=\"espZXT\",\"0123456789\",11,3\r\n");
		for(int i=0;i<999999;i++);
		if(usart2_flag ==1)
		{
			usart2_flag=0;
			USART1_SendStr(CH);
			if(strstr(CH,"OK")==0)goto VIP;
		}
		printf("AT+RST\r\n");
		for(int i=0;i<999999;i++);
		if(usart2_flag ==1)
		{
			usart2_flag=0;
			USART1_SendStr(CH);
			if(strstr(CH,"OK")==0)goto VIP;
		}
		for(int i=0;i<9999999;i++);
	 printf("AT+CIPMUX=1\r\n");
	 for(int i=0;i<999999;i++);
	 if( usart2_flag == 1 )
	 {
			usart2_flag =0;
			USART1_SendStr(CH);
			if(strstr(CH,"OK") == 0) goto VIP;
	 }
		printf("AT+CIPSERVER=1,8080\r\n");
		for(int i=0;i<999999;i++);
		if(usart2_flag ==1)
		{
			usart2_flag=0;
			USART1_SendStr(CH);
			if(strstr(CH,"OK")==0)goto VIP;
		}
	while(1)
	{
		if(usart2_flag==1)
		{
			usart2_flag=0;
			USART1_SendStr(CH);//显示手机发的内容
			if(strstr(CH,"开灯")!=0)
			{
				RGB_TO_GRB(0,RGB_RED);
				RGB_ENABLE();
				LED1=0;
			}
			else if(strstr(CH,"关灯")!=0)
			{
				RGB_TO_GRB(0,RGB_black);
				RGB_ENABLE();
				LED1=1;
			}
		}
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

