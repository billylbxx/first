#ifndef __WM8978_H
#define __WM8978_H

#include "io_bit.h"    									
 
#define WM8978_ADDR				0X1A	//WM8978的器件地址,固定为0X1A 
 
#define EQ1_80Hz		0X00
#define EQ1_105Hz		0X01
#define EQ1_135Hz		0X02
#define EQ1_175Hz		0X03

#define EQ2_230Hz		0X00
#define EQ2_300Hz		0X01
#define EQ2_385Hz		0X02
#define EQ2_500Hz		0X03

#define EQ3_650Hz		0X00
#define EQ3_850Hz		0X01
#define EQ3_1100Hz		0X02
#define EQ3_14000Hz		0X03

#define EQ4_1800Hz		0X00
#define EQ4_2400Hz		0X01
#define EQ4_3200Hz		0X02
#define EQ4_4100Hz		0X03

#define EQ5_5300Hz		0X00
#define EQ5_6900Hz		0X01
#define EQ5_9000Hz		0X02
#define EQ5_11700Hz		0X03

  
unsigned char WM8978_Init(void); 
void WM8978_ADDA_Cfg(unsigned char dacen,unsigned char adcen);
void WM8978_Input_Cfg(unsigned char micen,unsigned char lineinen,unsigned char auxen);
void WM8978_Output_Cfg(unsigned char dacen,unsigned char bpsen);
void WM8978_MIC_Gain(unsigned char gain);
void WM8978_LINEIN_Gain(unsigned char gain);
void WM8978_AUX_Gain(unsigned char gain);
unsigned char WM8978_Write_Reg(unsigned char reg,unsigned short val); 
unsigned short WM8978_Read_Reg(unsigned char reg);
void WM8978_HPvol_Set(unsigned char voll,unsigned char volr);
void WM8978_SPKvol_Set(unsigned char volx);
void WM8978_I2S_Cfg(unsigned char fmt,unsigned char len);
void WM8978_3D_Set(unsigned char depth);
void WM8978_EQ_3D_Dir(unsigned char dir); 
void WM8978_EQ1_Set(unsigned char cfreq,unsigned char gain); 
void WM8978_EQ2_Set(unsigned char cfreq,unsigned char gain);
void WM8978_EQ3_Set(unsigned char cfreq,unsigned char gain);
void WM8978_EQ4_Set(unsigned char cfreq,unsigned char gain);
void WM8978_EQ5_Set(unsigned char cfreq,unsigned char gain);

#endif





















