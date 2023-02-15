#include "tim3.h"
#include "led.h"

void tim3_Init(int ARR,int PSC)
{
	//ʹ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	//IO�ڸ���
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM3);
	//�����������
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;//����
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;//�ܽ�
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;//����
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct); 
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8;//�ܽ�
	GPIO_Init(GPIOC,&GPIO_InitStruct); 
	
	//��ʱ��ʱ��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_Prescaler         =ARR-1;  //���㿪ʼ  
	TIM_TimeBaseInitStruct.TIM_Period            =PSC-1;   //��ʽ  //Ԥ��Ƶ
	TIM_TimeBaseInitStruct.TIM_ClockDivision     =TIM_CKD_DIV1; //�ޱ�Ƶ
	TIM_TimeBaseInitStruct.TIM_CounterMode       =TIM_CounterMode_Up;   //������ģʽ
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter =0;  //����Ϊ0��ʱ�����ڲ��������߼���ʱ��ʹ�ã�
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
	
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;   //PWM1ģʽ
//	TIM_OCInitStruct.TIM_OCIdleState      =
//	TIM_OCInitStruct.TIM_OCNIdleState=    //�߼���ʱ��
//	TIM_OCInitStruct.TIM_OCNPolarity=
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCNPolarity_Low;  //��Ч��ƽ0 
//	TIM_OCInitStruct.TIM_OutputNState=
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable ;   //ʹ�����
	TIM_OCInitStruct.TIM_Pulse     =  (ARR-1)/2;   //�Ƚ�ֵ
	TIM_OC3Init(TIM3, &TIM_OCInitStruct);
	
	//����ʱ��
	TIM_Cmd(TIM3, ENABLE);

  //ʹ��CCR
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
 

}

//void LED3_PWM(int CCR)
//{
//	TIM_SetCompare3(TIM3,CCR);  //�������ֵ
//}


void FAN_PWM(int CCR)
{
	TIM_SetCompare3(TIM3,CCR);  //�������ֵ
}
