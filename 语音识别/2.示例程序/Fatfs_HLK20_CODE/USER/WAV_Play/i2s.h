#ifndef __I2S_H
#define __I2S_H
#include "io_bit.h"    									


void I2S2_Init(u8 std,u8 mode,u8 cpol,u8 datalen);  
u8 I2S2_SampleRate_Set(u32 samplerate);
void I2S2_TX_DMA_Init(u8* buf0,u8 *buf1,u16 num); 
  
#endif





















