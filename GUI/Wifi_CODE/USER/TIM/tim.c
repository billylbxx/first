#include "tim.h"
#include "led.h"
#include "BEEP.h"
#include "key.h"
#include "SPI1.h"


void tim_Init(int ARR,int PSC)
{
	 //配置定时器6模块内部寄存器
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	//使能时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);

 
	
	TIM_TimeBaseInitStruct.TIM_Prescaler=ARR-1;  //重零开始  
	//预分频
	TIM_TimeBaseInitStruct.TIM_Period=PSC-1;   
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;   //计数器模式
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;  //参数为0，时间周期不翻倍（高级定时器使用）
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStruct);
	
	
	NVIC_InitStruct.NVIC_IRQChannel=TIM6_DAC_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_InitStruct);
	
	//影子寄存器ARR：开
	//预分频影子 RSC
	//TIM_ARRPreloadConfig(TIM6, ENABLE);	
	
	//UG置1，产生一次软件更新，目的ARR值装入影子
//	TIM_GenerateEvent(TIM7,TIM_EventSource_Update);
	//使能定时中断	
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
	//自动重装载置：ARR  确定计数次数
//	TIM_UpdateRequestConfig(TIM6, TIM_UpdateSource_Global);   //循环计数模式
	TIM_Cmd(TIM6,ENABLE);   //使能

}


unsigned int i,j,k;	



void TIM6_DAC_IRQHandler(void)
{
	//1）实时时钟
//2）清中断标志位
//	int i=1;
//	if( TIM_GetITStatus(TIM6, TIM_IT_Update)==1)   //中断源是否为1
//	{ 
//		
//		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);  
//	  if(i)
//		{
//			i++;
//			led1=!led1;
//			printf("%d\r\n",i);
//		}
//		
//	}

  int key_flag=1;
  if( TIM_GetITStatus(TIM6, TIM_IT_Update)==1)
	{
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
		k++;
		printf("闹钟: 8:56\r\n");
		if(k==60)
		{
			k=0;
			j++;
		
		}	
				
		if(j==60)
		{
			j=0;
			i++;
			beep=1;
		}
		else if(j==30 && k==0)
		{
			beep=1;
		}

		
		if(i==24)
		{
			k=0;
			j=0;
			i=0;
			
		}
		
		if(i==8 && j==56 && k==0)  //闹钟
		{
			beep=1;
		}
		
		if(k==5)
		{
			beep=0;
		}

//		if(key1 && key_flag)
//		{
//			delay_MS(1000);
//			key_flag=0;
//			j++;
//		}
//		
//		if(key2==0 && key_flag)
//		{
//			delay_MS(1000);
//			key_flag=0;
//			i++;
//		}
		

	}
  printf("%d:%d:%d\r\n",i,j,k);



}

void delay_MS(int m)
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


