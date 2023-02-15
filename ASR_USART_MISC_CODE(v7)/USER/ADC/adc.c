#include "adc.h"
/*********************************************************************************************************
* 函 数 名 : Adc_Init
* 功能说明 : 初始化ADC1
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : NTC_CH：PC0，IR_CH：PC1
*********************************************************************************************************/ 
void Adc_Init(void)
{
	GPIO_InitTypeDef  		GPIO_InitStructure      = {0};
	ADC_CommonInitTypeDef	ADC_CommonInitStructure = {0};
	ADC_InitTypeDef       	ADC_InitStructure		= {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 	//使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,  ENABLE);	//使能ADC1时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;		//模拟输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//不带上下拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);				//初始化  

	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);	 //ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE); 	 //复位结束	 

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;					//独立模式
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样阶段之间的延迟5个时钟
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; 	//禁止DMA直接访问
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;					//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
	ADC_CommonInit(&ADC_CommonInitStructure);									//初始化

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;	//12位模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;			//非扫描模式	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		//关闭连续转换
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;	//禁止触发检测，使用软件触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//右对齐	
	ADC_InitStructure.ADC_NbrOfConversion = 2;				//2个转换在规则序列中 也就是只转换规则序列1 
	ADC_Init(ADC1, &ADC_InitStructure);						//ADC初始化
	
	ADC_Cmd(ADC1, ENABLE);									//开启AD转换器	
}
/*********************************************************************************************************
* 函 数 名 : Get_AdcValue
* 功能说明 : 获取ADC通道值
* 形    参 : ch：通道号
* 返 回 值 : 采集到的模拟量
* 备    注 : 无
*********************************************************************************************************/ 
unsigned short Get_AdcValue(unsigned char ch)   
{
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADC通道,转换数量，480个周期,提高采样时间可以提高精确度			    
	ADC_SoftwareStartConv(ADC1);					//使能指定的ADC1的软件转换启动功能	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));	//等待转换结束
	return ADC_GetConversionValue(ADC1);			//返回最近一次ADC1规则组的转换结果
}
/*********************************************************************************************************
* 函 数 名 : Get_AverageValue
* 功能说明 : 简单滤波算法，排序取中间求平均
* 形    参 : ch：通道号
* 返 回 值 : 处理后的数据
* 备    注 : 无
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
