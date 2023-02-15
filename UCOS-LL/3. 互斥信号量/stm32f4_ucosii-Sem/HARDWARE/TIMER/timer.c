#include "timer.h"
#include "led.h"
#include "GUI.h"

extern __IO int32_t OS_TimeMS;

//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3ʱ��ʹ��    
 	TIM3->ARR=arr;  	//�趨�������Զ���װֵ 
	TIM3->PSC=psc;  	//Ԥ��Ƶ��	  
	TIM3->DIER|=1<<0;   //��������ж�	  
	TIM3->CR1|=0x01;    //ʹ�ܶ�ʱ��3
  MY_NVIC_Init(1,3,TIM3_IRQn,2);	//��ռ1�������ȼ�3����2		
	
}

//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
	if(TIM3->SR&0X0001)//����ж�  
	{
		OS_TimeMS++;
	}
	TIM3->SR&=~(1<<0);//����жϱ�־λ 	 
}

//ͨ�ö�ʱ��4�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM4_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<2;	//TIM4ʱ��ʹ��    
 	TIM4->ARR=arr;  	//�趨�������Զ���װֵ 
	TIM4->PSC=psc;  	//Ԥ��Ƶ��	  
	TIM4->DIER|=1<<0;   //��������ж�	  
	TIM4->CR1|=0x01;    //ʹ�ܶ�ʱ��3
  MY_NVIC_Init(3,3,TIM3_IRQn,2);	//��ռ2�������ȼ�3����2	
	
}

//��ʱ��4�жϷ�����
void TIM4_IRQHandler(void)
{
	if(TIM4->SR&0X0001)//����ж� 
	{
		GUI_TOUCH_Exec();
	}
	TIM4->SR&=~(1<<0);//����жϱ�־λ 
}
