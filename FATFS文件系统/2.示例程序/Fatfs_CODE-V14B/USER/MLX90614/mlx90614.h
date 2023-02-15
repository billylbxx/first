#ifndef __MLX90614_H
#define __MLX90614_H

#include "io_bit.h"

#define MLX90614_ADDR	0x5A 
#define RAM_ACCESS		0x00 
//#define EEPROM_ACCESS	0x20 
#define RAM_TOBJ1		0x07 

void Mlx90614_Init(void);
void Mlx90614_ReadTemperature(float *temp);

#endif
