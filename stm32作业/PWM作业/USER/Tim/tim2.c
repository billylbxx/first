#include "tim2.h"
#include "stm32f4xx.h"
#include "led.h"
#include "lcd.h"
#include "systick.h"
//PA1/ADC_IN1/TIM2_CH2
//TIM2_CH1_ETR
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
	
		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_15;
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
		TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;//PWM1模式
		TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;//有效电平1
		TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//使能输出
		TIM_OCInitStruct.TIM_Pulse=(ARR-1)/2;//比较值
		TIM_OC2Init(TIM2,&TIM_OCInitStruct);

		TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;//PWM1模式
		TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;//有效电平1
		TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//使能输出
		TIM_OCInitStruct.TIM_Pulse=(ARR-1)/2;//比较值
		TIM_OC1Init(TIM2,&TIM_OCInitStruct);

		//使能CCR
		TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);
		TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);
		//开定时器
		TIM_Cmd(TIM2,ENABLE);	
}

/*********************************************************************************************************
* 函 数 名 : SetServoAngle
* 功能说明 : 设置舵机的角度，0~180
* 形    参 : angle：角度值
* 返 回 值 : 无
* 备    注 : 只需要调用一次，舵机即可保持角度；0.5ms--0°  2.5ms--180°
*********************************************************************************************************/ 
void Servo_SetAngle(unsigned char angle)
{
	unsigned short pulse = 0;
	//针对舵机可转角度限辐
	if(angle <= 5)		angle = 5;
	if(angle >= 175)	angle = 175;
	//将角度值转换为脉冲值 
	pulse = (unsigned short)(50 + angle * 100 / 90.0);  //此转换公式需根据pwm的arr及psc配置来做相应变化
	//TIM2->CCR2 = pulse;
	//TIM_SetCompare2(TIM2, pulse);
}

void lcd_ld(unsigned char CCR)
{
	TIM_SetCompare1(TIM2, CCR);
}
