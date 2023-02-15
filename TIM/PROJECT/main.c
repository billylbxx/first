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
	char s[50];
	//char s[50]={0x20,0x19,0,0,0x04,0x0d};

Led_Init();
Beep_Init();
key_Init();
Uart_Init();
Motor_Init();
Systick_Init(168);
tim6_Init(50000,84);
//tim7_Init(50000,84);
Exti_Init();
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//同一组优先级别中，不同的抢占级别之间，其中一抢占级别正在做事，另外抢占级别不能打断他
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

		if(usart_flag==1)
		{
			usart_flag=0;
			sec=buff[2];
			min=buff[1];
			hour=buff[0];
			if(buff[0]==0x19&&buff[1]==0x20&&buff[5]==0x0d&&buff[4]==0x04)
			{
				if(buff[3]==0x01)//开
				{
					switch(buff[2])
					{
						case 0x01:LED1=0;s[2]=buff[2];s[3]=buff[3];break;
						case 0x02:MOTOR=1;s[2]=buff[2];s[3]=buff[3];break;
						case 0x30:break;
						default:printf("无此功能\r\n");break;
					}
					sprintf(s,"%#x %#x %#x %#x %#x %#x",buff[0],buff[1],buff[2],buff[3],buff[4],buff[5]);
					printf("%s",s);
				}
				else if(buff[3]==0x00)
				{
					switch(buff[2])
					{
						case 0x01:LED1=1;break;
						case 0x02:MOTOR=0;break;
						case 0x30:break;
						default:printf("无此功能\r\n");break;
					}
				}
				else printf("命令错误\r\n");
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

