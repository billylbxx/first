#include "tim.h"
#include "stm32f4xx.h"
#include "led.h"
/*
F=84000000
f=F/84=1000000//F/PSC
T=1/f=0.000001s=0.001ms=1us
t=ARR*T
50000us*20=1000000us=1s//ARR*i
*/

void tim6_Init(int ARR,int PSC)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	
	/*���ö�ʱ��6ģ���ڲ��Ĵ���*/
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_Period = ARR-1;//�Զ����ص����������Զ���װ�ؼĴ������ڵ�ֵ
  TIM_TimeBaseInitStruct.TIM_Prescaler = PSC-1;  //Ԥ��Ƶ��
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//ʱ�ӷָ�
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;//���������ڼ�����ֵ0������
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStruct);
	
	//ʹ��Ӱ�ӼĴ��������Բ�д
	TIM_ARRPreloadConfig(TIM6,ENABLE);
	TIM_PrescalerConfig(TIM6,PSC-1, ENABLE);
	
//UGλ�����������
	TIM_UpdateDisableConfig(TIM6, DISABLE);	
	
//UGλ��1������һ��������£�Ŀ��ARRֵװ��Ӱ��
	TIM_GenerateEvent(TIM6,TIM_EventSource_Update);
	
	//ʹ�ܶ�ʱ�ж�
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
	//����NVIC
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=TIM6_DAC_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_InitStruct);
	
	//����ʱ��
	TIM_Cmd(TIM6,ENABLE);
}
//�жϷ�����
int sec=0,min=0,hour=0;
void TIM6_DAC_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)==1)
	{
		static int i;
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);//���жϱ�־λ
		i++;
		if(i>=20)//��ΪARR���ܳ���65536����������д
		{
			i=0;
			sec++;
			printf("%d:%d:%d\r\n",hour,min,sec);
			if(sec>=60)
			{
				sec=0;
				min++;
				if(min>=60)
				{
					min=0;
					hour++;
					if(hour>=24)
					{
						hour=0;
					}
				}
			}
//			if(sec==2&&min==1&&hour==0)
//			{
//				LED1=0;
//			}
		}
	}
}
	













