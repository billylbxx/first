#include "stm32f4xx.h"
#include "led.h"               
#include "Beep.h"
#include "key.h"
#include "uart.h"
#include "motor.h"
#include "exti.h"
#include "systick.h"
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
	char str[50];
	//char s[50]={0x20,0x19,0,0,0x04,0x0d};

Led_Init();
Beep_Init();
key_Init();
Uart_Init();
Motor_Init();
Systick_Init(168);
//exti_Init();
//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//同一组优先级别中，不同的抢占级别之间，其中一抢占级别正在做事，另外抢占级别不能打断他
	while(1)
	{

		
//		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
//		{
//			Delay_ms(5);
//			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
//			{
//				GPIO_WriteBit(GPIOC,GPIO_Pin_8, (BitAction)(1-GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_8)));
//				GPIO_WriteBit(GPIOB,GPIO_Pin_0, (BitAction)(1-GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_0)));
//				while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0));
//			}	
//		}

Usart_Rec_data();
		Usart_Rec_string(str);
		if(strcmp(str,"LED1ON")==0)
	{
		LED1=0;
	}
	else if(strcmp(str,"LED1OFF")==0)
	{
		LED1=1;
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

