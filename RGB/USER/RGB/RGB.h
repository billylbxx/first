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
#define RGB_RED 0XFF0000
#define RGB_green 0X00FF00
#define RGB_bule 0X0000ff
#define RGB_black 0X000000
#define RGB_white 0XFFFFFF
#define RGB_dj 0X323232  //等比例缩小就可以调亮度
#define RGB_dK 0XC8C8C8
#endif
