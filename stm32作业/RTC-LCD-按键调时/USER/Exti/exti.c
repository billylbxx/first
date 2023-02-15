#include "stm32f4xx.h"
#include "exti.h"
#include "key.h"
#include "led.h"
#include "systick.h"
#include "tim.h"
#include "uart.h"
#include "rtc.h"
#include "SPI.h"
#include "W25QXX.h"
static int key2_flag=0;
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
				if(key2_flag>=4)
				{
					key2_flag=1;
				}
			}
	}
	EXTI_ClearITPendingBit(EXTI_Line13);
	switch(key2_flag)
	{
		case 1:LCD_FLASH_SHOW_CHINESE_STR(0,200,"调秒",LCD_BLACK,LCD_WHITE,32);break;
		case 2:LCD_FLASH_SHOW_CHINESE_STR(0,200,"调分",LCD_BLACK,LCD_WHITE,32);break;
		case 3:LCD_FLASH_SHOW_CHINESE_STR(0,200,"调时",LCD_BLACK,LCD_WHITE,32);break;
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
			RTC_BKP++;
			RTC_TimeTypeDef RTC_TimeStruct;
			RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
			switch(key2_flag)
			{
			 case 1:RTC_TimeStruct.RTC_Seconds++;if(RTC_TimeStruct.RTC_Seconds>59)RTC_TimeStruct.RTC_Seconds=0;
				RTC_Set_Time(RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds,RTC_H12_AM);break;
			 case 2:RTC_TimeStruct.RTC_Minutes++;if(RTC_TimeStruct.RTC_Minutes>59)RTC_TimeStruct.RTC_Minutes=0;
				RTC_Set_Time(RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds,RTC_H12_AM);break;
			 case 3:RTC_TimeStruct.RTC_Hours++;if(RTC_TimeStruct.RTC_Hours>23)RTC_TimeStruct.RTC_Hours=0;
				RTC_Set_Time(RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds,RTC_H12_AM);break;
			}
			RTC_WriteBackupRegister(RTC_BKP_DR19, RTC_BKP);//判断是否要重置日期和时间	
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
			RTC_BKP--;
			RTC_TimeTypeDef RTC_TimeStruct;
			RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
			switch(key2_flag)
			{
			 case 1:if(RTC_TimeStruct.RTC_Seconds==0){RTC_TimeStruct.RTC_Seconds=60;}RTC_TimeStruct.RTC_Seconds--;
				RTC_Set_Time(RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds,RTC_H12_AM);break;
			 case 2:if(RTC_TimeStruct.RTC_Minutes==0){RTC_TimeStruct.RTC_Minutes=60;}RTC_TimeStruct.RTC_Minutes--;
				RTC_Set_Time(RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds,RTC_H12_AM);break;
			 case 3:if(RTC_TimeStruct.RTC_Hours==0){RTC_TimeStruct.RTC_Hours=24;}RTC_TimeStruct.RTC_Hours--;
				RTC_Set_Time(RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds,RTC_H12_AM);break;
			}
			RTC_WriteBackupRegister(RTC_BKP_DR19, RTC_BKP);//判断是否要重置日期和时间	
		}	
	}
}




