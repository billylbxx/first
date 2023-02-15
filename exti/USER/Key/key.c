#include "stm32f4xx.h"
#include "key.h"
void key_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
		
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//根据原理图，KEY1接的是下拉电阻（默认情况下即按键没有按下时，PA0是低电平），但是是芯片外部的，所以此处使用无上下拉，
	GPIO_Init(GPIOA,&GPIO_InitStruct);
		
		
		
		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_13;//同理，PA13和PA1在按键没有按下时是高电平
			GPIO_Init(GPIOC,&GPIO_InitStruct);
		
		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_1;
			GPIO_Init(GPIOB,&GPIO_InitStruct);
}

void delay(int m)
{
	int i,j;
	for(i=0;i<10000;i++)
	{
		for(j=0;j<m;j++)
		{
			;
		}
	}
}

/*********************************************************************************************************
* 函 数 名 : Scanf_Key
* 功能说明 : 按键扫描
* 形    参 : mode：1连按，0单击
* 返 回 值 : 按下的按键对应的值，如KEY1返回1
* 备    注 : PA0（KEY1）、PB1（KEY2）、PC13（KEY3）
*********************************************************************************************************/ 
unsigned char Scanf_Key(unsigned char mode)
{
	static unsigned char flag = 0;						//默认没有标记按下
	if(mode)	flag = 0;									//mode=1时，允许连续触发，不检测松手
	if((KEY1 == 1 || KEY2 == 0 || KEY3 == 0) && flag == 0)	//当按键按下，并且没有被标记按
	{
		flag = 1;							//标记按下
		delay(100);						//等待抖动消失
		if(KEY1 == 1)		return 1;		//如果按键还是按下的状态，那么是真的按下了
		else if(KEY2 == 0)	return 2;		//返回相应的值
		else if(KEY3 == 0)	return 3;	
	}
	else if((KEY1 == 0 && KEY2 == 1 && KEY3 == 1) && flag == 1)	//如果所有按键都松开了，并且被标记过按下 
		flag = 0;												//标记松开，允许下一次检测
	return 0;
}
