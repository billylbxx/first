#include "systick.h"//�ο�Ȩ���ֲ�
static unsigned char fac_us=0;
static unsigned short fac_ms=0;
void Systick_Init(unsigned char clk)
{
	SysTick->CTRL &= ~(1<<2);	//ʹ���ⲿʱ����Ϊ�δ��ʱ��Դ
	fac_us=clk/8.0;
	fac_ms=clk/8.0*1000;
}

void Delay_Us(unsigned int us)
{
	unsigned int temp=0;
	SysTick->LOAD = us*fac_us;	//1us��21����
	SysTick->VAL  = 0;			//�����ǰ����ֵ
	SysTick->CTRL |= (1<<0);	//ʹ�ܼ�����
	do
	{
		temp=SysTick->CTRL;
	}	while(!(temp&(1<<16)&&(temp&(1<<0))));	//���CTRL�ĵ�16Ϊ0���ͼ����ȡ��ȴ�Ϊ1ʱ����
	SysTick->CTRL &= ~(1<<0);	//�رռ�����
}

void Delay_ms(unsigned int ms)
{
	unsigned int temp=0;
	SysTick->LOAD = ms*fac_ms;	//1us��21����
	SysTick->VAL  = 0;			//�����ǰ����ֵ
	SysTick->CTRL |= (1<<0);	//ʹ�ܼ�����
	do
	{
		temp=SysTick->CTRL;
	}	while(!(temp&(1<<16)&&(temp&(1<<0))));	//���CTRL�ĵ�16Ϊ0���ͼ����ȡ��ȴ�Ϊ1ʱ����
	SysTick->CTRL &= ~(1<<0);	//�رռ�����
}

