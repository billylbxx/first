#include "uart4.h"
UART4_TypeDef Uart4;
/*********************************************************************************************************
* �� �� �� : Uart4_SendString
* ����˵�� : UART4�����ַ�������
* ��    �� : str����Ҫ���͵��ַ���
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void Uart4_SendString(char *str)
{
	while(*str != 0)		
	{
		while(USART_GetFlagStatus(UART4,USART_FLAG_TC) == RESET);	//�ȴ���һ���ֽڷ������
		USART_SendData(UART4,*str++);								//����һ���ֽ�
	}
}
/*********************************************************************************************************
* �� �� �� : Uart4_SendPackage
* ����˵�� : UART4���ݰ����ͺ���
* ��    �� : data����Ҫ���͵����ݣ�len�����͵����ݵĳ���
* �� �� ֵ : ��
* ��    ע : ���ݰ��м���ܻ������\0��, ������Ҫ�������Ƚ��з���
*********************************************************************************************************/ 
void Uart4_SendPackage(unsigned char *str, unsigned short len)
{
	while(len--)		
	{
		while(USART_GetFlagStatus(UART4,USART_FLAG_TC) == RESET);	//�ȴ���һ���ֽڷ������
		USART_SendData(UART4,*str++);								//����һ���ֽ�
	}
}
/*********************************************************************************************************
* �� �� �� : UART4_IRQHandler
* ����˵�� : UART4�жϷ�����
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : �����ж�+�����ж�, ÿ���յ�һ���ֽڣ��ͻ�ִ��һ��
*********************************************************************************************************/ 
void UART4_IRQHandler(void)	//ÿ���յ�һ���ֽڣ��ͻ�ִ��һ��
{
	if(USART_GetITStatus(UART4,USART_IT_RXNE) == SET)	//����յ������ݣ��жϴ�����ʲô�жϣ�
	{
		Uart4.RxBuff[Uart4.RecLen] = USART_ReceiveData(UART4);		//��ȡ����
		if(++Uart4.RecLen > 255)									//������յ������ݳ�����������С����ô��ͷ��ʼ���
			Uart4.RecLen = 0;
	}
	else if(USART_GetITStatus(UART4,USART_IT_IDLE) == SET)	//��������˿����жϣ���ô�ַ����������
	{
		Uart4.RxBuff[Uart4.RecLen] = 0;								//�ڵ�ǰ��ĩβ��ӽ�����			
		Uart4.RecFlag = true;										//��ʾλ��1��֪ͨ��������д���
		USART_ReceiveData(UART4);									//��ȡDR���SR����
	}
}
/*********************************************************************************************************
* �� �� �� : Uart4_Init
* ����˵�� : ��ʼ��UART4
* ��    �� : bound��������
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void Uart4_Init(unsigned int bound)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);	//ʹ��UART4����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);	//ʹ��PC����ʱ��

	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4);	//PC10����USRT4����
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4); //PC11����UART4����

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;	//���ø���ģʽ�󣬿���Ȩ��������
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;				//����ģʽ
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;				//�������
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;			//��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;			//50Mhz
	GPIO_Init(GPIOC, &GPIO_InitStruct);						//����GPIO_InitStruct��ʼ��
	
	USART_InitStruct.USART_BaudRate = bound;						//������
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��ʹ��Ӳ��������
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//�շ�ģʽ
	USART_InitStruct.USART_Parity = USART_Parity_No;				//��ʹ����żУ��
	USART_InitStruct.USART_StopBits = USART_StopBits_1;				//һλֹͣλ
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;		//��λ�����ݳ���
	USART_Init(UART4, &USART_InitStruct);
	
	USART_ITConfig(UART4,USART_IT_RXNE,ENABLE);			//ʹ�ܽ����ж�
	USART_ITConfig(UART4,USART_IT_IDLE,ENABLE);			//ʹ�ܿ����ж�

	NVIC_InitStruct.NVIC_IRQChannel = UART4_IRQn;			//�ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//ʹ���ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;			//��Ӧ���ȼ�2
	NVIC_Init(&NVIC_InitStruct);	
	
	USART_Cmd(UART4,ENABLE);		//ʹ������
}
