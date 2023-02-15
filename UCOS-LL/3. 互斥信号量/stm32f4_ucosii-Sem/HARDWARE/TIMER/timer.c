#include "timer.h"
#include "led.h"
#include "GUI.h"

extern __IO int32_t OS_TimeMS;

//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3时钟使能    
 	TIM3->ARR=arr;  	//设定计数器自动重装值 
	TIM3->PSC=psc;  	//预分频器	  
	TIM3->DIER|=1<<0;   //允许更新中断	  
	TIM3->CR1|=0x01;    //使能定时器3
  MY_NVIC_Init(1,3,TIM3_IRQn,2);	//抢占1，子优先级3，组2		
	
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
	if(TIM3->SR&0X0001)//溢出中断  
	{
		OS_TimeMS++;
	}
	TIM3->SR&=~(1<<0);//清除中断标志位 	 
}

//通用定时器4中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM4_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<2;	//TIM4时钟使能    
 	TIM4->ARR=arr;  	//设定计数器自动重装值 
	TIM4->PSC=psc;  	//预分频器	  
	TIM4->DIER|=1<<0;   //允许更新中断	  
	TIM4->CR1|=0x01;    //使能定时器3
  MY_NVIC_Init(3,3,TIM3_IRQn,2);	//抢占2，子优先级3，组2	
	
}

//定时器4中断服务函数
void TIM4_IRQHandler(void)
{
	if(TIM4->SR&0X0001)//溢出中断 
	{
		GUI_TOUCH_Exec();
	}
	TIM4->SR&=~(1<<0);//清除中断标志位 
}
