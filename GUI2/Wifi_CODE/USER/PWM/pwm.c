#include "pwm.h"
/*********************************************************************************************************
* 函 数 名 : Led2Pwm_Init
* 功能说明 : 初始化TIM3：CH3，输出PWM驱动LED2
* 形    参 : arr：重装载，psc：预分频
* 返 回 值 : 无
* 备    注 : Fpwm（HZ） = Fclk / （（arr+1）*（psc+1））
			 Duty_ratio（%） = CRR1 / ARR
*********************************************************************************************************/ 
void Led2Pwm_Init(unsigned short arr,unsigned short psc)
{
	GPIO_InitTypeDef 		GPIO_InitStruct			= {0};
	TIM_OCInitTypeDef 		TIM_OCInitStruct		= {0};
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct	= {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	//使能PB外设时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,  ENABLE);	//开启外设时钟
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);	//PB0复用TIM3功能
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0;		//初始化引脚
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;		//复用模式
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;		//推挽输出
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	//50MHZ
	GPIO_Init(GPIOB, &GPIO_InitStruct);				//根据GPIO_InitStruct初始化
	
	TIM_TimeBaseInitStruct.TIM_Period        = arr-1;				//重装载
	TIM_TimeBaseInitStruct.TIM_Prescaler     = psc-1;				//预分频
	TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;		//不分频
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	
	TIM_OCInitStruct.TIM_OCMode      = TIM_OCMode_PWM1;			//PWM模式1，TIMx_CNT<TIMx_CCR1时为有效电平
	TIM_OCInitStruct.TIM_OCPolarity  = TIM_OCPolarity_Low;		//输出极性为低
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	//比较输出使能
	TIM_OCInitStruct.TIM_Pulse 		 = 0;						//设置比较寄存器的初值
	TIM_OC3Init(TIM3, &TIM_OCInitStruct);
	
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);			//使能CCR3的预装载
	TIM_Cmd(TIM3, ENABLE);										//使能TIM3外设
}	
/*********************************************************************************************************
* 函 数 名 : HollowPwm_Init
* 功能说明 : 初始化TIM8：CH3，输出PWM驱动空心杯
* 形    参 : arr：重装载，psc：预分频
* 返 回 值 : 无
* 备    注 : Fpwm（HZ） = Fclk / （（arr+1）*（psc+1））
			 Duty_ratio（%） = CRR1 / ARR
			 用此定时器控制空心杯时，必须使能主输出（TIM1 or TIM8）
*********************************************************************************************************/ 
void HollowPwm_Init(unsigned short arr,unsigned short psc)
{
	GPIO_InitTypeDef 		GPIO_InitStruct			= {0};
	TIM_OCInitTypeDef 		TIM_OCInitStruct		= {0};
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct	= {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);		//使能PC外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,  ENABLE);		//开启TIM8外设时钟
	
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM8);		//PC8复用TIM8功能
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8;		//初始化引脚
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;		//复用模式
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;		//推挽输出
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	//50MHZ
	GPIO_Init(GPIOC, &GPIO_InitStruct);				//根据GPIO_InitStruct初始化
	
	TIM_TimeBaseInitStruct.TIM_Period 		 = arr-1;				//重装载
	TIM_TimeBaseInitStruct.TIM_Prescaler 	 = psc-1;				//预分频
	TIM_TimeBaseInitStruct.TIM_CounterMode 	 = TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;		//不分频
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseInitStruct);
	
	TIM_OCInitStruct.TIM_OCMode 	 = TIM_OCMode_PWM1;			//PWM模式2，TIMx_CNT<TIMx_CCR1时为有效电平
	TIM_OCInitStruct.TIM_OCPolarity  = TIM_OCPolarity_High;		//输出极性为高
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	//比较输出使能
	TIM_OCInitStruct.TIM_Pulse 		 = 0;						//设置比较寄存器的初值
	TIM_OC3Init(TIM8, &TIM_OCInitStruct);
       
	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);			//使能CCR3的预装载
	TIM_CtrlPWMOutputs(TIM8, ENABLE);							//使能主输出
	TIM_Cmd(TIM8, ENABLE);										//使能TIM8外设
}	
/*********************************************************************************************************
* 函 数 名 : SteeringPwm_Init
* 功能说明 : 初始化TIM2：CH2，输出PWM驱动舵机
* 形    参 : arr：重装载，psc：预分频
* 返 回 值 : 无
* 备    注 : Fpwm（HZ） = Fclk / （（arr+1）*（psc+1））
			 Duty_ratio（%） = CRR1 / ARR
			 84M / 840 = 0.1M 故记一个数的时间为10us  定时周期：（1999 + 1） * 10us = 20ms
*********************************************************************************************************/ 
void SteeringPwm_Init(unsigned short arr,unsigned short psc)
{
	GPIO_InitTypeDef 		GPIO_InitStruct			= {0};
	TIM_OCInitTypeDef 		TIM_OCInitStruct		= {0};
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct	= {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	//使能PA外设时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,  ENABLE);	//开启TIM2外设时钟
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);	//PA1复用TIM2功能
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_1;			//初始化引脚
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;		//复用模式
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;		//推挽输出
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	//50MHZ
	GPIO_Init(GPIOA, &GPIO_InitStruct);				//根据GPIO_InitStruct初始化
	
	TIM_TimeBaseInitStruct.TIM_Period 		 = arr-1;					//重装载
	TIM_TimeBaseInitStruct.TIM_Prescaler 	 = psc-1;					//预分频
	TIM_TimeBaseInitStruct.TIM_CounterMode 	 = TIM_CounterMode_Up;		//向上计数
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;			//不分频
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
	
	TIM_OCInitStruct.TIM_OCMode 	 = TIM_OCMode_PWM1;			//PWM模式1，TIMx_CNT<TIMx_CCR1时为有效电平
	TIM_OCInitStruct.TIM_OCPolarity  = TIM_OCPolarity_High;		//输出极性为低
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	//比较输出使能
	TIM_OCInitStruct.TIM_Pulse		 = 0;						//设置比较寄存器的初值
	TIM_OC2Init(TIM2, &TIM_OCInitStruct);
	
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);			//使能CCR2的预装载
	TIM_Cmd(TIM2, ENABLE);										//使能TIM2外设
}
/*********************************************************************************************************
* 函 数 名 : SetLedBrightness
* 功能说明 : 设置led的亮度
* 形    参 : value：亮度百分比，0~100
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void SetLedBrightness(char brightness)
{
	if(brightness >= 100) 		brightness = 100;
	else if(brightness <= 0)	brightness = 0;
	TIM3->CCR3 = brightness;
	//TIM_SetCompare3(TIM3, brightness);
}
/*********************************************************************************************************
* 函 数 名 : SetHollowSpeed
* 功能说明 : 设置空心杯的转速
* 形    参 : value：转速百分比，0~100
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void SetHollowSpeed(char speed)
{
	if(speed >= 100) 	speed = 100;
	else if(speed <= 0)	speed = 0;
	TIM8->CCR3 = speed;
	//TIM_SetCompare3(TIM8, speed);
}
/*********************************************************************************************************
* 函 数 名 : SetServoAngle
* 功能说明 : 设置舵机的角度，0~180
* 形    参 : angle：角度值
* 返 回 值 : 无
* 备    注 : 只需要调用一次，舵机即可保持角度；0.5ms--0°  2.5ms--180°
*********************************************************************************************************/ 
void SetServoAngle(unsigned char angle)
{
	unsigned short pulse = 0;
	//针对舵机可转角度限辐
	if(angle <= 5)		angle = 5;
	if(angle >= 175)	angle = 175;
	//将角度值转换为脉冲值  
	pulse = (unsigned short)(50 + angle * 100 / 90.0);  //此转换公式需根据pwm的arr及psc配置来做相应变化
	TIM2->CCR2 = pulse;
	//TIM_SetCompare2(TIM2, pulse);
}
 

