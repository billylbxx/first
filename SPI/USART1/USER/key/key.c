#include "key.h"
#include "systick.h"
/******************************************************
�� �� ����Key_Init
����˵����������ʼ��
��    �Σ���
�� �� ֵ����
��    ע��PA0��KEY1����PC13��KEY2���� PB1��KEY3��
*******************************************************/

void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_13;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
}

/******************************************************
�� �� ����Scanf_Key
����˵��������ɨ��
��    �Σ�mode:�㰴0 ������1
�� �� ֵ�����°�����Ӧ����ֵ���簴��led1 ����1
��    ע��PA0��KEY1_WK_UP����PB1��KEY2���� PC13��KEY3��
*******************************************************/

int Scanf_Key(int mode)
{
	static int flag=0;
	if(mode)flag=0;
	if((key1==1 || key2==0 || key3==0) && flag==0)
	{
		flag=1;
		Delay_Ms(100); 
		if(key1==1)return 1;
		else if(key2==0)return 2;
		else if(key3==0)return 3;
	}
	else if((key1==0 && key2==1 && key3==1) && flag==1) flag=0;
	return 0;
	
}





