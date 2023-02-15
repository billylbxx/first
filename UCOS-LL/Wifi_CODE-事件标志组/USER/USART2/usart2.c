#include "usart2.h"
USART2_TypeDef Usart2;
/*********************************************************************************************************
* �� �� �� : USART3_SendString
* ����˵�� : USART3�����ַ�������
* ��    �� : str����Ҫ���͵��ַ���
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void Usart2_SendString(unsigned char *str)
{
	while(*str != 0)		
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	//�ȴ���һ���ֽڷ������
		USART_SendData(USART2, *str++);								//����һ���ֽ�
	}
}
/*********************************************************************************************************
* �� �� �� : USART2_SendPackage
* ����˵�� : USART2���ݰ����ͺ���
* ��    �� : data����Ҫ���͵����ݣ�len�����͵����ݵĳ���
* �� �� ֵ : ��
* ��    ע : ���ݰ��м���ܻ������\0��, ������Ҫ�������Ƚ��з���
*********************************************************************************************************/ 
void USART2_SendPackage(unsigned char *data, unsigned short len)	
{
	while(len--)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	//�ȴ���һ���ֽڷ������
		USART_SendData(USART2, *data++);							//����һ���ֽ�
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
	if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)				//����յ������ݣ��жϴ�����ʲô�жϣ�
	{
		Usart2.RxBuff[Usart2.RecLen] = USART_ReceiveData(USART2);	//��ȡ����
		if(++Usart2.RecLen > 255)									//������յ������ݳ�����������С����ô��ͷ��ʼ���
			Usart2.RecLen = 0;
	}
	else if(USART_GetITStatus(USART2, USART_IT_IDLE) == SET)		//��������˿����жϣ���ô�ַ����������
	{
		//Usart2.RxBuff[Usart2.RecLen]= 0;			//�ڵ�ǰ��ĩβ��ӽ�����			
		Usart2.RecFlag = 1;											//��ʾλ��1��֪ͨ��������д���
		USART_ReceiveData(USART2);							//��ȡDR���SR����
		USART2->SR;
	}
}
/*********************************************************************************************************
* �� �� �� : USART2_Init
* ����˵�� : ��ʼ��USART2
* ��    �� : bound��������
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void Usart2_Init(unsigned int bound)
{
	GPIO_InitTypeDef 	GPIO_InitStruct		= {0};
	USART_InitTypeDef 	USART_InitStruct	= {0};
	NVIC_InitTypeDef 	NVIC_InitStruct		= {0};
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);		//ʹ��USART3����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,  ENABLE);		//ʹ��PA����ʱ��

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2, GPIO_AF_USART2);	//PA9����USART3����
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3, GPIO_AF_USART2);	//PA10����USART3����

	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3;		//���ø���ģʽ�󣬿���Ȩ��������
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;					//����ģʽ
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;					//�������
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;				//��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;				//100Mhz
	GPIO_Init(GPIOA, &GPIO_InitStruct);							//����GPIO_InitStruct��ʼ��
	
	USART_InitStruct.USART_BaudRate 	= bound;									//������
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��ʹ��Ӳ��������
	USART_InitStruct.USART_Mode 		= USART_Mode_Tx | USART_Mode_Rx;			//�շ�ģʽ
	USART_InitStruct.USART_Parity 		= USART_Parity_No;							//��ʹ����żУ��
	USART_InitStruct.USART_StopBits 	= USART_StopBits_1;							//һλֹͣλ
	USART_InitStruct.USART_WordLength 	= USART_WordLength_8b;						//��λ�����ݳ���
	USART_Init(USART2, &USART_InitStruct);
	
	USART_ITConfig(USART2,USART_IT_RXNE, ENABLE);			//ʹ�ܽ����ж�
	USART_ITConfig(USART2,USART_IT_IDLE, ENABLE);			//ʹ�ܿ����ж�

	NVIC_InitStruct.NVIC_IRQChannel    = USART2_IRQn;		//�ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//ʹ���ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 2;	//��Ӧ���ȼ�2
	NVIC_Init(&NVIC_InitStruct);	
	USART_Cmd(USART2, ENABLE);								//ʹ������
}

