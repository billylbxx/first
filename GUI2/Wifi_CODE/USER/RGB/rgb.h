#ifndef __RGB_H
#define __RGB_H

#include "io_bit.h" 

#define Pixel_NUM    2		//灯珠 RGB数量

#define CODE0 0xC0      	//0码发送的时间 
#define CODE1 0xF8      	//1码发送的时间

#define RGB_RED 	0xff0000
#define RGB_GREEN 	0x00ff00
#define RGB_BLUE	0x0000ff
#define RGB_SKY		0x00ffff
#define RGB_MAGENTA 0xff00ff
#define RGB_YELLOW  0xffff00
#define RGB_ORANGE  0x7f6a00
#define RGB_BLACK   0x000000
#define RGB_WHITE   0xffffff
#define RGB_PURPLE  0x4169e1

void Rgb_Init(void);

void RGB_SetColor(unsigned char LedId, const unsigned int Color);
void RGB_SetNumColor(unsigned char Pixel_LEN, const unsigned int Color);


#endif
