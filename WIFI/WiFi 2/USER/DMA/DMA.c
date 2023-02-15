#include "DMA.h"
#include "stm32f4xx.h"
#include "stdio.h"
#include <string.h>
static char buffer[1000]="GEGPIO";
void dma_Init(void)
{ 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2ʱ��ʹ��
	
	DMA_InitTypeDef DMA_InitStruct={0};
	DMA_InitStruct.DMA_BufferSize				=strlen(buffer);//Ҫ��������ֽ���
	DMA_InitStruct.DMA_Channel					=DMA_Channel_4;//ͨ��4����Ӳ������
	DMA_InitStruct.DMA_DIR							=DMA_DIR_MemoryToPeripheral;//�����ڴ浽����
	DMA_InitStruct.DMA_FIFOMode					=DMA_FIFOMode_Disable;//��������
	DMA_InitStruct.DMA_FIFOThreshold		=DMA_FIFOThreshold_Full;//FIFO��С��һ�δ�������
	DMA_InitStruct.DMA_Memory0BaseAddr	=(uint32_t)buffer;//�洢����ַ
	DMA_InitStruct.DMA_MemoryBurst			=DMA_MemoryBurst_Single;//ͻ��ģʽ���ֽ�
	DMA_InitStruct.DMA_MemoryDataSize		=DMA_MemoryDataSize_Byte;//�ֽ� λ��(���а��֣���)
	DMA_InitStruct.DMA_MemoryInc				=DMA_MemoryInc_Enable;//��ַ����ʹ��
	DMA_InitStruct.DMA_Mode							=DMA_Mode_Normal;//���ΰ���
	DMA_InitStruct.DMA_PeripheralBaseAddr	=(uint32_t)&USART1->DR;//�����ַ
	DMA_InitStruct.DMA_PeripheralBurst		=DMA_PeripheralBurst_Single;//ͻ��ģʽ���ֽ�
	DMA_InitStruct.DMA_PeripheralDataSize	=DMA_PeripheralDataSize_Byte;
	DMA_InitStruct.DMA_PeripheralInc			=DMA_PeripheralInc_Disable;//��ַ������ʹ��
	DMA_InitStruct.DMA_Priority						=DMA_Priority_VeryHigh;//�ǳ������ȼ�
	
	DMA_Init(DMA2_Stream7, &DMA_InitStruct);
	
	//ʹ���ж�
	DMA_ITConfig(DMA2_Stream7,DMA_IT_TC,ENABLE);//ʹ��DMA����ж�
	NVIC_InitTypeDef NVIC_InitStruct={0};
	NVIC_InitStruct.NVIC_IRQChannel=DMA2_Stream7_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//ʹ���ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;//�����ȼ�
	NVIC_Init(&NVIC_InitStruct);

	
}


void myDMA_ENABLE(void)
{
	DMA_Cmd(DMA2_Stream7,DISABLE);//�ر�DMA����
	while(DMA_GetCmdStatus(DMA2_Stream7)!=DISABLE){}//ȷ��DMA���Ա�����
	DMA_SetCurrDataCounter(DMA2_Stream7,strlen(buffer));//���ݴ�����
	DMA_Cmd(DMA2_Stream7,ENABLE);//����DMA����
		
}

void DMA2_Stream7_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream7,DMA_IT_TCIF7))//��������˴������
	{
		DMA_ClearITPendingBit(DMA2_Stream7,DMA_IT_TCIF7);	
	}
}
