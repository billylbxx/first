#include "usart1.h"
USART1_TypeDef Usart1;
/*********************************************************************************************************
* 函 数 名 : Usart1_SendString
* 功能说明 : USART1发送字符串函数
* 形    参 : str：需要发送的字符串
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Usart1_SendString(unsigned char *str)
{
	while(*str != 0)		
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	//等待上一个字节发送完成
		USART_SendData(USART1, *str++);								//发送一个字节
	}
}
/*********************************************************************************************************
* 函 数 名 : Usart1_SendPackage
* 功能说明 : USART1数据包发送函数
* 形    参 : data：需要发送的数据，len：发送的数据的长度
* 返 回 值 : 无
* 备    注 : 数据包中间可能会包含‘\0’, 所以需要依赖长度进行发送
*********************************************************************************************************/ 
void Usart1_SendPackage(unsigned char *data, unsigned short len)	
{
	while(len--)
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	//等待上一个字节发送完成
		USART_SendData(USART1, *data++);							//发送一个字节
	}
}
/*********************************************************************************************************
* 函 数 名 : USART1_IRQHandler
* 功能说明 : USART1中断服务函数
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 接收中断+空闲中断, 每接收到一个字节，就会执行一次
*********************************************************************************************************/ 
void USART1_IRQHandler(void)									
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)				//如果收到了数据（判断触发了什么中断）
	{
		Usart1.RxBuff[Usart1.RecLen] = USART_ReceiveData(USART1);	//读取数据
		if(++Usart1.RecLen > 255)									//如果接收到的数据超过缓冲区大小，那么从头开始存放
			Usart1.RecLen = 0;
	}
	else if(USART_GetITStatus(USART1, USART_IT_IDLE) == SET)		//如果触发了空闲中断，那么字符串接收完成
	{
		Usart1.RxBuff[Usart1.RecLen] = 0;							//在当前的末尾添加结束符			
		Usart1.RecFlag = true;										//标示位置1，通知主程序进行处理
		USART_ReceiveData(USART1);									//读取DR会对SR清零
	}
}
/*********************************************************************************************************
* 函 数 名 : Usart1_Init
* 功能说明 : 初始化USART1
* 形    参 : bound：波特率
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Usart1_Init(unsigned int bound)
{
	GPIO_InitTypeDef	GPIO_InitStruct		= {0};
	USART_InitTypeDef 	USART_InitStruct	= {0};
	NVIC_InitTypeDef 	NVIC_InitStruct		= {0};
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);		//使能USART1外设时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,  ENABLE);		//使能PA外设时钟

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,  GPIO_AF_USART1);	//PA9复用USART1功能
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10, GPIO_AF_USART1);	//PA10复用USART1功能

	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;		//配置复用模式后，控制权交由外设
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
	USART_Init(USART1, &USART_InitStruct);
	
	USART_ITConfig(USART1,USART_IT_RXNE, ENABLE);			//使能接收中断
	USART_ITConfig(USART1,USART_IT_IDLE, ENABLE);			//使能空闲中断

	NVIC_InitStruct.NVIC_IRQChannel    = USART1_IRQn;		//中断源
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//使能中断源
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 2;	//响应优先级2
	NVIC_Init(&NVIC_InitStruct);	
	
	USART_Cmd(USART1, ENABLE);								//使能外设
}

/*****加入以下代码,支持printf函数,而不需要选择use MicroLIB*****/	  
#if 1
	#pragma import(__use_no_semihosting)             
	//标准库需要的支持函数                 
	struct __FILE 
	{ 
		int handle; 
	}; 

	FILE __stdout;       
	//定义_sys_exit()以避免使用半主机模式    
	void _sys_exit(int x) 
	{ 
		x = x; 
	} 
	void _ttywrch(int ch)
	{
		ch = ch;
	}
	//重定义fputc函数 
	int fputc(int ch, FILE *f)
	{ 	
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		USART_SendData(USART1, ch);	
		return ch;
	}
/**********************printf support end**********************/	  
#endif
