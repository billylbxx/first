#ifndef __KEY_H
#define __KEY_H

#include "io_bit.h" 

#define KEY1 PAin(0)
#define KEY2 PBin(1)
#define KEY3 PCin(13)

void Key_Init(void);
unsigned char Scanf_Key(unsigned char mode);

#endif
