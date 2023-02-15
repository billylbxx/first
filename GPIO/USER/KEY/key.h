#ifndef _KEY_H
#define _KEY_H

#include "stm32f4xx.h"
#include "io_bit.h"

#define KEY1 PAin(0)
#define KEY2 PCin(13)
#define KEY3 PBin(1)

void Key_Init(void);

#endif
