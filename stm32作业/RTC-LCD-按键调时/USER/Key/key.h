#ifndef __KEY_H
#define __KEY_H
#include "io_bit.h" 
#define KEY1 PAin(0)
#define KEY2 PCin(13)
#define KEY3 PBin(1)

void key_Init(void);
unsigned char Scanf_Key(unsigned char mode);
#endif
