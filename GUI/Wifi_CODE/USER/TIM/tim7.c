#include "tim7.h"
#include "usart1.h"
#include "lvgl.h"
#include "stm32f4xx.h"
/*********************************************************************************************************
* �� �� �� : Timer6_Init
* ����˵�� : TIM6��ʼ��
* ��    �� : arr����װ�أ�psc��Ԥ��Ƶ
* �� �� ֵ : ��
* ��    ע : ���ʱ�� = (arr+1)*(psc+1) / clk
*********************************************************************************************************/ 
void Timer6_Init(unsigned short arr, unsigned short psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct 	= {0};
	NVIC_InitTypeDef 		NVIC_InitStruct			= {0};
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);		//ʹ��TIM6����ʱ��
	
	TIM_TimeBaseInitStruct.TIM_Period 		= arr - 1;
	TIM_TimeBaseInitStruct.TIM_Prescaler 	= psc -1 ;
	TIM_TimeBaseInitStruct.TIM_CounterMode 	= TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_ClockDivision 	 = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);
	
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel    = TIM6_DAC_IRQn;		//�ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//ʹ���ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 2;	//��Ӧ���ȼ�2
	NVIC_Init(&NVIC_InitStruct);	
	
	TIM_Cmd(TIM6, DISABLE);
}
/*********************************************************************************************************
* �� �� �� : Timer7_Init
* ����˵�� : TIM7��ʼ��
* ��    �� : arr����װ�أ�psc��Ԥ��Ƶ
* �� �� ֵ : ��
* ��    ע : ���ʱ�� = (arr+1)*(psc+1) / clk
*********************************************************************************************************/ 
void Timer7_Init(unsigned short arr, unsigned short psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct 	= {0};
	NVIC_InitTypeDef 		NVIC_InitStruct			= {0};
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);		//ʹ��TIM7����ʱ��
	
	TIM_TimeBaseInitStruct.TIM_Period 		= arr - 1;
	TIM_TimeBaseInitStruct.TIM_Prescaler 	= psc -1 ;
	TIM_TimeBaseInitStruct.TIM_CounterMode 	= TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_ClockDivision 	 = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseInitStruct);
	
	//TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel    = TIM7_IRQn;			//�ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//ʹ���ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 2;	//��Ӧ���ȼ�2
	NVIC_Init(&NVIC_InitStruct);	
	
	TIM_Cmd(TIM7, ENABLE);
}
/*********************************************************************************************************
* �� �� �� : TIM6_DAC_IRQHandler
* ����˵�� : ��ʱ��6�ж�
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void TIM6_DAC_IRQHandler(void)
{
	if(TIM6->SR & 0X01)
	{
		Usart1.RecFlag = 1;			//��ǽ������
		TIM_Cmd(TIM6, DISABLE);		//�رն�ʱ��
		
		TIM6->SR &= ~0X01;
	}
}
/*********************************************************************************************************
* �� �� �� : TIM7_IRQHandler
* ����˵�� : ��ʱ��7�ж�
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void TIM7_IRQHandler(void)
{
	if(TIM7->SR & 0X01)
	{
		lv_tick_inc(1);
		TIM7->SR &= ~0X01;
	}
}
