#ifndef _WM8978_H_
#define _WM8978_H_
#include "stm32f4xx.h"
#include "io_bit.h"
#define WM8978_ADDR				0X1A	//WM8978的器件地址,固定为0X1A 

u8  WM8978_Init(void); 
u8  WM8978_Write_Reg(u8 reg,u16 val); 
u16 WM8978_Read_Reg(u8 reg);
void WM8978_SPKvol_Set(u8 volx);
void WM8978_DAC_Init(void);
void WM8978_Set_Vol(u8 voll,u8 volr);
void WM8978_I2S_Mode(u8 fmt,u8 len);
void Music_Init(void);
#endif

