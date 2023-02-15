#ifndef SHT30_H
#define SHT30_H

#include "io_bit.h"

#define SHT30_ADDR 0X44

typedef struct
{
	float Temperature;
	float Humidity;
}SHT30_TypeDef; 
 
void Sht30_Init(void);
void Sht30_ReadData(SHT30_TypeDef *HT);
 
#endif
