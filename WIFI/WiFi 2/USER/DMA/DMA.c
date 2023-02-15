#include "DMA.h"
#include "stm32f4xx.h"
#include "stdio.h"
#include <string.h>
static char buffer[1000]="GEGPIO";
void dma_Init(void)
{ 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2时钟使能
	
	DMA_InitTypeDef DMA_InitStruct={0};
	DMA_InitStruct.DMA_BufferSize				=strlen(buffer);//要传输的总字节数
	DMA_InitStruct.DMA_Channel					=DMA_Channel_4;//通道4，由硬件决定
	DMA_InitStruct.DMA_DIR							=DMA_DIR_MemoryToPeripheral;//方向内存到外设
	DMA_InitStruct.DMA_FIFOMode					=DMA_FIFOMode_Disable;//不开缓存
	DMA_InitStruct.DMA_FIFOThreshold		=DMA_FIFOThreshold_Full;//FIFO大小，一次传输所有
	DMA_InitStruct.DMA_Memory0BaseAddr	=(uint32_t)buffer;//存储器地址
	DMA_InitStruct.DMA_MemoryBurst			=DMA_MemoryBurst_Single;//突发模式单字节
	DMA_InitStruct.DMA_MemoryDataSize		=DMA_MemoryDataSize_Byte;//字节 位宽(还有半字，字)
	DMA_InitStruct.DMA_MemoryInc				=DMA_MemoryInc_Enable;//地址增量使能
	DMA_InitStruct.DMA_Mode							=DMA_Mode_Normal;//单次搬运
	DMA_InitStruct.DMA_PeripheralBaseAddr	=(uint32_t)&USART1->DR;//外设地址
	DMA_InitStruct.DMA_PeripheralBurst		=DMA_PeripheralBurst_Single;//突发模式单字节
	DMA_InitStruct.DMA_PeripheralDataSize	=DMA_PeripheralDataSize_Byte;
	DMA_InitStruct.DMA_PeripheralInc			=DMA_PeripheralInc_Disable;//地址增量不使能
	DMA_InitStruct.DMA_Priority						=DMA_Priority_VeryHigh;//非常高优先级
	
	DMA_Init(DMA2_Stream7, &DMA_InitStruct);
	
	//使能中断
	DMA_ITConfig(DMA2_Stream7,DMA_IT_TC,ENABLE);//使能DMA完成中断
	NVIC_InitTypeDef NVIC_InitStruct={0};
	NVIC_InitStruct.NVIC_IRQChannel=DMA2_Stream7_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//使能中断源
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;//先占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;//从优先级
	NVIC_Init(&NVIC_InitStruct);

	
}


void myDMA_ENABLE(void)
{
	DMA_Cmd(DMA2_Stream7,DISABLE);//关闭DMA传输
	while(DMA_GetCmdStatus(DMA2_Stream7)!=DISABLE){}//确保DMA可以被设置
	DMA_SetCurrDataCounter(DMA2_Stream7,strlen(buffer));//数据传输量
	DMA_Cmd(DMA2_Stream7,ENABLE);//开启DMA传输
		
}

void DMA2_Stream7_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream7,DMA_IT_TCIF7))//如果触发了传输完成
	{
		DMA_ClearITPendingBit(DMA2_Stream7,DMA_IT_TCIF7);	
	}
}
