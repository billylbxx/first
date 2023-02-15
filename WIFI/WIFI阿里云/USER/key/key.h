#ifndef _KEY_H_
#define  _KEY_H_
#include "stm32f4xx.h"
#include "io_bit.h"

#define key1 PAin(0) 
#define key2 PCin(13) 
#define key3 PBin(1) 

#define KEY1_WK_UP 1
#define KEY2 2
#define KEY3 3

void Key_Init(void);
int Scanf_Key(int mode);



#endif 

