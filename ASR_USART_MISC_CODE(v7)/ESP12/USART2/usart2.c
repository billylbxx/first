#include "usart2.h"
#include "usart1.h"
#include "esp12.h"
#include <stdbool.h>
USART2_InitStructure Usart2;
#define WIFI_USART USART2
/*********************************************************************************************************
* �� �� �� : Usart2_SendString
* ����˵�� : USART2�����ַ�������
* ��    �� : str����Ҫ���͵��ַ���
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void Usart2_SendString(unsigned char *data)		//�������ݣ�ĩβ���С�\0��
{
	while(*data != '\0')
	{
		while(!USART_GetFlagStatus(WIFI_USART,USART_FLAG_TC));
		USART_SendData(WIFI_USART,*data++);
	}
}
/*********************************************************************************************************
* �� �� �� : Usart2_SendPackage
* ����˵�� : USART2���ݰ����ͺ���
* ��    �� : data����Ҫ���͵����ݣ�len�����͵����ݵĳ���
* �� �� ֵ : ��
* ��    ע : ���ݰ��м���ܻ������\0��, ������Ҫ�������Ƚ��з���
*********************************************************************************************************/ 
void Usart2_SendPackage(unsigned char *data,unsigned short len)	//�������ݰ����м������\0��,������Ҫ�������Ƚ��з���
{
	while(len--)
	{
		while(!USART_GetFlagStatus(WIFI_USART,USART_FLAG_TC));
		USART_SendData(WIFI_USART,*data++);
	}
}
/*********************************************************************************************************
* �� �� �� : USART2_IRQHandler
* ����˵�� : USART2�жϷ�����
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : �����ж�+�����ж�, ÿ���յ�һ���ֽڣ��ͻ�ִ��һ��
*********************************************************************************************************/ 
void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE))							//����ǽ����ж�
	{
		Usart2.RxBuff[Usart2.RecLen] = USART_ReceiveData(WIFI_USART);		//���յ������ݴ�����
		if (++Usart2.RecLen > 255)
			Usart2.RecLen = 0;
	}
	else if (USART_GetITStatus(WIFI_USART, USART_IT_IDLE))	//����ǿ����ж�
	{
		Usart2.RxBuff[Usart2.RecLen] = '\0';		//��ӿ��ַ�
		Usart2.RecFlag = true;						//���λ��1
		USART_ReceiveData(WIFI_USART);				//��ȡDR���൱�����SR��RXNE��IDLEλ
		if(ConnectFlag == false)					//δ����ʱ���ڻ���
		{
			//Usart1_SendPackage(Usart2.RxBuff,Usart2.RecLen);
			Usart2.RecLen = 0;
			Usart2.RecFlag = true;	
		}
	}
}
/*********************************************************************************************************
* �� �� �� : Usart2_Init
* ����˵�� : ��ʼ��USART2
* ��    �� : bound��������
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void Usart2_Init(unsigned int bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);			//ʹ��USART2ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);			//ʹ��GPIOAʱ��

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);		//PA2����ӳ��USART2
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);	    //PA3����ӳ��USART2

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
	USART_Init(WIFI_USART, &USART_InitStructure);

	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;			//USART1�ж�
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//ʹ���ж�
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;			//��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStruct);

	USART_ITConfig(WIFI_USART, USART_IT_RXNE, ENABLE);			//ʹ�ܽ����ж�
	USART_ITConfig(WIFI_USART, USART_IT_IDLE, ENABLE);			//ʹ�ܿ����ж�

	USART_Cmd(WIFI_USART, ENABLE);								//ʹ��USART1
}

