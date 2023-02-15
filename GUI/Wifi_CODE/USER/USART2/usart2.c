#include "usart2.h"
USART2_TypeDef Usart2;
/*********************************************************************************************************
* 函 数 名 : USART3_SendString
* 功能说明 : USART3发送字符串函数
* 形    参 : str：需要发送的字符串
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Usart2_SendString(unsigned char *str)
{
	while(*str != 0)		
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	//等待上一个字节发送完成
		USART_SendData(USART2, *str++);								//发送一个字节
	}
}
/*********************************************************************************************************
* 函 数 名 : USART2_SendPackage
* 功能说明 : USART2数据包发送函数
* 形    参 : data：需要发送的数据，len：发送的数据的长度
* 返 回 值 : 无
* 备    注 : 数据包中间可能会包含‘\0’, 所以需要依赖长度进行发送
*********************************************************************************************************/ 
void USART2_SendPackage(unsigned char *data, unsigned short len)	
{
	while(len--)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	//等待上一个字节发送完成
		USART_SendData(USART2, *data++);							//发送一个字节
	}
}
/*********************************************************************************************************
* 函 数 名 : USART2_IRQHandler
* 功能说明 : USART2中断服务函数
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 接收中断+空闲中断, 每接收到一个字节，就会执行一次
*********************************************************************************************************/ 
void USART2_IRQHandler(void)									
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)				//如果收到了数据（判断触发了什么中断）
	{
		Usart2.RxBuff[Usart2.RecLen] = USART_ReceiveData(USART2);	//读取数据
		if(++Usart2.RecLen > 255)									//如果接收到的数据超过缓冲区大小，那么从头开始存放
			Usart2.RecLen = 0;
	}
	else if(USART_GetITStatus(USART2, USART_IT_IDLE) == SET)		//如果触发了空闲中断，那么字符串接收完成
	{
		//Usart2.RxBuff[Usart2.RecLen]= 0;			//在当前的末尾添加结束符			
		Usart2.RecFlag = 1;											//标示位置1，通知主程序进行处理
		USART_ReceiveData(USART2);							//读取DR会对SR清零
		USART2->SR;
	}
}
/*********************************************************************************************************
* 函 数 名 : USART2_Init
* 功能说明 : 初始化USART2
* 形    参 : bound：波特率
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Usart2_Init(unsigned int bound)
{
	GPIO_InitTypeDef 	GPIO_InitStruct		= {0};
	USART_InitTypeDef 	USART_InitStruct	= {0};
	NVIC_InitTypeDef 	NVIC_InitStruct		= {0};
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);		//使能USART3外设时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,  ENABLE);		//使能PA外设时钟

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2, GPIO_AF_USART2);	//PA9复用USART3功能
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3, GPIO_AF_USART2);	//PA10复用USART3功能

	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3;		//配置复用模式后，控制权交由外设
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;					//复用模式
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;					//推挽输出
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;				//无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;				//100Mhz
	GPIO_Init(GPIOA, &GPIO_InitStruct);							//根据GPIO_InitStruct初始化
	
	USART_InitStruct.USART_BaudRate 	= bound;									//波特率
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//不使用硬件控制流
	USART_InitStruct.USART_Mode 		= USART_Mode_Tx | USART_Mode_Rx;			//收发模式
	USART_InitStruct.USART_Parity 		= USART_Parity_No;							//不使用奇偶校验
	USART_InitStruct.USART_StopBits 	= USART_StopBits_1;							//一位停止位
	USART_InitStruct.USART_WordLength 	= USART_WordLength_8b;						//八位的数据长度
	USART_Init(USART2, &USART_InitStruct);
	
	USART_ITConfig(USART2,USART_IT_RXNE, ENABLE);			//使能接收中断
	USART_ITConfig(USART2,USART_IT_IDLE, ENABLE);			//使能空闲中断

	NVIC_InitStruct.NVIC_IRQChannel    = USART2_IRQn;		//中断源
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//使能中断源
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 2;	//响应优先级2
	NVIC_Init(&NVIC_InitStruct);	
	USART_Cmd(USART2, ENABLE);								//使能外设
}

