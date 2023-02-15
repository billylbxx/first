#include "stm32f4xx.h"
#include "exti.h"
#include "key.h"
#include "led.h"
#include "systick.h"
void exti_Init(void)
{
	
	//时钟RCC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	//哪个io口SYSCFG
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource13);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource1);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);
	key_Init();
	//配置模块内部寄存器EXTI
	EXTI_InitTypeDef EXTI_InitStruct={0};
	EXTI_InitStruct.EXTI_Line = EXTI_Line13;
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line1;
	EXTI_Init(&EXTI_InitStruct);
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line0;
	EXTI_Init(&EXTI_InitStruct);
	//中断配置MISC
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=EXTI15_10_IRQn;//stm32f4xx.h
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;//先占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;//从优先级
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI0_IRQn;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI1_IRQn;
	NVIC_Init(&NVIC_InitStruct);
}


//void delay111(unsigned int t)
//{
//	while(t--);
//}
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line13)==1)
	{
		Delay_ms(5);
		LED1=~LED1;
	}
	
	EXTI_ClearITPendingBit(EXTI_Line13);
}

 void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0)==1)
	{
		Delay_ms(5);
		LED1=~LED1;
	}
	
		EXTI_ClearITPendingBit(EXTI_Line0);
}

void EXTI1_IRQHandler(void) 
{
	if(EXTI_GetITStatus(EXTI_Line1)==1)
	{
		Delay_ms(5);
		LED1=~LED1;
	}
		EXTI_ClearITPendingBit(EXTI_Line1);
}
