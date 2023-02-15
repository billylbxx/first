#include "pwm.h"
/*********************************************************************************************************
* �� �� �� : Led2Pwm_Init
* ����˵�� : ��ʼ��TIM3��CH3�����PWM����LED2
* ��    �� : arr����װ�أ�psc��Ԥ��Ƶ
* �� �� ֵ : ��
* ��    ע : Fpwm��HZ�� = Fclk / ����arr+1��*��psc+1����
			 Duty_ratio��%�� = CRR1 / ARR
*********************************************************************************************************/ 
void Led2Pwm_Init(unsigned short arr,unsigned short psc)
{
	GPIO_InitTypeDef 		GPIO_InitStruct			= {0};
	TIM_OCInitTypeDef 		TIM_OCInitStruct		= {0};
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct	= {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	//ʹ��PB����ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,  ENABLE);	//��������ʱ��
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);	//PB0����TIM3����
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0;		//��ʼ������
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;		//����ģʽ
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;		//�������
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	//50MHZ
	GPIO_Init(GPIOB, &GPIO_InitStruct);				//����GPIO_InitStruct��ʼ��
	
	TIM_TimeBaseInitStruct.TIM_Period        = arr-1;				//��װ��
	TIM_TimeBaseInitStruct.TIM_Prescaler     = psc-1;				//Ԥ��Ƶ
	TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;	//���ϼ���
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;		//����Ƶ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	
	TIM_OCInitStruct.TIM_OCMode      = TIM_OCMode_PWM1;			//PWMģʽ1��TIMx_CNT<TIMx_CCR1ʱΪ��Ч��ƽ
	TIM_OCInitStruct.TIM_OCPolarity  = TIM_OCPolarity_Low;		//�������Ϊ��
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	//�Ƚ����ʹ��
	TIM_OCInitStruct.TIM_Pulse 		 = 0;						//���ñȽϼĴ����ĳ�ֵ
	TIM_OC3Init(TIM3, &TIM_OCInitStruct);
	
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);			//ʹ��CCR3��Ԥװ��
	TIM_Cmd(TIM3, ENABLE);										//ʹ��TIM3����
}	
/*********************************************************************************************************
* �� �� �� : HollowPwm_Init
* ����˵�� : ��ʼ��TIM8��CH3�����PWM�������ı�
* ��    �� : arr����װ�أ�psc��Ԥ��Ƶ
* �� �� ֵ : ��
* ��    ע : Fpwm��HZ�� = Fclk / ����arr+1��*��psc+1����
			 Duty_ratio��%�� = CRR1 / ARR
			 �ô˶�ʱ�����ƿ��ı�ʱ������ʹ���������TIM1 or TIM8��
*********************************************************************************************************/ 
void HollowPwm_Init(unsigned short arr,unsigned short psc)
{
	GPIO_InitTypeDef 		GPIO_InitStruct			= {0};
	TIM_OCInitTypeDef 		TIM_OCInitStruct		= {0};
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct	= {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);		//ʹ��PC����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,  ENABLE);		//����TIM8����ʱ��
	
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM8);		//PC8����TIM8����
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8;		//��ʼ������
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;		//����ģʽ
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;		//�������
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	//50MHZ
	GPIO_Init(GPIOC, &GPIO_InitStruct);				//����GPIO_InitStruct��ʼ��
	
	TIM_TimeBaseInitStruct.TIM_Period 		 = arr-1;				//��װ��
	TIM_TimeBaseInitStruct.TIM_Prescaler 	 = psc-1;				//Ԥ��Ƶ
	TIM_TimeBaseInitStruct.TIM_CounterMode 	 = TIM_CounterMode_Up;	//���ϼ���
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;		//����Ƶ
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseInitStruct);
	
	TIM_OCInitStruct.TIM_OCMode 	 = TIM_OCMode_PWM1;			//PWMģʽ2��TIMx_CNT<TIMx_CCR1ʱΪ��Ч��ƽ
	TIM_OCInitStruct.TIM_OCPolarity  = TIM_OCPolarity_High;		//�������Ϊ��
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	//�Ƚ����ʹ��
	TIM_OCInitStruct.TIM_Pulse 		 = 0;						//���ñȽϼĴ����ĳ�ֵ
	TIM_OC3Init(TIM8, &TIM_OCInitStruct);
       
	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);			//ʹ��CCR3��Ԥװ��
	TIM_CtrlPWMOutputs(TIM8, ENABLE);							//ʹ�������
	TIM_Cmd(TIM8, ENABLE);										//ʹ��TIM8����
}	
/*********************************************************************************************************
* �� �� �� : SteeringPwm_Init
* ����˵�� : ��ʼ��TIM2��CH2�����PWM�������
* ��    �� : arr����װ�أ�psc��Ԥ��Ƶ
* �� �� ֵ : ��
* ��    ע : Fpwm��HZ�� = Fclk / ����arr+1��*��psc+1����
			 Duty_ratio��%�� = CRR1 / ARR
			 84M / 840 = 0.1M �ʼ�һ������ʱ��Ϊ10us  ��ʱ���ڣ���1999 + 1�� * 10us = 20ms
*********************************************************************************************************/ 
void SteeringPwm_Init(unsigned short arr,unsigned short psc)
{
	GPIO_InitTypeDef 		GPIO_InitStruct			= {0};
	TIM_OCInitTypeDef 		TIM_OCInitStruct		= {0};
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct	= {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	//ʹ��PA����ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,  ENABLE);	//����TIM2����ʱ��
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);	//PA1����TIM2����
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_1;			//��ʼ������
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;		//����ģʽ
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;		//�������
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	//50MHZ
	GPIO_Init(GPIOA, &GPIO_InitStruct);				//����GPIO_InitStruct��ʼ��
	
	TIM_TimeBaseInitStruct.TIM_Period 		 = arr-1;					//��װ��
	TIM_TimeBaseInitStruct.TIM_Prescaler 	 = psc-1;					//Ԥ��Ƶ
	TIM_TimeBaseInitStruct.TIM_CounterMode 	 = TIM_CounterMode_Up;		//���ϼ���
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;			//����Ƶ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
	
	TIM_OCInitStruct.TIM_OCMode 	 = TIM_OCMode_PWM1;			//PWMģʽ1��TIMx_CNT<TIMx_CCR1ʱΪ��Ч��ƽ
	TIM_OCInitStruct.TIM_OCPolarity  = TIM_OCPolarity_High;		//�������Ϊ��
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	//�Ƚ����ʹ��
	TIM_OCInitStruct.TIM_Pulse		 = 0;						//���ñȽϼĴ����ĳ�ֵ
	TIM_OC2Init(TIM2, &TIM_OCInitStruct);
	
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);			//ʹ��CCR2��Ԥװ��
	TIM_Cmd(TIM2, ENABLE);										//ʹ��TIM2����
}
/*********************************************************************************************************
* �� �� �� : SetLedBrightness
* ����˵�� : ����led������
* ��    �� : value�����Ȱٷֱȣ�0~100
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void SetLedBrightness(char brightness)
{
	if(brightness >= 100) 		brightness = 100;
	else if(brightness <= 0)	brightness = 0;
	TIM3->CCR3 = brightness;
	//TIM_SetCompare3(TIM3, brightness);
}
/*********************************************************************************************************
* �� �� �� : SetHollowSpeed
* ����˵�� : ���ÿ��ı���ת��
* ��    �� : value��ת�ٰٷֱȣ�0~100
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void SetHollowSpeed(char speed)
{
	if(speed >= 100) 	speed = 100;
	else if(speed <= 0)	speed = 0;
	TIM8->CCR3 = speed;
	//TIM_SetCompare3(TIM8, speed);
}
/*********************************************************************************************************
* �� �� �� : SetServoAngle
* ����˵�� : ���ö���ĽǶȣ�0~180
* ��    �� : angle���Ƕ�ֵ
* �� �� ֵ : ��
* ��    ע : ֻ��Ҫ����һ�Σ�������ɱ��ֽǶȣ�0.5ms--0��  2.5ms--180��
*********************************************************************************************************/ 
void SetServoAngle(unsigned char angle)
{
	unsigned short pulse = 0;
	//��Զ����ת�Ƕ��޷�
	if(angle <= 5)		angle = 5;
	if(angle >= 175)	angle = 175;
	//���Ƕ�ֵת��Ϊ����ֵ  
	pulse = (unsigned short)(50 + angle * 100 / 90.0);  //��ת����ʽ�����pwm��arr��psc����������Ӧ�仯
	TIM2->CCR2 = pulse;
	//TIM_SetCompare2(TIM2, pulse);
}
 

