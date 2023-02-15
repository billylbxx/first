#ifndef __SPI_H
#define __SPI_H

#include "io_bit.h" 

void Spi1_Init(void);
void Spi2_Init(void);
void Spi3_Init(void);

unsigned char Spi1_WriteReadByte(unsigned char data);
unsigned char Spi2_WriteReadByte(unsigned char data);
unsigned char Spi3_WriteReadByte(unsigned char data);

#endif
