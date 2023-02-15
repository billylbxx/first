#include "stm32f4xx.h"
#include "exti.h"
#include "key.h"
#include "led.h"
#include "systick.h"
#include "tim.h"
#include "uart.h"
void Exti_Init(void)
{
	//ʱ�ӣ�������ĸ�IO�ڣ���rcc,syscfg���ң�
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);//KEY2


	//����IO��
	key_Init();
	
	//����ģ���ڲ��Ĵ���  exti.c
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line   =EXTI_Line13;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode   =EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);
		
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);//KEY1
	EXTI_InitStruct.EXTI_Line   =EXTI_Line0;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode   =EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);//KEY3
	EXTI_InitStruct.EXTI_Line   =EXTI_Line1;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode   =EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);
	
	//�ж�����  misc
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel =EXTI15_10_IRQn;//��stm32f4xx.h�ļ�����
	NVIC_InitStruct.NVIC_IRQChannelCmd =ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority  =1;  //0~3
	NVIC_InitStruct.NVIC_IRQChannelSubPriority         =2;  //0~3
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel =EXTI0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd =ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority  =1;  //0~3
	NVIC_InitStruct.NVIC_IRQChannelSubPriority         =2;  //0~3
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel =EXTI1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd =ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority  =1;  //0~3
	NVIC_InitStruct.NVIC_IRQChannelSubPriority         =2;  //0~3
	NVIC_Init(&NVIC_InitStruct);
}
//�����ļ�  .s
void delay(unsigned int t)
{
	while(t--);
}

static  int time=0;
static unsigned int  key_flag=0;
static int hour_flag=0;

void EXTI0_IRQHandler(void)          //KEY1  PA0
{
	if(EXTI_GetITStatus(EXTI_Line0) ==1)
	{
		while(KEY1==1);           // �ȴ������ͷ�
		delay(500000);//��ʱȥ����
		if(EXTI_GetITStatus(EXTI_Line0) ==1)
		{
			EXTI_ClearITPendingBit(EXTI_Line0);//���־λ
			LED1=~LED1;
			printf("key1�Ѱ���\r\n");
			key_flag++;
			if(key_flag>4) key_flag=0;
		}
	}
	switch(key_flag)
	{
		case 1: TIM_ITConfig(TIM6,TIM_IT_Update,DISABLE);break;//�ö�ʱ�ж�ʧ�ܣ�����ͣ��ʱ��
		case 2: time=hour;
						hour_flag=1;
						printf("hour=%d\r\n",time);break;
		case 3: hour=time;
						time=min;
			      hour_flag=0;
				    printf("min=%d\r\n",time);break;
		case 4: min=time;
						time=sec; 
			      hour_flag=0;
            printf("sec=%d\r\n",time);break;
		
		case 0:	sec=time;
						TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);break;//ʹ�ܶ�ʱ�ж�,��������ʱ��
	}
}

void EXTI15_10_IRQHandler(void)      //KEY2    PC13
{
	//��ѯʱ�������߲����ж�exti.c
	if(EXTI_GetITStatus(EXTI_Line13) ==1)
	{
		while(KEY2==0);// �ȴ������ͷ�
		delay(500000);//��ʱȥ����
		if(EXTI_GetITStatus(EXTI_Line13) ==1)
		{
			EXTI_ClearITPendingBit(EXTI_Line13);//���־λ
			LED2=~LED2;
			time++;
			if(hour_flag==0)
			{
				if(time>=60) time=0;
			}
			else if(hour_flag==1)
			{
				if(time>=24) time=0;
			}
			printf("%d\r\n",time);
		}
	}
}
void EXTI1_IRQHandler(void)       //KEY3    PB1
{
	if(EXTI_GetITStatus(EXTI_Line1) ==1)
	{
		while(KEY3==0);// �ȴ������ͷ�
		delay(500000);//��ʱȥ����
		if(EXTI_GetITStatus(EXTI_Line1) ==1)
		{
			EXTI_ClearITPendingBit(EXTI_Line1);//���־λ
			LED3=~LED3;
			time--;
			if(hour_flag==0)
			{
				if(time<0) time=59;
			}
			else if(hour_flag==1)
			{
				if(time<0) time=23;
			}
			printf("%d\r\n",time);
		}
	}
}







