#include "tim7.h"
#include "usart1.h"
#include "lvgl.h"
#include "stm32f4xx.h"
/*********************************************************************************************************
* 函 数 名 : Timer6_Init
* 功能说明 : TIM6初始化
* 形    参 : arr：重装载，psc：预分频
* 返 回 值 : 无
* 备    注 : 溢出时间 = (arr+1)*(psc+1) / clk
*********************************************************************************************************/ 
void Timer6_Init(unsigned short arr, unsigned short psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct 	= {0};
	NVIC_InitTypeDef 		NVIC_InitStruct			= {0};
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);		//使能TIM6外设时钟
	
	TIM_TimeBaseInitStruct.TIM_Period 		= arr - 1;
	TIM_TimeBaseInitStruct.TIM_Prescaler 	= psc -1 ;
	TIM_TimeBaseInitStruct.TIM_CounterMode 	= TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_ClockDivision 	 = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);
	
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel    = TIM6_DAC_IRQn;		//中断源
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//使能中断源
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 2;	//响应优先级2
	NVIC_Init(&NVIC_InitStruct);	
	
	TIM_Cmd(TIM6, DISABLE);
}
/*********************************************************************************************************
* 函 数 名 : Timer7_Init
* 功能说明 : TIM7初始化
* 形    参 : arr：重装载，psc：预分频
* 返 回 值 : 无
* 备    注 : 溢出时间 = (arr+1)*(psc+1) / clk
*********************************************************************************************************/ 
void Timer7_Init(unsigned short arr, unsigned short psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct 	= {0};
	NVIC_InitTypeDef 		NVIC_InitStruct			= {0};
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);		//使能TIM7外设时钟
	
	TIM_TimeBaseInitStruct.TIM_Period 		= arr - 1;
	TIM_TimeBaseInitStruct.TIM_Prescaler 	= psc -1 ;
	TIM_TimeBaseInitStruct.TIM_CounterMode 	= TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_ClockDivision 	 = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseInitStruct);
	
	//TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel    = TIM7_IRQn;			//中断源
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//使能中断源
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 2;	//响应优先级2
	NVIC_Init(&NVIC_InitStruct);	
	
	TIM_Cmd(TIM7, ENABLE);
}
/*********************************************************************************************************
* 函 数 名 : TIM6_DAC_IRQHandler
* 功能说明 : 定时器6中断
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void TIM6_DAC_IRQHandler(void)
{
	if(TIM6->SR & 0X01)
	{
		Usart1.RecFlag = 1;			//标记接收完成
		TIM_Cmd(TIM6, DISABLE);		//关闭定时器
		
		TIM6->SR &= ~0X01;
	}
}
/*********************************************************************************************************
* 函 数 名 : TIM7_IRQHandler
* 功能说明 : 定时器7中断
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void TIM7_IRQHandler(void)
{
	if(TIM7->SR & 0X01)
	{
		lv_tick_inc(1);
		TIM7->SR &= ~0X01;
	}
}
