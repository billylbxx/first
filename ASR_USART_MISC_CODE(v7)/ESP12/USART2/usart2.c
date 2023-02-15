#include "usart2.h"
#include "usart1.h"
#include "esp12.h"
#include <stdbool.h>
USART2_InitStructure Usart2;
#define WIFI_USART USART2
/*********************************************************************************************************
* 函 数 名 : Usart2_SendString
* 功能说明 : USART2发送字符串函数
* 形    参 : str：需要发送的字符串
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Usart2_SendString(unsigned char *data)		//发送数据，末尾才有‘\0’
{
	while(*data != '\0')
	{
		while(!USART_GetFlagStatus(WIFI_USART,USART_FLAG_TC));
		USART_SendData(WIFI_USART,*data++);
	}
}
/*********************************************************************************************************
* 函 数 名 : Usart2_SendPackage
* 功能说明 : USART2数据包发送函数
* 形    参 : data：需要发送的数据，len：发送的数据的长度
* 返 回 值 : 无
* 备    注 : 数据包中间可能会包含‘\0’, 所以需要依赖长度进行发送
*********************************************************************************************************/ 
void Usart2_SendPackage(unsigned char *data,unsigned short len)	//发送数据包，中间包含‘\0’,所以需要依赖长度进行发送
{
	while(len--)
	{
		while(!USART_GetFlagStatus(WIFI_USART,USART_FLAG_TC));
		USART_SendData(WIFI_USART,*data++);
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
	if (USART_GetITStatus(USART2, USART_IT_RXNE))							//如果是接收中断
	{
		Usart2.RxBuff[Usart2.RecLen] = USART_ReceiveData(WIFI_USART);		//把收到的数据存起来
		if (++Usart2.RecLen > 255)
			Usart2.RecLen = 0;
	}
	else if (USART_GetITStatus(WIFI_USART, USART_IT_IDLE))	//如果是空闲中断
	{
		Usart2.RxBuff[Usart2.RecLen] = '\0';		//添加空字符
		Usart2.RecFlag = true;						//最高位置1
		USART_ReceiveData(WIFI_USART);				//读取DR，相当于清空SR的RXNE和IDLE位
		if(ConnectFlag == false)					//未连接时串口回显
		{
			//Usart1_SendPackage(Usart2.RxBuff,Usart2.RecLen);
			Usart2.RecLen = 0;
			Usart2.RecFlag = true;	
		}
	}
}
/*********************************************************************************************************
* 函 数 名 : Usart2_Init
* 功能说明 : 初始化USART2
* 形    参 : bound：波特率
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Usart2_Init(unsigned int bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);			//使能USART2时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);			//使能GPIOA时钟

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);		//PA2复用映射USART2
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);	    //PA3复用映射USART2

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
	USART_Init(WIFI_USART, &USART_InitStructure);

	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;			//USART1中断
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//使能中断
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;			//响应优先级
	NVIC_Init(&NVIC_InitStruct);

	USART_ITConfig(WIFI_USART, USART_IT_RXNE, ENABLE);			//使能接收中断
	USART_ITConfig(WIFI_USART, USART_IT_IDLE, ENABLE);			//使能空闲中断

	USART_Cmd(WIFI_USART, ENABLE);								//使能USART1
}

