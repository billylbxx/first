#include "rgb.h"
#include "systick.h"
static void RGB_Reset(void);
static void W2812_WriteByte(unsigned char Byte);
/*********************************************************************************************************
* 函 数 名 : Rgb_Init
* 功能说明 : RGB灯珠端口初始化
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : PB15
*********************************************************************************************************/ 
void Rgb_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	//使能PB外设时钟
	
	GPIO_InitStruct.GPIO_Pin 	= GPIO_Pin_14;			//初始化引脚
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_OUT;		//输出模式
	GPIO_InitStruct.GPIO_OType 	= GPIO_OType_PP;		//推挽输出
	GPIO_InitStruct.GPIO_PuPd 	= GPIO_PuPd_UP;			//无上下拉
	GPIO_InitStruct.GPIO_Speed 	= GPIO_Speed_2MHz;		//2MHZ
	GPIO_Init(GPIOB, &GPIO_InitStruct);					//根据GPIO_InitStruct初始化
	
	GPIO_SetBits(GPIOB, GPIO_Pin_14);		
	
	RGB_Reset();										//复位
	W2812_WriteColorData(RGB_BLACK);					//RGB1默认熄灭
	W2812_WriteColorData(RGB_BLACK);					//RGB2默认熄灭
}
/*********************************************************************************************************
* 函 数 名 : W2812_WriteColorData
* 功能说明 : 写入颜色值
* 形    参 : color：需要显示的颜色
* 返 回 值 : 无
* 备    注 : 通过RGB配比，可以显示出不同的颜色。IO模拟时序不是很准确，而且效率极低，项目中不建议使用此方法
*********************************************************************************************************/ 
void W2812_WriteColorData(unsigned int color)
{
	W2812_WriteByte((color>>8)&0xff);		//绿
	W2812_WriteByte((color>>16)&0xff);		//红
	W2812_WriteByte((color>>0)&0xff);		//蓝
}
/*********************************************************************************************************
* 函 数 名 : W2812_WriteByte
* 功能说明 : 向RGB灯珠写入一个字节的数据
* 形    参 : Byte：要写入的数值
* 返 回 值 : 无
* 备    注 : 	Data transefer time:
				T0H 		0 code ,high voltage time 		0.4us ±150ns
				T1H 		1 code ,high voltage time 		0.8us ±150ns
				T0L 		0 code , low voltage time 		0.85us ±150ns
				T1L 		1 code ,low voltage time 		0.45us ±150ns
				RES 		low voltage time 				Above 50us
*********************************************************************************************************/ 
static void W2812_WriteByte(unsigned char Byte)
{
	unsigned char i = 0, j = 0;
	for(i=0; i<8; i++)
	{
		if(Byte&0x80)		
		{
			RGB_DATA = 1;	
			Delay_Us(1);			//‘1’码高电平保持0.8ns +-150ns
			RGB_DATA = 0;		
			for(j=0; j<10; j++);	//‘1’码低电平保持0.4ns +-150ns（此处循环粗略延迟）
		}
		else 				
		{
			RGB_DATA = 1;	
			for(j=0; j<10; j++);	//‘0’码高电平保持0.45ns +-150ns
			RGB_DATA = 0;	
			Delay_Us(1);			//‘0’码低电平保持0.85ns +-150ns（此处循环粗略延迟）
		}
		Byte <<= 1;
	}	
}
/*********************************************************************************************************
* 函 数 名 : RGB_Reset
* 功能说明 : 
* 形    参 : 
* 返 回 值 : 
* 备    注 : 
*********************************************************************************************************/ 
static void RGB_Reset(void)
{
	RGB_DATA = 0;
	Delay_Us(50);
}
