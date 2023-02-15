#ifndef __SPI_H
#define __SPI_H
#include "io_bit.h" 
#define LCD_SCLK PBout(3)
#define LCD_MOSI PBout(5)

#define LCD_DC  PDout(2)
#define LCD_BLK PAout(15)  //Һ������������ź�
#define LCD_CS  PBout(4)

#define LCD_WHITE     	0xFFFF
#define LCD_BLACK     	0x0000	 
#define LCD_BLUE      	0x001F 
#define LCD_BRED       	0XF81F
#define LCD_GRED 		    0XFFE0
#define LCD_GBLUE		    0X07FF
#define LCD_RED        	0xF800
#define LCD_MAGENTA    	0xF81F
#define LCD_GREEN     	0x07E0
#define LCD_CYAN     	  0x7FFF
#define LCD_YELLOW    	0xFFE0
#define LCD_BROWN 		  0XBC40 //��ɫ
#define LCD_BRRED 		  0XFC07 //�غ�ɫ
#define LCD_GRAY 		    0X8430 //��ɫ
#define LCD_DARKBLUE   	0X01CF	//����ɫ
#define LCD_LIGHTBLUE   0X7D7C	//ǳ��ɫ 
#define LCD_GRAYBLUE    0X5458 //����ɫ
#define LCD_LIGHTGREEN  0X841F //ǳ��ɫ
#define LCD_LGRAY 		  0XC618 //ǳ��ɫ(PANNEL), ���屳��ɫ
#define LCD_LGRAYBLUE   0XA651 //ǳ����ɫ(�м����ɫ)
#define LCD_LBBLUE      0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)
void lcd_Init(void);
void point_color(u16 color);
void setadd(u8 x0, u8 y0,u8 x1, u8 y1);
void LCD_WR_DATA8(u8 data);
void LCD_WR_REG(u8 CMD);
u8 spi_r_w_data(u8 ch);
static void spi1_Init(void);
void draw_point(u8 x, u8 y, u16 color);
void LCD_Fill(u8 x0, u8 y0,u8 x1, u8 y1, u16 color);
void LCD_showdata(u16 x,u16 y,u16 num,u16 pin_color,u16 back_color,u16 sizey);
void LCD_showstring(u16 x,u16 y,char*str,u16 pin_color,u16 back_color,u16 sizey);
void LCD_CHINESE(u16 x,u16 y,char*str,u16 pin_color,u16 back_color,u16 sizey);
void LCD_CHINESES(u16 x,u16 y,char*str,u16 pin_color,u16 back_color,u16 sizey);
void show_pic(u8 x0, u8 y0,u8 wide, u8 high,const unsigned char *pic);
extern const unsigned char gImage_qqqq[115200];
extern const unsigned char gImage_IMG00000[36000];
extern const unsigned char gImage_IMG00001[36000];
extern const unsigned char gImage_IMG00006[36000];
extern const unsigned char gImage_IMG00012[36000];
extern const unsigned char gImage_IMG00013[36000];
extern const unsigned char gImage_IMG00019[36000];
#endif
