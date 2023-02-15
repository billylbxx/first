#include "tim2.h"

//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft s.

void tim2_Init(int ARR,int PSC)
{
  //使能CCR
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
  
	//定义变量 
	
  GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//打开GPIO时钟

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM2); //使能定时器2复用到GPIOA上
 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//打开定时器2时钟
	//GOIO初始化
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;//复用输出模式
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;//推挽输出
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1;//TIM2_CH2
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;//无上下拉
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;//速度50M
	GPIO_Init(GPIOA, &GPIO_InitStruct);//GPIO初始化
	
	//定时器初始化
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;//设置时钟分割
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;//定时器向上计数
	TIM_TimeBaseInitStruct.TIM_Period=ARR;//自动重装载值
	TIM_TimeBaseInitStruct.TIM_Prescaler=PSC;//时钟预分频数 
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);//定时器初始化

	//初始化TIM2 Channel2 PWM模式
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;//选着模式1，TIM脉冲宽度调制模式2
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//比较输出使能
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;//输出极性：TIM输出比较极性高
	TIM_OCInitStruct.TIM_Pulse=5;//设置待装入捕获比较寄存器的脉冲值
	TIM_OC2Init(TIM2,&TIM_OCInitStruct);////根据TIM_OCInitStruct中指定的参数初始化外设TIM2
	
	TIM_Cmd(TIM2,	ENABLE);//使能定时器2


}

//void PWM1(int CCR)
//{
//	TIM_SetCompare2(TIM2, CCR);

//}



/*********************************************************************************************************
* 函 数 名 : SetServoAngle
* 功能说明 : 设置舵机的角度，0~180
* 形    参 : angle：角度值
* 返 回 值 : 无
* 备    注 : 只需要调用一次，舵机即可保持角度；0.5ms--0°  2.5ms--180°

0.5ms------------0 度；     
1.0ms------------45 度；
1.5ms------------90 度；   
2.0ms-----------135 度；
2.5ms-----------180 度；

*********************************************************************************************************/ 
void Servo_SetAngle(unsigned char angle)
{
	unsigned short pulse = 0;
	//针对舵机可转角度限辐
	if(angle < 5)		angle = 5;
	if(angle > 175)	angle = 5;
	//将角度值转换为脉冲值 
	pulse = (unsigned short)(50 + angle * 100 / 90.0);  //此转换公式需根据pwm的arr及psc配置来做相应变化
	//TIM2->CCR2 = pulse;
	TIM_SetCompare2(TIM2, pulse);
}

