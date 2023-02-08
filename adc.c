#include "adc.h"
#include "systick.h"

void Adc1_Init(void)
{
	//ʹ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
	//ģ������
	GPIO_InitTypeDef GPIO_InitStruct={0};
	GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_AN; //ģ������
//	GPIO_InitStruct.GPIO_OType  =
	GPIO_InitStruct.GPIO_Pin    = GPIO_Pin_0;
//	GPIO_InitStruct.GPIO_PuPd   =
//	GPIO_InitStruct.GPIO_Speed  =
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin    = GPIO_Pin_1;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	//ʹ��ADC1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	
	//ADCģ���ڲ�����
	ADC_InitTypeDef ADC_InitStruct={0};
	ADC_InitStruct.ADC_ContinuousConvMode    = ENABLE;    //����
	ADC_InitStruct.ADC_DataAlign             = ADC_DataAlign_Right;   //�����Ҷ���
//	ADC_InitStruct.ADC_ExternalTrigConv      =           //�ⲿ������ʽ
	ADC_InitStruct.ADC_ExternalTrigConvEdge  = ADC_ExternalTrigConvEdge_None; //�������
	ADC_InitStruct.ADC_NbrOfConversion       = 1;          //��Ҫת����ͨ����
	ADC_InitStruct.ADC_Resolution            = ADC_Resolution_12b;    //12λ,����
	ADC_InitStruct.ADC_ScanConvMode          = DISABLE;    //��ɨ�裨��ͨ��ʱʹ�ܣ�
	ADC_Init(ADC1,&ADC_InitStruct);
	
	
	ADC_CommonInitTypeDef ADC_CommonInitStruct={0};
	ADC_CommonInitStruct.ADC_DMAAccessMode    = ADC_DMAAccessMode_Disabled;   //DMA��ʹ��
	ADC_CommonInitStruct.ADC_Mode             = ADC_Mode_Independent;         //����ģʽ
	ADC_CommonInitStruct.ADC_Prescaler        = ADC_Prescaler_Div4;           //  84M/4=21M   max=36M
	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; // ˫�����ӳ٣�����ʱ��
	ADC_CommonInit(&ADC_CommonInitStruct);
	//��ADC1
	ADC_Cmd(ADC1,ENABLE);
}


unsigned short Get_value(int ch)
{
	ADC_RegularChannelConfig(ADC1,ch,1,ADC_SampleTime_15Cycles);
	ADC_SoftwareStartConv(ADC1);   //����ADC����ת��
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==0);
	return ADC_GetConversionValue(ADC1) & 0xfff; //ֻȡ��12λ
		
}
//#define N 12

//char filter()
//{
//   char count,i,j;
//   char value_buf[N];
//   int  temp,sum=0;
//   for  (count=0;count<N;count++)
//   {
//      value_buf[count] =Get_value(ch);
//      Delay_Ms(500);
//   }
//   for (j=0;j<N-1;j++)
//   {
//      for (i=0;i<N-j;i++)
//      {
//         if (value_buf>value_buf[i+1])
//         {
//            temp = value_buf;
//            value_buf = value_buf[i+1]; 
//             value_buf[i+1] = temp;
//         }
//      }
//   }
//   for(count=1;count<N-1;count++)
//      sum += value[count];
//   return (char)(sum/(N-2));
//}




