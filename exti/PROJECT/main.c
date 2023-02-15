#include "stm32f4xx.h"
#include "led.h"               
#include "Beep.h"
#include "key.h"
#include "uart.h"
#include "motor.h"
#include "exti.h"
#include <string.h>
void delay(int m);
int main()
{
	
//		u8 key1_flag=1;1
//	u8 key2_flag=1;
//	u8 m=0;
//	u8 Data;
//	char a[11]="wdvndj\r\n";
char fwfw[50]={0};
	char s[50]={0x20,0x19,0,0,0x04,0x0d};
Led_Init();
Beep_Init();
key_Init();
Uart_Init();
	Motor_Init();
exti_Init();
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//同一组优先级别中，不同的抢占级别之间，其中一抢占级别正在做事，另外抢占级别不能打断他
	while(1)
	{
		
		
//		Usart_Rec_string(fwfw);
//		if((strcmp(fwfw,"LED1ON"))==0)
//		{
//			LED1=0;
//		}
//		else if((strcmp(fwfw,"LED1OFF"))==0)
//		{
//			LED1=1;
//		}
		
		
//		if(usart_flag==1)
//		{
//			usart_flag=0;	
//		if(strcmp(buff,"LED1ON")==0)
//		{
//			LED1=0;
//		}
//		else if(strcmp(buff,"LED1OFF")==0)
//		{
//			LED1=1;
//		}
//		}
		
//		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
//		{
//			delay(100);
//			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
//			{
//				GPIO_WriteBit(GPIOC,GPIO_Pin_8, (BitAction)(1-GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_8)));
//				GPIO_WriteBit(GPIOB,GPIO_Pin_0, (BitAction)(1-GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_0)));
//				while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0));
//			}	
//		}

	}
}


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

