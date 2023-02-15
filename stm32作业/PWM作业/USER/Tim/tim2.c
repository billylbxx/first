#include "tim2.h"
#include "stm32f4xx.h"
#include "led.h"
#include "lcd.h"
#include "systick.h"
//PA1/ADC_IN1/TIM2_CH2
//TIM2_CH1_ETR
void tim2_Init(int ARR,int PSC)
{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM2);
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_1;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOA,&GPIO_InitStruct);
	
		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_15;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOA,&GPIO_InitStruct);

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	
		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
		TIM_TimeBaseInitStruct.TIM_Period = ARR-1;//�Զ����ص����������Զ���װ�ؼĴ������ڵ�ֵ
		TIM_TimeBaseInitStruct.TIM_Prescaler = PSC-1;  //Ԥ��Ƶ��
		TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//ʱ�ӷָ�
		TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;//���������ڼ�����ֵ0������
		TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);

		TIM_OCInitTypeDef TIM_OCInitStruct;
		TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;//PWM1ģʽ
		TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;//��Ч��ƽ1
		TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//ʹ�����
		TIM_OCInitStruct.TIM_Pulse=(ARR-1)/2;//�Ƚ�ֵ
		TIM_OC2Init(TIM2,&TIM_OCInitStruct);

		TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;//PWM1ģʽ
		TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;//��Ч��ƽ1
		TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//ʹ�����
		TIM_OCInitStruct.TIM_Pulse=(ARR-1)/2;//�Ƚ�ֵ
		TIM_OC1Init(TIM2,&TIM_OCInitStruct);

		//ʹ��CCR
		TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);
		TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);
		//����ʱ��
		TIM_Cmd(TIM2,ENABLE);	
}

/*********************************************************************************************************
* �� �� �� : SetServoAngle
* ����˵�� : ���ö���ĽǶȣ�0~180
* ��    �� : angle���Ƕ�ֵ
* �� �� ֵ : ��
* ��    ע : ֻ��Ҫ����һ�Σ�������ɱ��ֽǶȣ�0.5ms--0��  2.5ms--180��
*********************************************************************************************************/ 
void Servo_SetAngle(unsigned char angle)
{
	unsigned short pulse = 0;
	//��Զ����ת�Ƕ��޷�
	if(angle <= 5)		angle = 5;
	if(angle >= 175)	angle = 175;
	//���Ƕ�ֵת��Ϊ����ֵ 
	pulse = (unsigned short)(50 + angle * 100 / 90.0);  //��ת����ʽ�����pwm��arr��psc����������Ӧ�仯
	//TIM2->CCR2 = pulse;
	//TIM_SetCompare2(TIM2, pulse);
}

void lcd_ld(unsigned char CCR)
{
	TIM_SetCompare1(TIM2, CCR);
}
