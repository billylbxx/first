#include "beep.h"
/*********************************************************************************************************
* 函 数 名 : Beep_Init
* 功能说明 : 初始化BEEP端口
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 蜂鸣器PC12（BEEP）
*********************************************************************************************************/ 
void Beep_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	//使能PC外设时钟
	
	GPIO_InitStruct.GPIO_Pin 	= GPIO_Pin_12;				//初始化引脚
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_OUT;			//输出模式
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;			//推挽输出
	GPIO_InitStruct.GPIO_PuPd 	= GPIO_PuPd_NOPULL;			//无上下拉
	GPIO_InitStruct.GPIO_Speed 	= GPIO_Speed_2MHz;			//2MHZ
	GPIO_Init(GPIOC, &GPIO_InitStruct);						//根据GPIO_InitStruct初始化
	
	GPIO_ResetBits(GPIOC, GPIO_Pin_12);						//输出低电平，蜂鸣器不响
}

