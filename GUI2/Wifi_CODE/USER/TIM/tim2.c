#include "tim2.h"

//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft s.

void tim2_Init(int ARR,int PSC)
{
  //ʹ��CCR
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
  
	//������� 
	
  GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//��GPIOʱ��

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM2); //ʹ�ܶ�ʱ��2���õ�GPIOA��
 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//�򿪶�ʱ��2ʱ��
	//GOIO��ʼ��
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;//�������ģʽ
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;//�������
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1;//TIM2_CH2
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;//��������
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;//�ٶ�50M
	GPIO_Init(GPIOA, &GPIO_InitStruct);//GPIO��ʼ��
	
	//��ʱ����ʼ��
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;//����ʱ�ӷָ�
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;//��ʱ�����ϼ���
	TIM_TimeBaseInitStruct.TIM_Period=ARR;//�Զ���װ��ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler=PSC;//ʱ��Ԥ��Ƶ�� 
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);//��ʱ����ʼ��

	//��ʼ��TIM2 Channel2 PWMģʽ
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;//ѡ��ģʽ1��TIM�����ȵ���ģʽ2
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//�Ƚ����ʹ��
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;//������ԣ�TIM����Ƚϼ��Ը�
	TIM_OCInitStruct.TIM_Pulse=5;//���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OC2Init(TIM2,&TIM_OCInitStruct);////����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIM2
	
	TIM_Cmd(TIM2,	ENABLE);//ʹ�ܶ�ʱ��2


}

//void PWM1(int CCR)
//{
//	TIM_SetCompare2(TIM2, CCR);

//}



/*********************************************************************************************************
* �� �� �� : SetServoAngle
* ����˵�� : ���ö���ĽǶȣ�0~180
* ��    �� : angle���Ƕ�ֵ
* �� �� ֵ : ��
* ��    ע : ֻ��Ҫ����һ�Σ�������ɱ��ֽǶȣ�0.5ms--0��  2.5ms--180��

0.5ms------------0 �ȣ�     
1.0ms------------45 �ȣ�
1.5ms------------90 �ȣ�   
2.0ms-----------135 �ȣ�
2.5ms-----------180 �ȣ�

*********************************************************************************************************/ 
void Servo_SetAngle(unsigned char angle)
{
	unsigned short pulse = 0;
	//��Զ����ת�Ƕ��޷�
	if(angle < 5)		angle = 5;
	if(angle > 175)	angle = 5;
	//���Ƕ�ֵת��Ϊ����ֵ 
	pulse = (unsigned short)(50 + angle * 100 / 90.0);  //��ת����ʽ�����pwm��arr��psc����������Ӧ�仯
	//TIM2->CCR2 = pulse;
	TIM_SetCompare2(TIM2, pulse);
}

