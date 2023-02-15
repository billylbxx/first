#ifndef __RGB_H
#define __RGB_H
#include "io_bit.h" 
#define RGB_1 0xF8
#define RGB_0 0xC0
#define RGB_num 4

void RGB_TO_GRB(u8 LEDid,u32 color);
void RGB_Init(void);
u8 RGB_read_write(u8 data);
void resetRGB(void);
void RGB_ENABLE(void);
void RGB_PWM(int CRR);
void RGB_TO_GRB2(u8 LEDid,u8 red,u8 green,u8 blue);
extern u8 buff_RGB[RGB_num][24];
#define RGB_RED 0XFF0000
#define RGB_green 0X00FF00
#define RGB_bule 0X0000ff
#define RGB_black 0X000000
#define RGB_white 0XFFFFFF
#endif
