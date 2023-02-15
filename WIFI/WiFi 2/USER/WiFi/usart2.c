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
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);//映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);//映射
	
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
	
	/*开中断*/
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//接收数据寄存器非空中断
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);//空闲线路检测中断
	
	NVIC_InitTypeDef NVIC_InitStruct;//根据指定的初始化NVIC外设NVIC_InitStruct中的参数。
	NVIC_InitStruct.NVIC_IRQChannel=USART2_IRQn;//USART1全球中断  在启动文件里找stm32f4xx.h
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;//先占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;			//从优先级
	NVIC_Init(&NVIC_InitStruct);  //初始化  //misc里找
}


char CH[300]={0};
int usart2_flag=0;
void USART2_IRQHandler(void)
{
	static int i=0;
	if(USART_GetITStatus(USART2,USART_IT_RXNE)==1)//判断是否发生中断 0：未接收到数据 1：已准备好读取接收到的数据  //uart
		CH[i++]=(char)USART_ReceiveData(USART2);
	if(USART_GetITStatus(USART2,USART_IT_IDLE)==1)//判断是否接收完成- -0：未检测到空闲线路 1：检测到空闲线路
	{
		USART_ReceiveData(USART2);
		CH[i]='\0';
		i=0;
		usart2_flag=1;
	}
}







