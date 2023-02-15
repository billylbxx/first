#include "gpio.h"

void Led_Init(void)
{
    /* GPIO Peripheral clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_InitTypeDef  GPIO_InitStructure;
    /* Configure output pushpull mode */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_4 | GPIO_Pin_5);
}


void Key_Init(void)
{
  /* GPIO Peripheral clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  GPIO_InitTypeDef  GPIO_InitStructure;
  /* Configure output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

u8 Key_Scan(u8 mode)
{
	static u8 key_sta = 0;
	if(mode) key_sta = 0 ;
	if((key_sta == 0)&&(GPIOA->IDR & 1<<0))
	{
		key_sta = 1;
		//delay_ms(20);
		if(GPIOA->IDR & 1<<0) return 1;
	}
	else if (!(GPIOA->IDR & 1<<0))
		key_sta = 0 ;
	return 0;
}
