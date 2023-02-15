#ifndef __KEY_H_
#define __KEY_H_
#include "stm32f4xx.h"  
#include "io_bit.h"   
#define key1 PAin(0)
void Key_init(void);
u8 Key_Scan(void);
#endif


