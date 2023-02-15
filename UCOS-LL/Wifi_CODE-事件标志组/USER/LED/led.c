#include "led.h"
/*********************************************************************************************************
* 函 数 名 : Led_Init
* 功能说明 : LED端口初始化
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : PC4（LED1）、PB0（LED2）、PC5（LED3）
*********************************************************************************************************/ 
void Led_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	//使能PC外设时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	//使能PC外设时钟
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_4 | GPIO_Pin_5;	//初始化引脚
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;				//输出模式
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;				//推挽输出
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;			//无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;			//2MHZ
	GPIO_Init(GPIOC, &GPIO_InitStruct);						//根据GPIO_InitStruct初始化
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStruct);						//根据GPIO_InitStruct初始化
	
	GPIO_SetBits(GPIOC, GPIO_Pin_4 | GPIO_Pin_5);			//输出高电平，灯灭
	GPIO_SetBits(GPIOB, GPIO_Pin_0);						//输出高电平，灯灭
}

