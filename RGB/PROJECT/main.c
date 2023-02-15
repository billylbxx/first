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
#include "ADC.h"
#include "SPI.h"
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
RGB_Init();


//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//同一组优先级别中，不同的抢占级别之间，其中一抢占级别正在做事，另外抢占级别不能打断他

	while(1)
	{
//	  if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
//		{
//			Delay_ms(100);
//			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
//			{
//				RGB_TO_GRB(0,0XABCD);
//				RGB_ENABLE();
//				Delay_ms(500);
//				RGB_TO_GRB(1,0XABCD);
//				RGB_ENABLE();
//				Delay_ms(500);
//				RGB_TO_GRB(2,0XABCD);
//				RGB_ENABLE();
//				Delay_ms(500);
//				RGB_TO_GRB(3,0XABCD);
//				RGB_ENABLE();
//				Delay_ms(500);
//				for(int i=0;i<4;i++)
//				{
//					RGB_TO_GRB(i,RGB_black);
//					RGB_ENABLE();
//					Delay_ms(500);
//				}
//				GPIO_WriteBit(GPIOB,GPIO_Pin_15, (BitAction)(1-GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_15)));
//				while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0));
//			}     
//		}
	}
}



