#include "adc.h"
/*********************************************************************************************************
* �� �� �� : Adc_Init
* ����˵�� : ��ʼ��ADC1
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : NTC_CH��PC0��IR_CH��PC1
*********************************************************************************************************/ 
void Adc_Init(void)
{
	GPIO_InitTypeDef  		GPIO_InitStructure      = {0};
	ADC_CommonInitTypeDef	ADC_CommonInitStructure = {0};
	ADC_InitTypeDef       	ADC_InitStructure		= {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 	//ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,  ENABLE);	//ʹ��ADC1ʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;		//ģ������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);				//��ʼ��  

	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);	 //ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE); 	 //��λ����	 

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;					//����ģʽ
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; 	//��ֹDMAֱ�ӷ���
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;					//Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
	ADC_CommonInit(&ADC_CommonInitStructure);									//��ʼ��

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;	//12λģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;			//��ɨ��ģʽ	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		//�ر�����ת��
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;	//��ֹ������⣬ʹ���������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//�Ҷ���	
	ADC_InitStructure.ADC_NbrOfConversion = 2;				//2��ת���ڹ��������� Ҳ����ֻת����������1 
	ADC_Init(ADC1, &ADC_InitStructure);						//ADC��ʼ��
	
	ADC_Cmd(ADC1, ENABLE);									//����ADת����	
}
/*********************************************************************************************************
* �� �� �� : Get_AdcValue
* ����˵�� : ��ȡADCͨ��ֵ
* ��    �� : ch��ͨ����
* �� �� ֵ : �ɼ�����ģ����
* ��    ע : ��
*********************************************************************************************************/ 
unsigned short Get_AdcValue(unsigned char ch)   
{
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADCͨ��,ת��������480������,��߲���ʱ�������߾�ȷ��			    
	ADC_SoftwareStartConv(ADC1);					//ʹ��ָ����ADC1�����ת����������	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));	//�ȴ�ת������
	return ADC_GetConversionValue(ADC1);			//�������һ��ADC1�������ת�����
}
/*********************************************************************************************************
* �� �� �� : Get_AverageValue
* ����˵�� : ���˲��㷨������ȡ�м���ƽ��
* ��    �� : ch��ͨ����
* �� �� ֵ : ����������
* ��    ע : ��
*********************************************************************************************************/ 
unsigned short Get_AverageValue(unsigned char ch)
{
	unsigned char i = 0, j = 0;
	unsigned short tem[10] = {0}, n = 0;
	
	for(i=0; i<10; i++)
		tem[i] = Get_AdcValue(ch);

	for(i=0; i<10; i++)
	{
		for(j=9; j>i; j--)
		{
			if(tem[j] < tem[j-1])
			{
				n 		 = tem[j];
				tem[j]   = tem[j-1];
				tem[j-1] = n;
			}
		}
	}
	
	return (tem[4] + tem[5]) / 2;
}
