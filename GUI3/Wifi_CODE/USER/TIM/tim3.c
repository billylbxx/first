#include "tim3.h"
#include "led.h"

void tim3_Init(int ARR,int PSC)
{
	//使能时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	//IO口复用
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM3);
	//复用推挽输出
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;//推挽
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;//管脚
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;//浮空
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct); 
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8;//管脚
	GPIO_Init(GPIOC,&GPIO_InitStruct); 
	
	//定时器时钟
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_Prescaler         =ARR-1;  //重零开始  
	TIM_TimeBaseInitStruct.TIM_Period            =PSC-1;   //公式  //预分频
	TIM_TimeBaseInitStruct.TIM_ClockDivision     =TIM_CKD_DIV1; //无倍频
	TIM_TimeBaseInitStruct.TIM_CounterMode       =TIM_CounterMode_Up;   //计数器模式
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter =0;  //参数为0，时间周期不翻倍（高级定时器使用）
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
	
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;   //PWM1模式
//	TIM_OCInitStruct.TIM_OCIdleState      =
//	TIM_OCInitStruct.TIM_OCNIdleState=    //高级定时器
//	TIM_OCInitStruct.TIM_OCNPolarity=
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCNPolarity_Low;  //有效电平0 
//	TIM_OCInitStruct.TIM_OutputNState=
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable ;   //使能输出
	TIM_OCInitStruct.TIM_Pulse     =  (ARR-1)/2;   //比较值
	TIM_OC3Init(TIM3, &TIM_OCInitStruct);
	
	//开定时器
	TIM_Cmd(TIM3, ENABLE);

  //使能CCR
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
 

}

//void LED3_PWM(int CCR)
//{
//	TIM_SetCompare3(TIM3,CCR);  //捕获计数值
//}


void FAN_PWM(int CCR)
{
	TIM_SetCompare3(TIM3,CCR);  //捕获计数值
}
