#include "usart3.h"

USART3_TypeDef Usart3;

/*********************************************************************************************************
* �� �� �� : Usart3_SendString
* ����˵�� : USART3�����ַ�������
* ��    �� : str����Ҫ���͵��ַ���
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void Usart3_SendString(char *str)
{
	while(*str != 0)		
	{
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET);	//�ȴ���һ���ֽڷ������
		USART_SendData(USART3,*str++);								//����һ���ֽ�
	}
}
/*********************************************************************************************************
* �� �� �� : Usart3_SendPackage
* ����˵�� : USART3���ݰ����ͺ���
* ��    �� : data����Ҫ���͵����ݣ�len�����͵����ݵĳ���
* �� �� ֵ : ��
* ��    ע : ���ݰ��м���ܻ������\0��, ������Ҫ�������Ƚ��з���
*********************************************************************************************************/ 
void Usart3_SendPackage(unsigned char *data,unsigned short len)	//�������ݰ����м������\0��,������Ҫ�������Ƚ��з���
{
	while(len--)
	{
		while(!USART_GetFlagStatus(USART3,USART_FLAG_TC));
		USART_SendData(USART3,*data++);
	}
}
/*********************************************************************************************************
* �� �� �� : USART3_IRQHandler
* ����˵�� : USART3�жϷ�����
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : �����ж�+�����ж�, ÿ���յ�һ���ֽڣ��ͻ�ִ��һ��
*********************************************************************************************************/ 
void USART3_IRQHandler(void)									
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)				//����յ������ݣ��жϴ�����ʲô�жϣ�
	{
		Usart3.RxBuff[Usart3.RecLen] = USART_ReceiveData(USART3);	//��ȡ����
		if(++Usart3.RecLen > 255)									//������յ������ݳ�����������С����ô��ͷ��ʼ���
			Usart3.RecLen = 0;
	}
	else if(USART_GetITStatus(USART3, USART_IT_IDLE) == SET)		//��������˿����жϣ���ô�ַ����������
	{
		Usart3.RxBuff[Usart3.RecLen] = 0;							//�ڵ�ǰ��ĩβ��ӽ�����			
		Usart3.RecFlag = true;										//��ʾλ��1��֪ͨ��������д���
		USART_ReceiveData(USART3);									//��ȡDR���SR����
	}
}
/*********************************************************************************************************
* �� �� �� : Usart3_Init
* ����˵�� : ��ʼ��USART3
* ��    �� : bound��������
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void Usart3_Init(unsigned int bound)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);	//ʹ��USART3����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);	//ʹ��PB����ʱ��

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);	//PB10����USART3����
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); 	//PB11����USART3����

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;	//���ø���ģʽ�󣬿���Ȩ��������
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;				//����ģʽ
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;				//�������
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;			//��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;			//50Mhz
	GPIO_Init(GPIOB, &GPIO_InitStruct);						//����GPIO_InitStruct��ʼ��
	
	USART_InitStruct.USART_BaudRate = bound;						//������
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��ʹ��Ӳ��������
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//�շ�ģʽ
	USART_InitStruct.USART_Parity = USART_Parity_No;				//��ʹ����żУ��
	USART_InitStruct.USART_StopBits = USART_StopBits_1;				//һλֹͣλ
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;		//��λ�����ݳ���
	USART_Init(USART3, &USART_InitStruct);
	
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);			//ʹ�ܽ����ж�
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);			//ʹ�ܿ����ж�

	NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;			//�ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//ʹ���ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;			//��Ӧ���ȼ�2
	NVIC_Init(&NVIC_InitStruct);	
	
	USART_Cmd(USART3,ENABLE);		//ʹ������
}
