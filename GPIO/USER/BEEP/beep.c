#include "beep.h"

void Beep_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_12;//选择管脚 
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//输出模式
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;//输出频率
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_ResetBits(GPIOC, GPIO_Pin_12);
}
