#include "led.h"
/*********************************************************************************************************
* �� �� �� : Led_Init
* ����˵�� : LED�˿ڳ�ʼ��
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : PC4��LED1����PB0��LED2����PC5��LED3��
*********************************************************************************************************/ 
void Led_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	//ʹ��PC����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	//ʹ��PC����ʱ��
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_4 | GPIO_Pin_5;	//��ʼ������
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;				//���ģʽ
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;				//�������
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;			//��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;			//2MHZ
	GPIO_Init(GPIOC, &GPIO_InitStruct);						//����GPIO_InitStruct��ʼ��
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStruct);						//����GPIO_InitStruct��ʼ��
	
	GPIO_SetBits(GPIOC, GPIO_Pin_4 | GPIO_Pin_5);			//����ߵ�ƽ������
	GPIO_SetBits(GPIOB, GPIO_Pin_0);						//����ߵ�ƽ������
}

