#ifndef __DMA_H
#define __DMA_H
#include "io_bit.h" 
//#define interrupt
void dma_Init(void);
void myDMA_ENABLE(void);
extern uint16_t ADC_ConvertedValue;
#endif
