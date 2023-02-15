#include "DMA.h"
#include "stm32f4xx.h"
#include "stdio.h"
#include "systick.h"
#include <string.h>
uint16_t ADC_ConvertedValue;
void dma_Init(void)
{ 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2时钟使能
	
	DMA_Cmd(DMA2_Stream0,DISABLE);//关闭DMA传输
	while(DMA_GetCmdStatus(DMA2_Stream0)!=DISABLE);/*等待DMA可以配置*/
	
	DMA_InitTypeDef DMA_InitStruct={0};
	DMA_InitStruct.DMA_BufferSize				=1;//要传输的总字节数
	DMA_InitStruct.DMA_Channel					=DMA_Channel_0;//通道0，由硬件决定
	DMA_InitStruct.DMA_DIR							=DMA_DIR_PeripheralToMemory;//方向外设到内存
	DMA_InitStruct.DMA_FIFOMode					=DMA_FIFOMode_Enable;//开缓存
	DMA_InitStruct.DMA_FIFOThreshold		=DMA_FIFOThreshold_Full;//FIFO大小，一次传输所有
	//DMA_Memory
	DMA_InitStruct.DMA_Memory0BaseAddr	=(uint32_t)&ADC_ConvertedValue;//存储器地址
	DMA_InitStruct.DMA_MemoryBurst			=DMA_MemoryBurst_Single;//突发模式单字节
	DMA_InitStruct.DMA_MemoryDataSize		=DMA_MemoryDataSize_HalfWord;//半字 位宽(还有半字，字)
	DMA_InitStruct.DMA_MemoryInc				=DMA_MemoryInc_Disable;//内存地址固定
	
	DMA_InitStruct.DMA_Mode							=DMA_Mode_Circular;//循环搬运
	//DMA_Peripheral
	DMA_InitStruct.DMA_PeripheralBaseAddr	=(uint32_t)&ADC1->DR;//外设地址
	DMA_InitStruct.DMA_PeripheralBurst		=DMA_PeripheralBurst_Single;//突发模式单字节
	DMA_InitStruct.DMA_PeripheralDataSize	=DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_PeripheralInc			=DMA_PeripheralInc_Disable;//地址增量不使能
	DMA_InitStruct.DMA_Priority						=DMA_Priority_VeryHigh;//非常高优先级
	
	DMA_Init(DMA2_Stream0, &DMA_InitStruct);
	DMA_Cmd(DMA2_Stream0,ENABLE);//开启DMA传输
	
	#ifdef interrupt
	//使能中断
	DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,ENABLE);//使能DMA完成中断
	NVIC_InitTypeDef NVIC_InitStruct={0};
	NVIC_InitStruct.NVIC_IRQChannel=DMA2_Stream0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//使能中断源
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;//先占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;//从优先级
	NVIC_Init(&NVIC_InitStruct);
	#endif
	
}


//void myDMA_ENABLE(void)
//{
//	DMA_Cmd(DMA2_Stream0,DISABLE);//关闭DMA传输
//	while(DMA_GetCmdStatus(DMA2_Stream0)!=DISABLE){}//确保DMA可以被设置
//	DMA_SetCurrDataCounter(DMA2_Stream0,2);//数据传输量
//	DMA_Cmd(DMA2_Stream0,ENABLE);//开启DMA传输
//		
//}

void DMA2_Stream0_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream0,DMA_IT_TCIF0))//如果触发了传输完成
	{
		printf("ad=%d\r\n",ADC_ConvertedValue&0x0fff);
		printf("V=%0.2f\r\n",3.3*(ADC_ConvertedValue&0x0fff)/4096);
		DMA_ClearITPendingBit(DMA2_Stream0,DMA_IT_TCIF0);
		Delay_ms(500);
		Delay_ms(500);		
	}
}
