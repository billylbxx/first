#include <stdbool.h>
#include "key.h"
#include "led.h"
#include "systick.h"
#include "app_task.h"

/*********************************************************************************************************
* 函 数 名 : TaskKeyMsg_Handle
* 功能说明 : 按键处理任务
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : PA0（KEY1）、PB1（KEY2）、PC13（KEY3）
*********************************************************************************************************/ 
void TaskKeyMsg_Handle(void)
{
	static unsigned char key = 0;
	static unsigned char n = 0;
	
	//key = Scanf_Key();
	switch(key)
	{
		case KEY1_SHORT:
		{
			float temperature = 0.0;
			BaseType_t xHigherPriorityTaskWoken;
			
			Mlx90614_ReadTemperature(&temperature);
			xQueueSendFromISR(MessageQueue_Tem, &temperature, &xHigherPriorityTaskWoken);	//把结果发送到消息队列
		}	break;
		
		case KEY2_SHORT:
			n = 0;
			break;
		
		case KEY3_SHORT:
			Infrared_SendCmd(IR_IN_ENTER, n);	//进入内部学习模式
			if(++n >= 6)
				n = 0;
			break;
		
		case KEY1_LONG:
			Infrared_SendCmd(IR_EX_ENTRE,  0);//进入外部学习模式
			break;
		
		case KEY2_LONG:
			printf("进入配网模式\r\n");
			Set_Esp12ConnectionStatus(THREE);	//进入配网模#式
			RevampState(NULL, AT_CWMODE2, AT_CWMODE2, AT_CWMODE2, AT_OK, WAIT_1000, WAIT_100);
			break;
		
		case KEY3_LONG:
			Infrared_SendCmd(IR_FORMAT, 0);	//格式化红外模块
			break;
	}
}

/*********************************************************************************************************
* 函 数 名 : Scanf_Key
* 功能说明 : 按键扫描
* 形    参 : 无
* 返 回 值 : 所有按键的状态
* 备    注 : PA0（SWITCH_KEY）
*********************************************************************************************************/ 
#if 0
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
#endif
#if 1
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
		Delay_Ms(10);						//等待抖动消失
		if(KEY1 == 1)		return 1;		//如果按键还是按下的状态，那么是真的按下了
		else if(KEY2 == 0)	return 2;		//返回相应的值
		else if(KEY3 == 0)	return 3;	
	}
	else if((KEY1 == 0 && KEY2 == 1 && KEY3 == 1) && flag == 1)	//如果所有按键都松开了，并且被标记过按下 
		flag = 0;												//标记松开，允许下一次检测
	return 0;
}
#endif
/*********************************************************************************************************
* 函 数 名 : Key_Init
* 功能说明 : KEY端口初始化
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : PA0（KEY1）、PB1（KEY2）、PC13（KEY3）
*********************************************************************************************************/ 
void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	//使能PA外设时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	//使能PB外设时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	//使能PC外设时钟
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0;					//初始化引脚
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;				//输入模式
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;			//无上下拉
	GPIO_Init(GPIOA, &GPIO_InitStruct);						//根据GPIO_InitStruct初始化
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_1;					//初始化引脚
	GPIO_Init(GPIOB, &GPIO_InitStruct);						//根据GPIO_InitStruct初始化
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_13;				//初始化引脚
	GPIO_Init(GPIOC, &GPIO_InitStruct);						//根据GPIO_InitStruct初始化	
}
