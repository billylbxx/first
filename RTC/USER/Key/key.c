#include "stm32f4xx.h"
#include "key.h"
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
* ��    �� : ��
* �� �� ֵ : ���а�����״̬
* ��    ע : PA0��SWITCH_KEY��
*********************************************************************************************************/ 
char Scanf_Key(void)
{
	static unsigned char state = KEY_FREE;
	static unsigned char cnt = 0;
	static unsigned char key = 0;
	
	switch(state)
	{
		case KEY_FREE:	//����Ƿ��а������£������������״̬
			if(KEY1 == true || KEY2 == false || KEY3 == false)		
				state = KEY_SHAKE;
			break;
		case KEY_SHAKE:
			if(++cnt > 5)				//����50ms�����50ms�󰴼����ǰ���״̬����Ϊ��İ��£�������Ϊ����
			{
				cnt = 0;
				if(KEY1 == true || KEY2 == false || KEY3 == false)		
				{
					key = ((!!KEY1)<<0) | ((!KEY2)<<1) | ((!KEY3)<<2);	//��¼������ǰֵ
					state = KEY_PRESS;	//������һ��״̬
				}
				else 	state = KEY_FREE;		//�ص����״̬
			}
			break;
		case KEY_PRESS:	//������µ�ʱ�䳬��1s����ô��Ϊ�ǳ��������ؽ������������һ��״̬Ϊ�ȴ�����̬
			if(KEY1 == true || KEY2 == false || KEY3 == false)			
			{
				if(++cnt > 100)
				{
					state = KEY_LOOSEN;
					return (key|KEY_LONG_PRESS);	//���λ��ǳ����̰�
				}
			}
			else					
			{
				state = KEY_LOOSEN;	//������һ��״̬Ϊ�ȴ�����̬
				return (key|KEY_SHORT_PRESS);		//�����1s��ʱ�������֣���ô��Ϊ�̰������ؽ����
			}
			break;
		case KEY_LOOSEN:	//���а����ɿ�������Ϊ���ΰ��½������ص������״̬
			if(KEY1 == false && KEY2 == true && KEY3 == true)	
			{
				state = KEY_FREE;
				cnt = 0;
				key = 0;
			}
			break;
	}
	return 0;
}
