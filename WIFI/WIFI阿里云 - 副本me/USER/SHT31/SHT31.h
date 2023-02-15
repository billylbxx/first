#ifndef __SHT31_H
#define __SHT31_H
#include "io_bit.h" 
void SHT31_Init(void);
void SHT31_read_T_H(void);
unsigned char crc8(const unsigned char *data, int len);
typedef struct 
{
	float t;
	float h;
}t_h;
extern t_h temp;
#endif
