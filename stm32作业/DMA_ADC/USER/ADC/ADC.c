#include "ADC.h"
#include "stm32f4xx.h"
#include "systick.h"

//PC0
//�����飬���Σ�1ͨ�� ADC1-IN10

void ADC1_Init(void)
{
	//ʹ��C��ʱ��
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	//
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	//	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	//	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	//	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_1;//ADC2
	//	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	//ʹ��ADC1ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	
	ADC_InitTypeDef ADC_InitStruct={0};
  ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;//����
  ADC_InitStruct.ADC_ScanConvMode = DISABLE;//��ɨ��
  ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;//����
  ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//�������
  //ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;//�ⲿ����
  ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//�����Ҷ���
  ADC_InitStruct.ADC_NbrOfConversion = 1;//ADת������ͨ����
	ADC_Init(ADC1, &ADC_InitStruct);
	
	ADC_CommonInitTypeDef ADC_CommonInitStruct={0};
	ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;//����ģʽ
  ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div4;//4��Ƶ  84/4=21  M4������36 M3������14
  ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//DMA��ʹ�ܣ�����ADC��
  ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//T=(5+12.5)/21 ����ʱ��
	ADC_CommonInit(&ADC_CommonInitStruct);
	
	
	//��ADC1
	ADC_Cmd(ADC1,ENABLE);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_10,1,ADC_SampleTime_15Cycles);//ͨ��10������ʱ��Ϊ15����
	ADC_SoftwareStartConv(ADC1);//ȷ��adc�������ת��
	ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE);//Դ���ݱ仯ʱ����DMA����
	ADC_DMACmd(ADC1, ENABLE);////ʹ��ADC��DMA����
}

unsigned short get_value(int ch)
{
	ADC_RegularChannelConfig(ADC1,ch,1,ADC_SampleTime_480Cycles);
	ADC_SoftwareStartConv(ADC1);//ȷ��adc����ת��
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);
	return ADC_GetConversionValue(ADC1)&0xfff;//ֻȡ��12λ
}

unsigned short get2_value(int ch)
{
	ADC_RegularChannelConfig(ADC2,ch,1,ADC_SampleTime_15Cycles);
	ADC_SoftwareStartConv(ADC2);//ȷ��adc����ת��
	while(ADC_GetFlagStatus(ADC2,ADC_FLAG_EOC)==RESET);
	return ADC_GetConversionValue(ADC2)&0xfff;//ֻȡ��12λ
}

char filter(int N,int ch)
{
	int sum=0;
	for(int i=0;i<N;i++)
	{
		sum=sum+get2_value(ch);
		Delay_ms(5);
	}
	return (char)sum/N;
}
