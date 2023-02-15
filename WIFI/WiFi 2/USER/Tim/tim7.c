#include "tim7.h"
#include "stm32f4xx.h"
#include "led.h"
void tim7_Init(int ARR,int PSC)
{
//ʹ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
//ARR��PSC,���ϼ�������Ƶ�����ڼӱ����߼���ʱ����
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_Period = ARR-1;//�Զ����ص����������Զ���װ�ؼĴ������ڵ�ֵ
  TIM_TimeBaseInitStruct.TIM_Prescaler = PSC-1;  //Ԥ��Ƶ��
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//ʱ�ӷָ�
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;//���������ڼ�����ֵ0������
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStruct);
	
//ʹ��Ӱ�ӼĴ��������Բ�д
	TIM_ARRPreloadConfig(TIM7,ENABLE);
	TIM_PrescalerConfig(TIM7,PSC-1, ENABLE);
	
//UGλ�����������
	TIM_UpdateDisableConfig(TIM7, DISABLE);	
	
//UGλ��1������һ��������£�Ŀ��ARRֵװ��Ӱ��
	TIM_GenerateEvent(TIM7,TIM_EventSource_Update);
	
//ѭ������
	TIM_UpdateRequestConfig(TIM7,TIM_UpdateSource_Global);


//ʹ�ܶ����ж�
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
	
	//����NVIC
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=TIM7_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_InitStruct);
	
	//����ʱ��
	TIM_Cmd(TIM7,ENABLE);
}
//�жϷ�����

void TIM7_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)==1)
	{
		static int i;
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);//���жϱ�־λ
		i++;
		if(i>=20)//��ΪARR���ܳ���65536����������д
		{
			i=0;
		}
	}
}
	
