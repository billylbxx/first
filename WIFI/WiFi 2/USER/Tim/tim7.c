#include "tim7.h"
#include "stm32f4xx.h"
#include "led.h"
void tim7_Init(int ARR,int PSC)
{
//使能时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
//ARR，PSC,向上计数，倍频，周期加倍（高级定时器）
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_Period = ARR-1;//自动重载递增计数器自动重装载寄存器周期的值
  TIM_TimeBaseInitStruct.TIM_Prescaler = PSC-1;  //预分频器
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//时钟分割
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;//设置了周期计数器值0不翻倍
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStruct);
	
//使能影子寄存器，可以不写
	TIM_ARRPreloadConfig(TIM7,ENABLE);
	TIM_PrescalerConfig(TIM7,PSC-1, ENABLE);
	
//UG位软件触发更新
	TIM_UpdateDisableConfig(TIM7, DISABLE);	
	
//UG位置1，产生一次软件更新，目的ARR值装入影子
	TIM_GenerateEvent(TIM7,TIM_EventSource_Update);
	
//循环计数
	TIM_UpdateRequestConfig(TIM7,TIM_UpdateSource_Global);


//使能定器中断
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
	
	//配置NVIC
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=TIM7_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_InitStruct);
	
	//开定时器
	TIM_Cmd(TIM7,ENABLE);
}
//中断服务函数

void TIM7_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)==1)
	{
		static int i;
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);//清中断标志位
		i++;
		if(i>=20)//因为ARR不能超过65536，所以这样写
		{
			i=0;
		}
	}
}
	
