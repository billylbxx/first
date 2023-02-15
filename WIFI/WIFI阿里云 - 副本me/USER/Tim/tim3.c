#include "tim3.h"//PWM
#include "stm32f4xx.h"
#include "led.h"
#include "systick.h"

//PA1/ADC_IN1/TIM2_CH2

void tim2_Init(int ARR,int PSC)
{
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM2);
	GPIO_InitTypeDef GPIO_InitStruct = {0};
		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_1;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOA,&GPIO_InitStruct);
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
		TIM_TimeBaseInitStruct.TIM_Period = ARR-1;//自动重载递增计数器自动重装载寄存器周期的值
		TIM_TimeBaseInitStruct.TIM_Prescaler = PSC-1;  //预分频器
		TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//时钟分割
		TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;//设置了周期计数器值0不翻倍
		TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);

	TIM_OCInitTypeDef TIM_OCInitStruct;
	//TIM_OCInitStruct.TIM_OCIdleState=TIM3;
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;//PWM1模式
	//TIM_OCInitStruct.TIM_OCNIdleState
	//TIM_OCInitStruct.TIM_OCNPolarity
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;//有效电平1
	//TIM_OCInitStruct.TIM_OutputNState
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//使能输出
	TIM_OCInitStruct.TIM_Pulse=(ARR-1)/2;//比较值
	TIM_OC2Init(TIM2,&TIM_OCInitStruct);


	//使能CCR
	TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);
	//开定时器
	TIM_Cmd(TIM2,ENABLE);	
	TIM_SetCompare2(TIM2,499);
	delay_ms(5);
}

void tim3_Init(int ARR,int PSC)
{
		//使能时钟PB0
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		
		//IO口复用
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_TIM3);
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM3);
		
		//复用推挽输出
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOB,&GPIO_InitStruct);
		
		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_8;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOC,&GPIO_InitStruct);
			
		//定时器时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
		TIM_TimeBaseInitStruct.TIM_Period = ARR-1;//自动重载递增计数器自动重装载寄存器周期的值
		TIM_TimeBaseInitStruct.TIM_Prescaler = PSC-1;  //预分频器
		TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//时钟分割
		TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;//设置了周期计数器值0不翻倍
		TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
		
				
		//使能影子寄存器，可以不写
		TIM_ARRPreloadConfig(TIM3,ENABLE);
		TIM_PrescalerConfig(TIM3,PSC-1, ENABLE);
		
	//UG位软件触发更新
		TIM_UpdateDisableConfig(TIM3, DISABLE);	
		
	//UG位置1，产生一次软件更新，目的ARR值装入影子
		TIM_GenerateEvent(TIM3,TIM_EventSource_Update);
		
	//循环计数
	TIM_UpdateRequestConfig(TIM3,TIM_UpdateSource_Global);

	TIM_OCInitTypeDef TIM_OCInitStruct;
	//TIM_OCInitStruct.TIM_OCIdleState=TIM3;
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;//PWM1模式
	//TIM_OCInitStruct.TIM_OCNIdleState
	//TIM_OCInitStruct.TIM_OCNPolarity
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;//有效电平1,同相
	//TIM_OCInitStruct.TIM_OutputNState
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//使能输出
	TIM_OCInitStruct.TIM_Pulse=(ARR-1)/2;//比较值（占空比50%）
	TIM_OC3Init(TIM3,&TIM_OCInitStruct);


	//使能CCR
	TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);

	//开定时器
	TIM_Cmd(TIM3,ENABLE);
	
}

//void LED3_PWM(int CRR)
//{
//	TIM_SetCompare3(TIM3,CRR);
//}
void MOTOR_PWM(int CRR1)
{
	TIM_SetCompare3(TIM3,CRR1);
}


	

