#include "rgb.h"
#include "systick.h"
static void RGB_Reset(void);
static void W2812_WriteByte(unsigned char Byte);
/*********************************************************************************************************
* �� �� �� : Rgb_Init
* ����˵�� : RGB����˿ڳ�ʼ��
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : PB15
*********************************************************************************************************/ 
void Rgb_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	//ʹ��PB����ʱ��
	
	GPIO_InitStruct.GPIO_Pin 	= GPIO_Pin_14;			//��ʼ������
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_OUT;		//���ģʽ
	GPIO_InitStruct.GPIO_OType 	= GPIO_OType_PP;		//�������
	GPIO_InitStruct.GPIO_PuPd 	= GPIO_PuPd_UP;			//��������
	GPIO_InitStruct.GPIO_Speed 	= GPIO_Speed_2MHz;		//2MHZ
	GPIO_Init(GPIOB, &GPIO_InitStruct);					//����GPIO_InitStruct��ʼ��
	
	GPIO_SetBits(GPIOB, GPIO_Pin_14);		
	
	RGB_Reset();										//��λ
	W2812_WriteColorData(RGB_BLACK);					//RGB1Ĭ��Ϩ��
	W2812_WriteColorData(RGB_BLACK);					//RGB2Ĭ��Ϩ��
}
/*********************************************************************************************************
* �� �� �� : W2812_WriteColorData
* ����˵�� : д����ɫֵ
* ��    �� : color����Ҫ��ʾ����ɫ
* �� �� ֵ : ��
* ��    ע : ͨ��RGB��ȣ�������ʾ����ͬ����ɫ��IOģ��ʱ���Ǻ�׼ȷ������Ч�ʼ��ͣ���Ŀ�в�����ʹ�ô˷���
*********************************************************************************************************/ 
void W2812_WriteColorData(unsigned int color)
{
	W2812_WriteByte((color>>8)&0xff);		//��
	W2812_WriteByte((color>>16)&0xff);		//��
	W2812_WriteByte((color>>0)&0xff);		//��
}
/*********************************************************************************************************
* �� �� �� : W2812_WriteByte
* ����˵�� : ��RGB����д��һ���ֽڵ�����
* ��    �� : Byte��Ҫд�����ֵ
* �� �� ֵ : ��
* ��    ע : 	Data transefer time:
				T0H 		0 code ,high voltage time 		0.4us ��150ns
				T1H 		1 code ,high voltage time 		0.8us ��150ns
				T0L 		0 code , low voltage time 		0.85us ��150ns
				T1L 		1 code ,low voltage time 		0.45us ��150ns
				RES 		low voltage time 				Above 50us
*********************************************************************************************************/ 
static void W2812_WriteByte(unsigned char Byte)
{
	unsigned char i = 0, j = 0;
	for(i=0; i<8; i++)
	{
		if(Byte&0x80)		
		{
			RGB_DATA = 1;	
			Delay_Us(1);			//��1����ߵ�ƽ����0.8ns +-150ns
			RGB_DATA = 0;		
			for(j=0; j<10; j++);	//��1����͵�ƽ����0.4ns +-150ns���˴�ѭ�������ӳ٣�
		}
		else 				
		{
			RGB_DATA = 1;	
			for(j=0; j<10; j++);	//��0����ߵ�ƽ����0.45ns +-150ns
			RGB_DATA = 0;	
			Delay_Us(1);			//��0����͵�ƽ����0.85ns +-150ns���˴�ѭ�������ӳ٣�
		}
		Byte <<= 1;
	}	
}
/*********************************************************************************************************
* �� �� �� : RGB_Reset
* ����˵�� : 
* ��    �� : 
* �� �� ֵ : 
* ��    ע : 
*********************************************************************************************************/ 
static void RGB_Reset(void)
{
	RGB_DATA = 0;
	Delay_Us(50);
}
