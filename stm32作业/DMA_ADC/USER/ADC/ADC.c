#include "ADC.h"
#include "stm32f4xx.h"
#include "systick.h"

//PC0
//规则组，单次，1通道 ADC1-IN10

void ADC1_Init(void)
{
	//使能C口时钟
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
	
	//使能ADC1时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	
	ADC_InitTypeDef ADC_InitStruct={0};
  ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;//精度
  ADC_InitStruct.ADC_ScanConvMode = DISABLE;//不扫描
  ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;//连续
  ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//软件触发
  //ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;//外部触发
  ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//数据右对齐
  ADC_InitStruct.ADC_NbrOfConversion = 1;//AD转换的总通道数
	ADC_Init(ADC1, &ADC_InitStruct);
	
	ADC_CommonInitTypeDef ADC_CommonInitStruct={0};
	ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;//独立模式
  ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div4;//4分频  84/4=21  M4不超过36 M3不超过14
  ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//DMA不使能（多重ADC）
  ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//T=(5+12.5)/21 采样时间
	ADC_CommonInit(&ADC_CommonInitStruct);
	
	
	//开ADC1
	ADC_Cmd(ADC1,ENABLE);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_10,1,ADC_SampleTime_15Cycles);//通道10，采样时间为15周期
	ADC_SoftwareStartConv(ADC1);//确定adc软件采样转换
	ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE);//源数据变化时开启DMA传输
	ADC_DMACmd(ADC1, ENABLE);////使能ADC的DMA传输
}

unsigned short get_value(int ch)
{
	ADC_RegularChannelConfig(ADC1,ch,1,ADC_SampleTime_480Cycles);
	ADC_SoftwareStartConv(ADC1);//确定adc采样转换
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);
	return ADC_GetConversionValue(ADC1)&0xfff;//只取低12位
}

unsigned short get2_value(int ch)
{
	ADC_RegularChannelConfig(ADC2,ch,1,ADC_SampleTime_15Cycles);
	ADC_SoftwareStartConv(ADC2);//确定adc采样转换
	while(ADC_GetFlagStatus(ADC2,ADC_FLAG_EOC)==RESET);
	return ADC_GetConversionValue(ADC2)&0xfff;//只取低12位
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
