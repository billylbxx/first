#include "beep.h"

void Beep_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_12;//ѡ��ܽ� 
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//���ģʽ
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;//���Ƶ��
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//��������
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_ResetBits(GPIOC, GPIO_Pin_12);
}
