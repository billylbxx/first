#include "lcd.h"
#include "font.h"
#include "systick.h"
#include "spi.h"
#include "w25qxx.h"
/*********************************************************************************************************
* 函 数 名 : LCD_ShowFlashString
* 功能说明 : 显示字符串
* 形    参 : x、y：起止坐标，str：显示的字符串，pin_color：画笔颜色，back_color：背景颜色
* 返 回 值 : 无
* 备    注 : 字库里面的英文是等宽字体，不好看
*********************************************************************************************************/ 
void LCD_ShowFlashString(unsigned short x, unsigned short y, unsigned char *str, unsigned short pin_color, unsigned short back_color)
{
	while(*str != 0)
	{
		if(*str < 127)		//ASCII可见字符最大为126
		{
			LCD_ShowChar(x, y, *str, pin_color, back_color, 16);			//显示字符
			x += 8;			//列地址偏移
			str++;
			if(x > 240-16)	//如果走到了边沿
			{
				x = 0;		//回到首列
				y += 16;	//行地址偏移
			}
		}
		else
		{
			LCD_ShowFlashGB2312(x, y, (char *)str, pin_color, back_color);	//显示中文
			x += 16;			//列地址偏移
			str += 2;			//一个中文两个字节
			if(x > 240-16)		//如果走到了边沿
			{
				x = 0;			//回到首列
				y += 16;		//行地址偏移
			}
		}
	}
}
/*********************************************************************************************************
* 函 数 名 : LCD_ShowFlashGB2312
* 功能说明 : 从flash中读取中文字模数据并显示
* 形    参 : x、y：起止坐标，ch：需要显示的中文，pen_color：画笔颜色，back_color：背景颜色
* 返 回 值 : 无
* 备    注 : 需要事先把字模数据写入到25Q64里，并且字模数组需要从第0个扇区开始存放，否则需要加上偏移量
*********************************************************************************************************/ 
void LCD_ShowFlashGB2312(unsigned char x, unsigned char y, char* ch, unsigned short pen_color, unsigned short back_color)
{
	unsigned char j = 0, k = 0;
	unsigned char temp[32] = {0}, c = 0;
	W25QXX_ReadDatas(temp, ((*ch-0xA1)*94+(*(ch+1)-0xA1))*32, 32);
	LCD_Address_Set(x, y, x+16-1, y+16-1);				//挖好坑
	for(k=0; k<32; k++)
	{
		c = temp[k];
		for(j=0; j<8; j++)								//逐个位的判断，并显示对应的颜色
		{
			if(c&0x01)	LCD_WR_DATA(pen_color);			//如果该位为1就画前景色
			else 		LCD_WR_DATA(back_color);		//如果该位为0就画背景色
			c >>= 1;
		}
	}
}
/*********************************************************************************************************
* 函 数 名 : LCD_ShowFlashChar
* 功能说明 : 从flash中读取字符字模数据并显示
* 形    参 : x,y：起止坐标，ch：要显示的字符，pin_color：画笔颜色，back_color：背景颜色
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void LCD_ShowFlashChar(unsigned short x, unsigned short y, unsigned char ch, unsigned short pin_color, unsigned short back_color)
{
	unsigned short i = 0, j = 0;
	unsigned char temp[32] = {0}, c = 0;
	W25QXX_ReadDatas(temp, ((0XA3-0XA1)*94+(ch-33))*32, 32);
	LCD_Address_Set(x, y, x+16-1, y+16-1);				//挖好坑
	for(i=0; i<32; i++)									//一个16*8的字符由16个字节的数据组成
	{
		c = temp[i];
		for(j=0; j<8; j++)								//逐个位的判断，并显示对应的颜色
		{
			if(c&0x01)		LCD_WR_DATA(pin_color);		//如果该位为1就画前景色
			else 			LCD_WR_DATA(back_color);	//如果该位为0就画背景色
			c >>= 1;
		}
	}
}
/*********************************************************************************************************
* 函 数 名 : LCD_ShowFlashPic
* 功能说明 : 从flash中读取图片取模数据并显示
* 形    参 : x0、y0：起止坐标，x1、y1：图片的宽度和高度，sector：图片取模数据存放的扇区编号
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void LCD_ShowFlashPic(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short sector)
{
	unsigned char temp[40] = {0};
	unsigned int i = 0, j = 0;
	unsigned int addr = sector * 4096;			//计算扇区地址
	LCD_Address_Set(x0, y0, x0+x1-1, y0+y1-1);	//挖好坑
	for(i=0; i<x1*y1/20; i++)
	{
		W25QXX_ReadDatas(temp, addr, 40);
		for(j=0; j<40; j+=2)
			LCD_WR_DATA(temp[j+1]<<8 | temp[j]);
		addr += 40;
	}
}
/*********************************************************************************************************
* 函 数 名 : LCD_ShowPic
* 功能说明 : 显示存储在rom中的图片
* 形    参 : x0、y0：起止坐标，x1、y1：图片的宽度和高度，gimage：图片取模数据指针
* 返 回 值 : 无
* 备    注 : 该指针为图片取模数据数组的地址
*********************************************************************************************************/ 
void LCD_ShowPic(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, const unsigned char *gimage)
{
	unsigned int i = 0;
	LCD_Address_Set(x0, y0, x0+x1-1, y0+y1-1);	//挖好坑
	for(i=0; i<x1*y1; i++)
	{
		LCD_WR_DATA(*(gimage+1)<<8 | *gimage);
		gimage += 2;
	}
}
/*********************************************************************************************************
* 函 数 名 : LCD_ShowGB2312
* 功能说明 : 显示存储在rom中的中文
* 形    参 : x0、y0：起止坐标，要显示的中文，pen_color：画笔颜色，back_color：背景颜色
* 返 回 值 : 无
* 备    注 : 具备索引功能
*********************************************************************************************************/ 
void LCD_ShowGB2312(unsigned char x, unsigned char y, char* ch, unsigned short pen_color, unsigned short back_color)
{
	unsigned char i = 0,j = 0,k = 0;
	unsigned char temp = 0;
	for(i=0; i<sizeof(indexes)/sizeof(indexes[0]); i++)
	{
		if(*ch == indexes[i][0] && *(ch+1) == indexes[i][1])	//分别比较该中文的两个字节是否相等
			break;												//如果完全相等那么就得到了该中文在字模数组中的位置
	}
	if(i>=sizeof(indexes)/sizeof(indexes[0]))	return;
	LCD_Address_Set(x, y, x+32-1, y+32-1);				//挖好坑
	for(k=0; k<128; k++)
	{
		temp = GB2312[i][k];
		for(j=0; j<8; j++)								//逐个位的判断，并显示对应的颜色
		{
			if(temp&0x01)	LCD_WR_DATA(pen_color);		//如果该位为1就画前景色
			else 			LCD_WR_DATA(back_color);	//如果该位为0就画背景色
			temp >>= 1;
		}
	}
}
/*********************************************************************************************************
* 函 数 名 : mypow
* 功能说明 : 求m的n次方
* 形    参 : m：底数，n：指数
* 返 回 值 : 
* 备    注 : 
*********************************************************************************************************/ 
unsigned int mypow(unsigned char m, unsigned char n)
{
	unsigned int result = 1;	 
	while(n--) result *= m;
	return result;
}
/*********************************************************************************************************
* 函 数 名 : LCD_ShowIntNum
* 功能说明 : 显示数字
* 形    参 : x、y：起止坐标，num：要显示的数字，len：显示的位数，pin_color：画笔颜色，back_color：背景颜色，sizey：字体大小
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void LCD_ShowIntNum(unsigned char x, unsigned char y, unsigned short num, unsigned char len, unsigned short pin_color, unsigned short back_color, unsigned char sizey)
{         	
	unsigned char t = 0, temp = 0;
	unsigned char enshow = 0;
	unsigned char sizex = sizey/2;
	for(t=0; t<len; t++)
	{
		temp = (num/mypow(10,len-t-1))%10;
		if(enshow==0 && t<(len-1))
		{
			if(temp == 0)
			{
				LCD_ShowChar(x+t*sizex, y, ' ', pin_color, back_color, sizey);
				continue;
			}else enshow = 1;  
		}
	 	LCD_ShowChar(x+t*sizex, y, temp+48, pin_color, back_color, sizey);
	}
}
/*********************************************************************************************************
* 函 数 名 : LCD_ShowFloatNum
* 功能说明 : 显示带两位小数的浮点数
* 形    参 : x、y：起止坐标，num：显示的浮点数，len：显示的总位数，pin_color：画笔颜色，back_color：背景颜色，sizey：字体大小
* 返 回 值 : 
* 备    注 : 
*********************************************************************************************************/ 
void LCD_ShowFloatNum(unsigned char x, unsigned char y, float num, unsigned char len, unsigned short pin_color, unsigned short back_color, unsigned char sizey)
{         	
	unsigned char t = 0, temp = 0, sizex = 0;
	unsigned short num1 = 0;
	sizex = sizey/2;
	num1 = num*100;
	for(t=0; t<len; t++)
	{
		temp = (num1/mypow(10,len-t-1))%10;
		if(t == (len-2))
		{
			LCD_ShowChar(x+(len-2)*sizex, y, '.', pin_color, back_color, sizey);
			t++;
			len += 1;
		}
	 	LCD_ShowChar(x+t*sizex, y, temp+48, pin_color, back_color, sizey);
	}
}
/*********************************************************************************************************
* 函 数 名 : LCD_ShowString
* 功能说明 : 显示字符串
* 形    参 : x、y：起止坐标，str：显示的字符串，pin_color：画笔颜色，back_color：背景颜色，sizey：字体大小（中文未实现大小选择）
* 返 回 值 : 无
* 备    注 : 对于英文字符来说，字宽是字高的一半
*********************************************************************************************************/ 
void LCD_ShowString(unsigned short x, unsigned short y, unsigned char *str, unsigned short pin_color, unsigned short back_color, unsigned char sizey)
{
	while(*str != 0)
	{
		if(*str < 127)	//ASCII可见字符最大为126
		{
			LCD_ShowChar(x, y, *str, pin_color, back_color, sizey);	//显示字符
			x += sizey/2;		//列地址偏移size/2
			str++;
			if(x > 240-sizey/2)	//如果走到了边沿
			{
				x = 0;			//回到首列
				y += sizey;		//行地址偏移size
			}
		}
		else
		{
			LCD_ShowGB2312(x, y, (char *)str, pin_color, back_color);	//显示中文
			x += sizey;			//列地址偏移size
			str += 2;			//一个中文两个字节
			if(x > 240-sizey)	//如果走到了边沿
			{
				x = 0;			//回到首列
				y += sizey;		//行地址偏移size
			}
		}
	}
}
/**********************************************************************
*函数名：LCD_ShowChar
*功  能：显示单个字符
*参  数：x、y：起止坐标，num：显示的字符，pin_color：画笔颜色，back_color：背景颜色，sizey：字体大小（16 or 32）
*返  回：无
*备  注：无
**********************************************************************/
void LCD_ShowChar(unsigned short x, unsigned short y, unsigned char num, unsigned short pin_color, unsigned short back_color, unsigned char sizey)
{
	unsigned short i = 0, j = 0;
	unsigned char ch = num - 32;								//计算出该字符的字模数据位置
	unsigned char temp = 0, size = 0;

	LCD_Address_Set(x, y, x+sizey/2-1, y+16*(sizey/16)-1);		//挖好坑
	size = sizey/2*16*sizey/16/8;								//得到一个字符的字模字节数

	for(i=0; i<size; i++)										//一个16*8的字符由16个字节的数据组成
	{
		temp = (size==16?ascii_16_8[ch][i]:ascii_32_16[ch][i]);	//逐个取出字模数据
		for(j=0;j<8;j++)										//逐个位的判断，并显示对应的颜色
		{
			if(temp&0x01)	LCD_WR_DATA(pin_color);				//如果该位为1就画前景色
			else 			LCD_WR_DATA(back_color);			//如果该位为0就画背景色
			temp >>= 1;
		}
	}
}
/*********************************************************************************************************
* 函 数 名 : LCD_Fill
* 功能说明 : 填充矩形
* 形    参 : xsta、ysta：起始坐标；xend、yend：宽度和高度，color：填充的颜色
* 返 回 值 : 无
* 备    注 : 可实现填充矩形，清屏，画点、画水平线、画垂直线等功能绘制
*********************************************************************************************************/ 
void LCD_Fill(unsigned short xsta, unsigned short ysta, unsigned short xend, unsigned short yend, unsigned short color)
{          
	unsigned int i = 0;
	LCD_Address_Set(xsta, ysta, xsta+xend-1, ysta+yend-1);	//设置显示范围
	for(i=0; i<xend*yend; i++)
		LCD_WR_DATA(color);					  	    
}
void LCD_Color_Fill(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, const unsigned short *pic)
{
	unsigned int i = 0;
	LCD_Address_Set(x0, y0, x0+x1, y0+y1);	//挖好坑
	for(i=0; i<x1*y1; i++)
	{
		LCD_WR_DATA(pic[i]);
	}
}
/*********************************************************************************************************
* 函 数 名 : LCD_DrawPoint
* 功能说明 : 画点
* 形    参 : x、y：起止坐标，color：点的颜色
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void LCD_DrawPoint(unsigned short x, unsigned short y, unsigned short color)
{
	LCD_Address_Set(x, y, x, y);	//设置光标位置 
	LCD_WR_DATA(color);
} 
/*********************************************************************************************************
* 函 数 名 : LCD_Writ_Bus
* 功能说明 : 软件模拟SPI数据写入
* 形    参 : dat：写入的数据
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void LCD_Writ_Bus(unsigned char dat) 
{	
	unsigned char i = 0;
	for(i=0; i<8; i++)
	{			  
		LCD_SCLK = 0;
		LCD_MOSI = dat&0x80?1:0;
		LCD_SCLK = 1;
		dat<<=1;
	}	
}
/*********************************************************************************************************
* 函 数 名 : LCD_WR_DATA8
* 功能说明 : 写入8位数据
* 形    参 : dat：写入的数据
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void LCD_WR_DATA8(unsigned char dat)
{
	LCD_CS = 0;
	Spi3_WriteReadByte(dat);
	LCD_CS = 1;
}
/*********************************************************************************************************
* 函 数 名 : LCD_WR_DATA
* 功能说明 : 写入16位数据
* 形    参 : dat：写入的数据
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void LCD_WR_DATA(unsigned short dat)
{
	LCD_CS = 0;
	Spi3_WriteReadByte(dat>>8);
	Spi3_WriteReadByte(dat);
	LCD_CS = 1;
}
/*********************************************************************************************************
* 函 数 名 : LCD_WR_REG写入命令
* 功能说明 : 写入命令
* 形    参 : dat：写入的命令
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void LCD_WR_REG(unsigned char dat)
{
	LCD_CS = 0;
	LCD_DC = 0;		//写命令
	Spi3_WriteReadByte(dat);
	LCD_DC = 1;		//写数据，写完命令，默认回到写数据的状态
	LCD_CS = 1;
}
/*********************************************************************************************************
* 函 数 名 : LCD_Address_Set
* 功能说明 : 设置光标位置
* 形    参 : x1、y1：起止坐标，x2、y2：终止坐标
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void LCD_Address_Set(unsigned short x1,unsigned short y1,unsigned short x2,unsigned short y2)
{
	LCD_WR_REG(0x2a);	//列地址设置
	LCD_WR_DATA(x1);
	LCD_WR_DATA(x2);
	LCD_WR_REG(0x2b);	//行地址设置
	LCD_WR_DATA(y1);
	LCD_WR_DATA(y2);
	LCD_WR_REG(0x2c);	//储存器写
}

void Lcd_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure = {0};

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;		//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//50MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//浮空
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//初始化 BLK
	GPIO_ResetBits(GPIOA, GPIO_Pin_15);					//默认关闭背光
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
	GPIO_Init(GPIOB, &GPIO_InitStructure);				//初始化 CS		
	GPIO_SetBits(GPIOB, GPIO_Pin_4);					//默认不选中器件
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);				//初始化 DC		
	GPIO_SetBits(GPIOD, GPIO_Pin_2);					//默认数据模式
	
	Spi3_Init();										//硬件spi3初始化
	
	Delay_Ms(100);
	
	//************* Start Initial Sequence **********//
	LCD_WR_REG(0x11); 			//Sleep out 
	Delay_Ms(120);              //Delay 120ms 
	//************* Start Initial Sequence **********// 
	LCD_WR_REG(0x36);
	LCD_WR_DATA8(0x00);

	LCD_WR_REG(0x3A); 
	LCD_WR_DATA8(0x05);

	LCD_WR_REG(0xB2);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x33); 

	LCD_WR_REG(0xB7); 
	LCD_WR_DATA8(0x35);  

	LCD_WR_REG(0xBB);
	LCD_WR_DATA8(0x19);

	LCD_WR_REG(0xC0);
	LCD_WR_DATA8(0x2C);

	LCD_WR_REG(0xC2);
	LCD_WR_DATA8(0x01);

	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x12);   

	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x20);  

	LCD_WR_REG(0xC6); 
	LCD_WR_DATA8(0x0F);    

	LCD_WR_REG(0xD0); 
	LCD_WR_DATA8(0xA4);
	LCD_WR_DATA8(0xA1);

	LCD_WR_REG(0xE0);
	LCD_WR_DATA8(0xD0);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x11);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x2B);
	LCD_WR_DATA8(0x3F);
	LCD_WR_DATA8(0x54);
	LCD_WR_DATA8(0x4C);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x0B);
	LCD_WR_DATA8(0x1F);
	LCD_WR_DATA8(0x23);

	LCD_WR_REG(0xE1);
	LCD_WR_DATA8(0xD0);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x11);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x2C);
	LCD_WR_DATA8(0x3F);
	LCD_WR_DATA8(0x44);
	LCD_WR_DATA8(0x51);
	LCD_WR_DATA8(0x2F);
	LCD_WR_DATA8(0x1F);
	LCD_WR_DATA8(0x1F);
	LCD_WR_DATA8(0x20);
	LCD_WR_DATA8(0x23);
	LCD_WR_REG(0x21); 

	LCD_WR_REG(0x29); 
	
	LCD_BLK = 1;							//打开背光
	LCD_Fill(0, 0, 240, 240, LCD_WHITE);	//清屏
}
