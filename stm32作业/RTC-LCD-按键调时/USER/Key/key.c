#include "stm32f4xx.h"
#include "key.h"
#include "systick.h"
void key_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
		
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
		
		
		
		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_13;
			GPIO_Init(GPIOC,&GPIO_InitStruct);
		
		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_1;
			GPIO_Init(GPIOB,&GPIO_InitStruct);
}



/*********************************************************************************************************
* �� �� �� : Scanf_Key
* ����˵�� : ����ɨ��
* ��    �� : mode��1������0����
* �� �� ֵ : ���µİ�����Ӧ��ֵ����KEY1����1
* ��    ע : PA0��KEY1����PB1��KEY2����PC13��KEY3��
*********************************************************************************************************/ 
unsigned char Scanf_Key(unsigned char mode)
{
	static unsigned char flag = 0;						//Ĭ��û�б�ǰ���
	if(mode)	flag = 0;									//mode=1ʱ�������������������������
	if((KEY1 == 1 || KEY2 == 0 || KEY3 == 0) && flag == 0)	//���������£�����û�б���ǰ�
	{
		flag = 1;							//��ǰ���
		Delay_ms(100);						//�ȴ�������ʧ
		if(KEY1 == 1)		return 1;		//����������ǰ��µ�״̬����ô����İ�����
		else if(KEY2 == 0)	return 2;		//������Ӧ��ֵ
		else if(KEY3 == 0)	return 3;	
	}
	else if((KEY1 == 0 && KEY2 == 1 && KEY3 == 1) && flag == 1)	//������а������ɿ��ˣ����ұ���ǹ����� 
		flag = 0;												//����ɿ���������һ�μ��
	return 0;
}