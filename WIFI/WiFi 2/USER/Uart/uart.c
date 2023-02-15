#include "uart.h"
#include "stm32f4xx.h"
#include "stdio.h"

//int fputc(int ch,FILE*file)
//{
//	while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
//	USART_SendData(USART1,ch);
//	return ch;
//}
void Uart_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};
	USART_InitTypeDef USART_InitStruct={0};
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);//PA9复用USART1功能
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);//PA10复用USART1功能
	
	//TXD--RXD
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_9|GPIO_Pin_10;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;//复用
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
//	//RXD--PA10
//	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_10;
//  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
//  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//UART配置
	USART_InitStruct.USART_BaudRate = 115200;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No ;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
  USART_Init(USART1,&USART_InitStruct);
	USART_Cmd(USART1,ENABLE);

//使能串口接收中断
	USART_ITConfig(USART1,USART_IT_RXNE, ENABLE);
//使能空闲中断
	USART_ITConfig(USART1,USART_IT_IDLE, ENABLE);

//开中断

	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;//先占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;//从优先级
	NVIC_Init(&NVIC_InitStruct);
}


////中断服务函数————中断名已有————启动文件可找到
char buff[50];
int usart_flag;
void USART1_IRQHandler(void)
{
	static int i=0;
	if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==1)//
	{
		buff[i++]=(char)USART_ReceiveData(USART1);
	}
	if(USART_GetITStatus(USART1,USART_IT_IDLE)==1)//检查指定的 USART 中断发生与否(空闲总线中断)
	{
		buff[i]='\0';
		i=0;
		usart_flag=1;
		USART_ReceiveData(USART1);//清除空闲中断标志位
	}
}


char Usart_Rec_data(void)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE)==RESET);
	return  (char)USART_ReceiveData(USART1);
}

void Usart_Rec_string(char* sp)
{
	int i;
	for(i=0; ;i++)
	{
		sp[i]=Usart_Rec_data();
		if(sp[i]=='\r'||sp[i]=='\n')
	{
		sp[i]='\0';
		break;
	}

	}
}

/*********************************************
函数名：void USART1_SendByte(u8 data)
函数功能：发送一个字节
函数形参：u8 data
函数返回值：无
函数备注：
********************************************/
void USART1_SendByte(char data)
{
//	while(!(USART1->SR &1<<7)){;}  //等待发送完成  阻塞
//	USART1->DR = data;
	
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==RESET);
	USART_SendData(USART1,data);
}

/*********************************************
函数名：void USART1_SendStr(u8 *str)
函数功能：发送一串字符串
函数形参：u8 *str  
函数返回值：无
函数备注：USART1_SendStr("abcd");
********************************************/
void USART1_SendStr(char *str)
{
	while(*str!='\0') 
	{
		USART1_SendByte(*str);
		str++;
	}
}

