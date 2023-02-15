#include "stm32f4xx.h"
#include "exti.h"
#include "key.h"
#include "led.h"
#include "systick.h"
#include "tim.h"
#include "uart.h"
static int key2_flag=0;
static int time;
void exti_Init(void)
{
	//时钟，哪组的哪个IO口（在rcc,syscfg里找）
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);//KEY2


	//配置IO口
	key_Init();
	
	//配置模块内部寄存器  exti.c
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line   =EXTI_Line13;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode   =EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);
		
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);//KEY1
	EXTI_InitStruct.EXTI_Line   =EXTI_Line0;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode   =EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);//KEY3
	EXTI_InitStruct.EXTI_Line   =EXTI_Line1;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode   =EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);
	
	//中断配置  misc
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel =EXTI15_10_IRQn;//在stm32f4xx.h文件里找
	NVIC_InitStruct.NVIC_IRQChannelCmd =ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority  =1;  //0~3
	NVIC_InitStruct.NVIC_IRQChannelSubPriority         =2;  //0~3
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel =EXTI0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd =ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority  =1;  //0~3
	NVIC_InitStruct.NVIC_IRQChannelSubPriority         =2;  //0~3
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel =EXTI1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd =ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority  =1;  //0~3
	NVIC_InitStruct.NVIC_IRQChannelSubPriority         =2;  //0~3
	NVIC_Init(&NVIC_InitStruct);
}



void EXTI15_10_IRQHandler(void)//KEY2
{
	if(EXTI_GetITStatus(EXTI_Line13)==1)
	{
		while(KEY2==0);
		Delay_ms(20);
			if(EXTI_GetITStatus( EXTI_Line13)==1)
			{
				key2_flag++;
				if(key2_flag>=5)
				{
				key2_flag=0;
				}
			}
	}
	EXTI_ClearITPendingBit(EXTI_Line13);
	switch(key2_flag)
	{
		case 1:TIM_ITConfig(TIM6,TIM_IT_Update,DISABLE);break;
		case 2:time=sec;printf("sec=%d\r\n",time);break;
		case 3:sec=time;time=min;printf("min=%d\r\n",time);break;
		case 4:min=time;time=hour;printf("hour=%d\r\n",time);break;
		case 0:	hour=time;TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);break;
	}
}

void EXTI0_IRQHandler(void)//KEY1
{
	if(EXTI_GetITStatus(EXTI_Line0)==1)
	{
		while(KEY1==1);
		Delay_ms(20);
		if(EXTI_GetITStatus(EXTI_Line0)==1)
		{
			EXTI_ClearITPendingBit(EXTI_Line0);
			switch(key2_flag)
			{
			 case 2:time++;if(time>60)time=1;printf("sec=%d\r\n",time);break;
			 case 3:time++;if(time>60)time=1;printf("min=%d\r\n",time);break;
			 case 4:time++;if(time>24)time=1;printf("hour=%d\r\n",time);break;
			}
		}
	}		
}

void EXTI1_IRQHandler(void) //KEY3
{
	if(EXTI_GetITStatus(EXTI_Line1)==1)
	{
		while(KEY3==0);
		Delay_ms(20);
		if(EXTI_GetITStatus(EXTI_Line1)==1)
		{
			EXTI_ClearITPendingBit(EXTI_Line1);
			switch(key2_flag)
			{
			 case 2:time--;if(time<0)time=59;printf("sec=%d\r\n",time);break;
			 case 3:time--;if(time<0)time=59;printf("min=%d\r\n",time);break;
			 case 4:time--;if(time<0)time=23;printf("hour=%d\r\n",time);break;
			}
		}	
	}
}





