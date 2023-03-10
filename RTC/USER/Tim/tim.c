#include "tim.h"
#include "stm32f4xx.h"
#include "led.h"
/*
F=84000000
f=F/84=1000000//F/PSC
T=1/f=0.000001s=0.001ms=1us
t=ARR*T
50000us*20=1000000us=1s/ARR*i
*/

void tim6_Init(int ARR,int PSC)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	
	/*配置定时器6模块内部寄存器*/
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_Period = ARR-1;//自动重载递增计数器自动重装载寄存器周期的值
  TIM_TimeBaseInitStruct.TIM_Prescaler = PSC-1;  //预分频器
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//时钟分割
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;//设置了周期计数器值0不翻倍
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStruct);
	
	//影子寄存器
	TIM_ARRPreloadConfig(TIM6,ENABLE);
	//循环计数
	TIM_UpdateRequestConfig(TIM6,TIM_UpdateSource_Global);
	
	//使能定时中断
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
	//配置NVIC
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=TIM6_DAC_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	//开定时器
	TIM_Cmd(TIM6,ENABLE);
}
//中断服务函数
void TIM6_DAC_IRQHandler(void)
{
	int sec;
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)==1)
	{
		static int i;
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);//清中断标志位
		i++;
		if(i>=20)//因为ARR不能超过65536，所以这样写
		{
			i=0;
			LED1=~LED1;
			sec++;
			printf("%d\r\n",sec);
		}
	}
}
	













