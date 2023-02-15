#include "beep.h"
/*********************************************************************************************************
* �� �� �� : Beep_Init
* ����˵�� : ��ʼ��BEEP�˿�
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ������PC12��BEEP��
*********************************************************************************************************/ 
void Beep_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	//ʹ��PC����ʱ��
	
	GPIO_InitStruct.GPIO_Pin 	= GPIO_Pin_12;				//��ʼ������
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_OUT;			//���ģʽ
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;			//�������
	GPIO_InitStruct.GPIO_PuPd 	= GPIO_PuPd_NOPULL;			//��������
	GPIO_InitStruct.GPIO_Speed 	= GPIO_Speed_2MHz;			//2MHZ
	GPIO_Init(GPIOC, &GPIO_InitStruct);						//����GPIO_InitStruct��ʼ��
	
	GPIO_ResetBits(GPIOC, GPIO_Pin_12);						//����͵�ƽ������������
}

