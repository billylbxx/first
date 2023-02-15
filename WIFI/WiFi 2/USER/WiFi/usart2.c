#include "usart2.h"
#include "stdbool.h"
#include "stdio.h"

int fputc(int ch,FILE*file)
{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE)==RESET);
	USART_SendData(USART2,ch);
	return ch;
} 

void usart2_Init(void)
{
USART_InitTypeDef USART_InitStruct;
	GPIO_InitTypeDef   GPIO_InitStruct;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);//ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);//ӳ��
	
	GPIO_InitStruct.GPIO_Pin =GPIO_Pin_2 |GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF ;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	
	USART_InitStruct.USART_BaudRate = 115200;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No ;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
	USART_Init(USART2,&USART_InitStruct);
	USART_Cmd(USART2,ENABLE);
	
	/*���ж�*/
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//�������ݼĴ����ǿ��ж�
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);//������·����ж�
	
	NVIC_InitTypeDef NVIC_InitStruct;//����ָ���ĳ�ʼ��NVIC����NVIC_InitStruct�еĲ�����
	NVIC_InitStruct.NVIC_IRQChannel=USART2_IRQn;//USART1ȫ���ж�  �������ļ�����stm32f4xx.h
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//ʹ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;			//�����ȼ�
	NVIC_Init(&NVIC_InitStruct);  //��ʼ��  //misc����
}


char CH[300]={0};
int usart2_flag=0;
void USART2_IRQHandler(void)
{
	static int i=0;
	if(USART_GetITStatus(USART2,USART_IT_RXNE)==1)//�ж��Ƿ����ж� 0��δ���յ����� 1����׼���ö�ȡ���յ�������  //uart
		CH[i++]=(char)USART_ReceiveData(USART2);
	if(USART_GetITStatus(USART2,USART_IT_IDLE)==1)//�ж��Ƿ�������- -0��δ��⵽������· 1����⵽������·
	{
		USART_ReceiveData(USART2);
		CH[i]='\0';
		i=0;
		usart2_flag=1;
	}
}







