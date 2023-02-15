#include "usart2.h"
#include "uart1.h"
#include "stdbool.h"

#define WIFI_USART USART2

USART2_InitStructure Usart2;

void USART2_IRQHandler(void)
{	
	u8 date;
	if (USART_GetITStatus(USART2, USART_IT_RXNE))			//����ǽ����ж�
	{
		if (Usart2.FlagLen >= 256)
			Usart2.FlagLen = 0;
		 date = USART_ReceiveData(USART2);	//���յ������ݴ�����
		 USART1_SendByte(date);//���Ե����Դ��ڵ�������
		 Usart2.RxBuff[Usart2.FlagLen++] = date;
		
	}
	else if (USART_GetITStatus(USART2, USART_IT_IDLE))	//����ǿ����ж�
	{
		Usart2.RxBuff[Usart2.FlagLen] = '\0';	//��ӿ��ַ�
		Usart2.FlagLen |= 0x8000;				//���λ��1
		Usart2.RecFlag = true;
		USART_ReceiveData(USART2);				//��ȡDR���൱�����SR��RXNE��IDLEλ
	}
}


void SendUsart2Data(u8 *data)		//�������ݣ�ĩβ���С�\0��
{
	while(*data != '\0')
	{
		while(!USART_GetFlagStatus(WIFI_USART,USART_FLAG_TC));
		USART_SendData(WIFI_USART,*data++);
	}
}

void SendUsart2Package(u8 *data,u16 len)	//�������ݰ����м������\0��,������Ҫ�������Ƚ��з���
{
	while(len--)
	{
		while(!USART_GetFlagStatus(WIFI_USART,USART_FLAG_TC));
		USART_SendData(WIFI_USART,*data++);
	}
}
/*
*********************************************************************************************************
*	�� ����Usart2_Init
*	����˵������ʼ��USART2
*	��    �Σ���
*	�� �� ֵ����
*********************************************************************************************************
*/
void Usart2_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);			//ʹ��USART2ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);			//ʹ��GPIOAʱ��

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);		//PA2����ӳ��USART2
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);	  //PA3����ӳ��USART2

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//����ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//50M
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = bound;					//�����ʣ�0��7500001��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_InitStructure.USART_Parity = USART_Parity_No;				//����żУ��
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//һλֹͣλ
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//��λ���ݿ��
	USART_Init(USART2, &USART_InitStructure);

	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;			//USART2�ж�
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//ʹ���ж�
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;			//��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStruct);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);			//ʹ�ܽ����ж�
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);			//ʹ�ܿ����ж�

	USART_Cmd(USART2, ENABLE);								//ʹ��USART2
}

