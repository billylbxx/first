#include "stm32f4xx.h"
#include "Beep.h"
void Beep_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_12;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_ResetBits(GPIOC,GPIO_Pin_12);
	
}
