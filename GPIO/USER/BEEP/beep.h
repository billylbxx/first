#ifndef _BEEP_H
#define _BEEP_H

#include "stm32f4xx.h"

#define BEEP PCout(12)

void Beep_Init(void);

#endif
