#ifndef __KEY_H_
#define __KEY_H_
#include "stm32f4xx.h"  
#include "io_bit.h"

#define key1 PAin(0)
#define key2 PCin(13)
#define key3 PBin(1)

void Key_init(void);
//u8 Key_Scan(void);
#endif


