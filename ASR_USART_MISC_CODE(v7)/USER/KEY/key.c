#include <stdbool.h>
#include "key.h"
#include "led.h"
#include "systick.h"
#include "app_task.h"

/*********************************************************************************************************
* �� �� �� : TaskKeyMsg_Handle
* ����˵�� : ������������
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : PA0��KEY1����PB1��KEY2����PC13��KEY3��
*********************************************************************************************************/ 
void TaskKeyMsg_Handle(void)
{
	static unsigned char key = 0;
	static unsigned char n = 0;
	
	//key = Scanf_Key();
	switch(key)
	{
		case KEY1_SHORT:
		{
			float temperature = 0.0;
			BaseType_t xHigherPriorityTaskWoken;
			
			Mlx90614_ReadTemperature(&temperature);
			xQueueSendFromISR(MessageQueue_Tem, &temperature, &xHigherPriorityTaskWoken);	//�ѽ�����͵���Ϣ����
		}	break;
		
		case KEY2_SHORT:
			n = 0;
			break;
		
		case KEY3_SHORT:
			Infrared_SendCmd(IR_IN_ENTER, n);	//�����ڲ�ѧϰģʽ
			if(++n >= 6)
				n = 0;
			break;
		
		case KEY1_LONG:
			Infrared_SendCmd(IR_EX_ENTRE,  0);//�����ⲿѧϰģʽ
			break;
		
		case KEY2_LONG:
			printf("��������ģʽ\r\n");
			Set_Esp12ConnectionStatus(THREE);	//��������ģ#ʽ
			RevampState(NULL, AT_CWMODE2, AT_CWMODE2, AT_CWMODE2, AT_OK, WAIT_1000, WAIT_100);
			break;
		
		case KEY3_LONG:
			Infrared_SendCmd(IR_FORMAT, 0);	//��ʽ������ģ��
			break;
	}
}

/*********************************************************************************************************
* �� �� �� : Scanf_Key
* ����˵�� : ����ɨ��
* ��    �� : ��
* �� �� ֵ : ���а�����״̬
* ��    ע : PA0��SWITCH_KEY��
*********************************************************************************************************/ 
#if 0
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
#endif
#if 1
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
		Delay_Ms(10);						//�ȴ�������ʧ
		if(KEY1 == 1)		return 1;		//����������ǰ��µ�״̬����ô����İ�����
		else if(KEY2 == 0)	return 2;		//������Ӧ��ֵ
		else if(KEY3 == 0)	return 3;	
	}
	else if((KEY1 == 0 && KEY2 == 1 && KEY3 == 1) && flag == 1)	//������а������ɿ��ˣ����ұ���ǹ����� 
		flag = 0;												//����ɿ���������һ�μ��
	return 0;
}
#endif
/*********************************************************************************************************
* �� �� �� : Key_Init
* ����˵�� : KEY�˿ڳ�ʼ��
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : PA0��KEY1����PB1��KEY2����PC13��KEY3��
*********************************************************************************************************/ 
void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	//ʹ��PA����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	//ʹ��PB����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	//ʹ��PC����ʱ��
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0;					//��ʼ������
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;				//����ģʽ
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;			//��������
	GPIO_Init(GPIOA, &GPIO_InitStruct);						//����GPIO_InitStruct��ʼ��
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_1;					//��ʼ������
	GPIO_Init(GPIOB, &GPIO_InitStruct);						//����GPIO_InitStruct��ʼ��
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_13;				//��ʼ������
	GPIO_Init(GPIOC, &GPIO_InitStruct);						//����GPIO_InitStruct��ʼ��	
}
