#include "led.h"



/*********************************************************************************************************
* �� �� �� : Led_Init
* ����˵�� : LED�˿ڳ�ʼ��
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : PC4��LED1����PC5��LED2����PB0��LED3��
*********************************************************************************************************/ 

void Led_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);	
	GPIO_SetBits(GPIOC,GPIO_Pin_4 | GPIO_Pin_5);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_SetBits(GPIOB,GPIO_Pin_0);
	
		
}


