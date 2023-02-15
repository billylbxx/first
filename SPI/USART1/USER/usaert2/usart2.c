#include "usart2.h"
#include "uart1.h"
#include "stdbool.h"

#define WIFI_USART USART2

USART2_InitStructure Usart2;

void USART2_IRQHandler(void)
{	
	u8 date;
	if (USART_GetITStatus(USART2, USART_IT_RXNE))			//如果是接收中断
	{
		if (Usart2.FlagLen >= 256)
			Usart2.FlagLen = 0;
		 date = USART_ReceiveData(USART2);	//把收到的数据存起来
		 USART1_SendByte(date);//回显到电脑串口调试助手
		 Usart2.RxBuff[Usart2.FlagLen++] = date;
		
	}
	else if (USART_GetITStatus(USART2, USART_IT_IDLE))	//如果是空闲中断
	{
		Usart2.RxBuff[Usart2.FlagLen] = '\0';	//添加空字符
		Usart2.FlagLen |= 0x8000;				//最高位置1
		Usart2.RecFlag = true;
		USART_ReceiveData(USART2);				//读取DR，相当于清空SR的RXNE和IDLE位
	}
}


void SendUsart2Data(u8 *data)		//发送数据，末尾才有‘\0’
{
	while(*data != '\0')
	{
		while(!USART_GetFlagStatus(WIFI_USART,USART_FLAG_TC));
		USART_SendData(WIFI_USART,*data++);
	}
}

void SendUsart2Package(u8 *data,u16 len)	//发送数据包，中间包含‘\0’,所以需要依赖长度进行发送
{
	while(len--)
	{
		while(!USART_GetFlagStatus(WIFI_USART,USART_FLAG_TC));
		USART_SendData(WIFI_USART,*data++);
	}
}
/*
*********************************************************************************************************
*	函 数：Usart2_Init
*	功能说明：初始化USART2
*	形    参：无
*	返 回 值：无
*********************************************************************************************************
*/
void Usart2_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);			//使能USART2时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);			//使能GPIOA时钟

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);		//PA2复用映射USART2
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);	  //PA3复用映射USART2

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//50M
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = bound;					//波特率（0到7500001）
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件控制流
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_InitStructure.USART_Parity = USART_Parity_No;				//无奇偶校验
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//一位停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//八位数据宽度
	USART_Init(USART2, &USART_InitStructure);

	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;			//USART2中断
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//使能中断
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;			//响应优先级
	NVIC_Init(&NVIC_InitStruct);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);			//使能接收中断
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);			//使能空闲中断

	USART_Cmd(USART2, ENABLE);								//使能USART2
}

