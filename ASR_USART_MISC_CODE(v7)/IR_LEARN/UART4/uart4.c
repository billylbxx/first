#include "uart4.h"
UART4_TypeDef Uart4;
/*********************************************************************************************************
* 函 数 名 : Uart4_SendString
* 功能说明 : UART4发送字符串函数
* 形    参 : str：需要发送的字符串
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Uart4_SendString(char *str)
{
	while(*str != 0)		
	{
		while(USART_GetFlagStatus(UART4,USART_FLAG_TC) == RESET);	//等待上一个字节发送完成
		USART_SendData(UART4,*str++);								//发送一个字节
	}
}
/*********************************************************************************************************
* 函 数 名 : Uart4_SendPackage
* 功能说明 : UART4数据包发送函数
* 形    参 : data：需要发送的数据，len：发送的数据的长度
* 返 回 值 : 无
* 备    注 : 数据包中间可能会包含‘\0’, 所以需要依赖长度进行发送
*********************************************************************************************************/ 
void Uart4_SendPackage(unsigned char *str, unsigned short len)
{
	while(len--)		
	{
		while(USART_GetFlagStatus(UART4,USART_FLAG_TC) == RESET);	//等待上一个字节发送完成
		USART_SendData(UART4,*str++);								//发送一个字节
	}
}
/*********************************************************************************************************
* 函 数 名 : UART4_IRQHandler
* 功能说明 : UART4中断服务函数
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 接收中断+空闲中断, 每接收到一个字节，就会执行一次
*********************************************************************************************************/ 
void UART4_IRQHandler(void)	//每接收到一个字节，就会执行一次
{
	if(USART_GetITStatus(UART4,USART_IT_RXNE) == SET)	//如果收到了数据（判断触发了什么中断）
	{
		Uart4.RxBuff[Uart4.RecLen] = USART_ReceiveData(UART4);		//读取数据
		if(++Uart4.RecLen > 255)									//如果接收到的数据超过缓冲区大小，那么从头开始存放
			Uart4.RecLen = 0;
	}
	else if(USART_GetITStatus(UART4,USART_IT_IDLE) == SET)	//如果触发了空闲中断，那么字符串接收完成
	{
		Uart4.RxBuff[Uart4.RecLen] = 0;								//在当前的末尾添加结束符			
		Uart4.RecFlag = true;										//标示位置1，通知主程序进行处理
		USART_ReceiveData(UART4);									//读取DR会对SR清零
	}
}
/*********************************************************************************************************
* 函 数 名 : Uart4_Init
* 功能说明 : 初始化UART4
* 形    参 : bound：波特率
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Uart4_Init(unsigned int bound)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);	//使能UART4外设时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);	//使能PC外设时钟

	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4);	//PC10复用USRT4功能
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4); //PC11复用UART4功能

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;	//配置复用模式后，控制权交由外设
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;				//复用模式
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;				//推挽输出
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;			//无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;			//50Mhz
	GPIO_Init(GPIOC, &GPIO_InitStruct);						//根据GPIO_InitStruct初始化
	
	USART_InitStruct.USART_BaudRate = bound;						//波特率
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//不使用硬件控制流
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//收发模式
	USART_InitStruct.USART_Parity = USART_Parity_No;				//不使用奇偶校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;				//一位停止位
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;		//八位的数据长度
	USART_Init(UART4, &USART_InitStruct);
	
	USART_ITConfig(UART4,USART_IT_RXNE,ENABLE);			//使能接收中断
	USART_ITConfig(UART4,USART_IT_IDLE,ENABLE);			//使能空闲中断

	NVIC_InitStruct.NVIC_IRQChannel = UART4_IRQn;			//中断源
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//使能中断源
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;			//响应优先级2
	NVIC_Init(&NVIC_InitStruct);	
	
	USART_Cmd(UART4,ENABLE);		//使能外设
}
