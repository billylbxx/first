#include "systick.h"//参考权威手册
static unsigned char fac_us=0;
static unsigned short fac_ms=0;
void Systick_Init(unsigned char clk)
{
	SysTick->CTRL &= ~(1<<2);	//使用外部时钟作为滴答的时钟源
	fac_us=clk/8.0;
	fac_ms=clk/8.0*1000;
}

void Delay_Us(unsigned int us)
{
	unsigned int temp=0;
	SysTick->LOAD = us*fac_us;	//1us计21个数
	SysTick->VAL  = 0;			//清除当前计数值
	SysTick->CTRL |= (1<<0);	//使能计数器
	do
	{
		temp=SysTick->CTRL;
	}	while(!(temp&(1<<16)&&(temp&(1<<0))));	//如果CTRL的第16为0，就继续等。等待为1时结束
	SysTick->CTRL &= ~(1<<0);	//关闭计数器
}

void Delay_ms(unsigned int ms)
{
	unsigned int temp=0;
	SysTick->LOAD = ms*fac_ms;	//1us计21个数
	SysTick->VAL  = 0;			//清除当前计数值
	SysTick->CTRL |= (1<<0);	//使能计数器
	do
	{
		temp=SysTick->CTRL;
	}	while(!(temp&(1<<16)&&(temp&(1<<0))));	//如果CTRL的第16为0，就继续等。等待为1时结束
	SysTick->CTRL &= ~(1<<0);	//关闭计数器
}

