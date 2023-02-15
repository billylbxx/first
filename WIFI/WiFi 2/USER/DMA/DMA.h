#ifndef __DMA_H
#define __DMA_H
#include "io_bit.h" 

void dma_Init(void);
void myDMA_ENABLE(void);
void DMA2_Stream7_IRQHandler(void);
#endif
