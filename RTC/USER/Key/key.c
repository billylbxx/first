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
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
		
		
		
		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_13;
			GPIO_Init(GPIOC,&GPIO_InitStruct);
		
		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_1;
			GPIO_Init(GPIOB,&GPIO_InitStruct);
}
/*********************************************************************************************************
* 函 数 名 : Scanf_Key
* 功能说明 : 按键扫描
* 形    参 : 无
* 返 回 值 : 所有按键的状态
* 备    注 : PA0（SWITCH_KEY）
*********************************************************************************************************/ 
char Scanf_Key(void)
{
	static unsigned char state = KEY_FREE;
	static unsigned char cnt = 0;
	static unsigned char key = 0;
	
	switch(state)
	{
		case KEY_FREE:	//检测是否有按键按下，有则进入消抖状态
			if(KEY1 == true || KEY2 == false || KEY3 == false)		
				state = KEY_SHAKE;
			break;
		case KEY_SHAKE:
			if(++cnt > 5)				//消抖50ms，如果50ms后按键还是按下状态，认为真的按下，否则认为抖动
			{
				cnt = 0;
				if(KEY1 == true || KEY2 == false || KEY3 == false)		
				{
					key = ((!!KEY1)<<0) | ((!KEY2)<<1) | ((!KEY3)<<2);	//记录按键当前值
					state = KEY_PRESS;	//进入下一个状态
				}
				else 	state = KEY_FREE;		//回到检测状态
			}
			break;
		case KEY_PRESS:	//如果按下的时间超过1s，那么认为是长按，返回结果，并设置下一个状态为等待松手态
			if(KEY1 == true || KEY2 == false || KEY3 == false)			
			{
				if(++cnt > 100)
				{
					state = KEY_LOOSEN;
					return (key|KEY_LONG_PRESS);	//最高位标记长按短按
				}
			}
			else					
			{
				state = KEY_LOOSEN;	//设置下一个状态为等待松手态
				return (key|KEY_SHORT_PRESS);		//如果在1s的时间内松手，那么认为短按并返回结果，
			}
			break;
		case KEY_LOOSEN:	//所有按键松开，则认为本次按下结束，回到最初的状态
			if(KEY1 == false && KEY2 == true && KEY3 == true)	
			{
				state = KEY_FREE;
				cnt = 0;
				key = 0;
			}
			break;
	}
	return 0;
}
