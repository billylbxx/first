#ifndef TOUCH_H
#define TOUCH_H

#include "main.h"

#define T_SCK(x)  x?(GPIOB->ODR |= (1<<0)):(GPIOB->ODR &=~ (1<<0))
#define T_DIN(x)  x?(GPIOF->ODR |= (1<<11)):(GPIOF->ODR &=~ (1<<11))
#define T_DOUT()  !!(GPIOB->IDR & (1<<2))
#define T_PEN()   !!(GPIOB->IDR & (1<<1))
#define T_CS(x)   x?(GPIOC->ODR |= (1<<13)):(GPIOC->ODR &=~ (1<<13))

#define T_PARAM_ADDR   0
#define ADJUSTFLAG     0xA2

#define cmd_x 0xD0
#define cmd_y 0x90

typedef struct
{
  uint16_t ad_xvalue;
	uint16_t ad_yvalue;
	uint8_t isadjust;
	uint16_t xpos;
	uint16_t ypos;
	
	float A;
	float B;
	float C;
	float D;
	float E;
	float F;
}TOUCHDef;
extern TOUCHDef touch;

void Touch_Init(void);
void adjust(void);
void xyposget(void);

#endif
