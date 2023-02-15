#ifndef __LCD_H_
#define __LCD_H_

#include "io_bit.h"    

#define LCD_SCLK PBout(13)
#define LCD_MOSI PBout(15)

#define LCD_DC  PDout(2)
#define LCD_BLK PAout(15)
#define LCD_CS  PBout(4)

#define LCD_WHITE     	0xFFFF
#define LCD_BLACK     	0x0000	 
#define LCD_BLUE      	0x001F 
#define LCD_BRED       	0XF81F
#define LCD_GRED 		0XFFE0
#define LCD_GBLUE		0X07FF
#define LCD_RED      	0xF800
#define LCD_MAGENTA    	0xF81F
#define LCD_GREEN     	0x07E0
#define LCD_CYAN     	0x7FFF
#define LCD_YELLOW    	0xFFE0
#define LCD_BROWN 		0XBC40 //棕色
#define LCD_BRRED 		0XFC07 //棕红色
#define LCD_GRAY 		0X8430 //灰色
#define LCD_DARKBLUE   	0X01CF	//深蓝色
#define LCD_LIGHTBLUE   0X7D7C	//浅蓝色 
#define LCD_GRAYBLUE    0X5458 //灰蓝色
#define LCD_LIGHTGREEN  0X841F //浅绿色
#define LCD_LGRAY 		0XC618 //浅灰色(PANNEL), 窗体背景色
#define LCD_LGRAYBLUE   0XA651 //浅灰蓝色(中间层颜色)
#define LCD_LBBLUE      0X2B12 //浅棕蓝色(选择条目的反色)


void Lcd_Init(void);
void LCD_WR_REG(unsigned char dat);
void LCD_WR_DATA(unsigned short dat);
void LCD_WR_DATA8(unsigned char dat);
void LCD_Writ_Bus(unsigned char dat);

void LCD_DrawPoint(unsigned short x, unsigned short y, unsigned short color);
void LCD_Address_Set(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2);
void LCD_Fill(unsigned short xsta, unsigned short ysta, unsigned short xend, unsigned short yend, unsigned short color);
void LCD_Color_Fill(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, const unsigned short *pic);
void LCD_ShowChar(unsigned short x, unsigned short y, unsigned char num, unsigned short pin_color, unsigned short back_color, unsigned char sizey);
void LCD_ShowGB2312(unsigned char x, unsigned char y, char* ch, unsigned short pen_color, unsigned short back_color);
void LCD_ShowString(unsigned short x, unsigned short y, unsigned char *str, unsigned short pin_color, unsigned short back_color, unsigned char sizey);
void LCD_ShowIntNum(unsigned char x, unsigned char y, unsigned short num, unsigned char len, unsigned short pin_color, unsigned short back_color, unsigned char sizey);
void LCD_ShowFloatNum(unsigned char x, unsigned char y, float num, unsigned char len, unsigned short pin_color, unsigned short back_color, unsigned char sizey);
void LCD_ShowFlashChar(unsigned short x, unsigned short y, unsigned char ch, unsigned short pin_color, unsigned short back_color);
void LCD_ShowFlashGB2312(unsigned char x, unsigned char y, char* ch, unsigned short pen_color, unsigned short back_color);
void LCD_ShowFlashString(unsigned short x, unsigned short y, unsigned char *str, unsigned short pin_color, unsigned short back_color);
void LCD_ShowPic(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, const unsigned char *gimage);
void LCD_ShowFlashPic(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short sector);

#endif
