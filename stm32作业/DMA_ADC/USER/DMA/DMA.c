#include "DMA.h"
#include "stm32f4xx.h"
#include "stdio.h"
#include "systick.h"
#include <string.h>
uint16_t ADC_ConvertedValue;
void dma_Init(void)
{ 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2ʱ��ʹ��
	
	DMA_Cmd(DMA2_Stream0,DISABLE);//�ر�DMA����
	while(DMA_GetCmdStatus(DMA2_Stream0)!=DISABLE);/*�ȴ�DMA��������*/
	
	DMA_InitTypeDef DMA_InitStruct={0};
	DMA_InitStruct.DMA_BufferSize				=1;//Ҫ��������ֽ���
	DMA_InitStruct.DMA_Channel					=DMA_Channel_0;//ͨ��0����Ӳ������
	DMA_InitStruct.DMA_DIR							=DMA_DIR_PeripheralToMemory;//�������赽�ڴ�
	DMA_InitStruct.DMA_FIFOMode					=DMA_FIFOMode_Enable;//������
	DMA_InitStruct.DMA_FIFOThreshold		=DMA_FIFOThreshold_Full;//FIFO��С��һ�δ�������
	//DMA_Memory
	DMA_InitStruct.DMA_Memory0BaseAddr	=(uint32_t)&ADC_ConvertedValue;//�洢����ַ
	DMA_InitStruct.DMA_MemoryBurst			=DMA_MemoryBurst_Single;//ͻ��ģʽ���ֽ�
	DMA_InitStruct.DMA_MemoryDataSize		=DMA_MemoryDataSize_HalfWord;//���� λ��(���а��֣���)
	DMA_InitStruct.DMA_MemoryInc				=DMA_MemoryInc_Disable;//�ڴ��ַ�̶�
	
	DMA_InitStruct.DMA_Mode							=DMA_Mode_Circular;//ѭ������
	//DMA_Peripheral
	DMA_InitStruct.DMA_PeripheralBaseAddr	=(uint32_t)&ADC1->DR;//�����ַ
	DMA_InitStruct.DMA_PeripheralBurst		=DMA_PeripheralBurst_Single;//ͻ��ģʽ���ֽ�
	DMA_InitStruct.DMA_PeripheralDataSize	=DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_PeripheralInc			=DMA_PeripheralInc_Disable;//��ַ������ʹ��
	DMA_InitStruct.DMA_Priority						=DMA_Priority_VeryHigh;//�ǳ������ȼ�
	
	DMA_Init(DMA2_Stream0, &DMA_InitStruct);
	DMA_Cmd(DMA2_Stream0,ENABLE);//����DMA����
	
	#ifdef interrupt
	//ʹ���ж�
	DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,ENABLE);//ʹ��DMA����ж�
	NVIC_InitTypeDef NVIC_InitStruct={0};
	NVIC_InitStruct.NVIC_IRQChannel=DMA2_Stream0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//ʹ���ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;//�����ȼ�
	NVIC_Init(&NVIC_InitStruct);
	#endif
	
}


//void myDMA_ENABLE(void)
//{
//	DMA_Cmd(DMA2_Stream0,DISABLE);//�ر�DMA����
//	while(DMA_GetCmdStatus(DMA2_Stream0)!=DISABLE){}//ȷ��DMA���Ա�����
//	DMA_SetCurrDataCounter(DMA2_Stream0,2);//���ݴ�����
//	DMA_Cmd(DMA2_Stream0,ENABLE);//����DMA����
//		
//}

void DMA2_Stream0_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream0,DMA_IT_TCIF0))//��������˴������
	{
		printf("ad=%d\r\n",ADC_ConvertedValue&0x0fff);
		printf("V=%0.2f\r\n",3.3*(ADC_ConvertedValue&0x0fff)/4096);
		DMA_ClearITPendingBit(DMA2_Stream0,DMA_IT_TCIF0);
		Delay_ms(500);
		Delay_ms(500);		
	}
}
