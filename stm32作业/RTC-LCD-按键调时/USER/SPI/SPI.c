#include "SPI.h"
#include "font.h"
#include "stm32f4xx.h"
#include "systick.h"
#include "W25QXX.h"
#include "qqqq.h"
#include "IMG00000.h"
#include "IMG00001.h"
#include "IMG00006.h"
#include "IMG00012.h"
#include "IMG00013.h"
#include "IMG00019.h"
/*同步串行通讯，同步时钟线（主从机使用同一个时钟，用来同步）
spi 有3线，4线，6线（这里4线）
CS--NSS:片选（使能）PB4--通用推挽输出
CLK--SCL:时钟       PB3--推挽复用输出   
MOSI:主机输出，从机输入  PB5--推挽复用输出
MISO：主机输入，从机输出*/
void spi3_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_4;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_3|GPIO_Pin_5;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI3);
	//SPI内部模块寄存器配置
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	SPI_InitTypeDef SPI_InitStruct={0};
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//双线双向全双工
  SPI_InitStruct.SPI_Mode = SPI_Mode_Master;//主模式
  SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//8位数据位
  SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;  //极性，时钟线空闲为高电平
  SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;//相位,模式3，上升沿有效,第二个边沿触发
  SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;//软件管理片选
  SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//81/4
  SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//高位在前
  //SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_Init(SPI3,&SPI_InitStruct);
	
	SPI_Cmd(SPI3, ENABLE);
}

void spi1_Init(void)
{
	//引脚
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct={0};
	
	GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_AF;//推挽复用输出
	GPIO_InitStruct.GPIO_OType  = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin    = GPIO_Pin_7|GPIO_Pin_5;  // SCL/SDI
	GPIO_InitStruct.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_PuPd   =GPIO_PuPd_NOPULL;//浮空输入或带上拉输入
	GPIO_InitStruct.GPIO_Pin    = GPIO_Pin_6;  // MISO
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1);
	
	//SPI1内部模块寄存器配置--四线SPI1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	
	SPI_InitTypeDef SPI_InitStruct ={0};
	SPI_InitStruct.SPI_BaudRatePrescaler  = SPI_BaudRatePrescaler_2; // 84/2=42M
	SPI_InitStruct.SPI_CPHA               = SPI_CPHA_2Edge;    //选第二个边沿触发
	SPI_InitStruct.SPI_CPOL               = SPI_CPOL_High;     //时钟线空闲为高电平。模式3，上升沿有效
//	SPI_InitStruct.SPI_CRCPolynomial      = 7;
	SPI_InitStruct.SPI_DataSize           = SPI_DataSize_8b;   //SPI发送接收8位结构
	SPI_InitStruct.SPI_Direction          = SPI_Direction_2Lines_FullDuplex;//双线双向全双工
	SPI_InitStruct.SPI_FirstBit           = SPI_FirstBit_MSB;  //数据传输从高位开始
	SPI_InitStruct.SPI_Mode               = SPI_Mode_Master;   //设置为主SPI
	SPI_InitStruct.SPI_NSS                = SPI_NSS_Soft;      //软件管理CS
	SPI_Init(SPI1,&SPI_InitStruct);
	SPI_Cmd(SPI1,ENABLE);
}







//spi读写函数
u8 spi_r_w_data(u8 ch)
{
	u8 DAT;
	//LCD_CS=0;
	while(SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_TXE)==0);
	SPI_I2S_SendData(SPI3,ch);
	while(SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_RXNE)==0);
	DAT=SPI_I2S_ReceiveData(SPI3);
	//LCD_CS=1;
	return DAT;
}

u8 spi1_r_w_data(u8 ch)
{
	u8 DAT;
	//LCD_CS=0;
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==0);
	SPI_I2S_SendData(SPI1,ch);
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==0);
	DAT=SPI_I2S_ReceiveData(SPI1);
	//LCD_CS=1;
	return DAT;
}

/*功能：写命令函数
CMD:命令
LCD_CS：使能0，失能1
LCD_DC：命令0，数据1
*/
void LCD_WR_REG(u8 CMD)
{
	LCD_CS=0;
	LCD_DC=0;
	spi_r_w_data(CMD);
	LCD_CS=1;
	
}

/*功能：写数据函数
CMD:命令
LCD_CS：使能0，失能1
LCD_DC：命令0，数据1
*/
void LCD_WR_DATA8(u8 data)
{
	LCD_CS=0;
	LCD_DC=1;
	spi_r_w_data(data);
	LCD_CS=1;
}

/*功能：设置地址
参数：x0：x轴起始坐标；x1：x轴结束坐标；
      y0：y轴起始坐标；y1：y轴结束坐标
*/
void setadd(u8 x0,u8 y0,u8 x1,u8 y1)
{
	LCD_WR_REG(0x2a);
	LCD_WR_DATA8(x0>>8);
	LCD_WR_DATA8(x0);
	LCD_WR_DATA8(x1>>8);
	LCD_WR_DATA8(x1);
	LCD_WR_REG(0x2b);
	LCD_WR_DATA8(y0>>8);
	LCD_WR_DATA8(y0);
	LCD_WR_DATA8(y1>>8);
	LCD_WR_DATA8(y1);
	LCD_WR_REG(0x2c);
}

/*功能：打点函数（点的颜色）
color:16位颜色数据
*/
void point_color(u16 color)
{
	LCD_CS=0;
	LCD_DC=1;
	spi_r_w_data(color>>8);
	spi_r_w_data(color);
	LCD_CS=1;
}

/*函数功能：打点*/
//void draw_point(u8 x, u8 y, u16 color)
//{
//	
//	setadd(x,y,x,y);
//	point_color(color);
//}

/*功能：清屏
参数：x0：x轴起始坐标；x1：x轴结束坐标；
      y0：y轴起始坐标；y1：y轴结束坐标
color：填充颜色数据
*/
void LCD_Fill(u8 x0, u8 y0,u8 x1, u8 y1, u16 color)
{
	int i;
	setadd(x0,y0,x1,y1);
	for(i=0;i<x1*y1;i++)
	point_color(color);
}

void LCD_showdata(u16 x,u16 y,u16 num,u16 pin_color,u16 back_color,u16 sizey)
{
	u16 i=0,j=0;
	u16 ch=num-32;
	u8 temp=0,size=0;
	setadd(x,y,x+sizey/2-1,y+sizey-1);//字模框范围
	size=sizey/2*sizey/8; //字节数
	for(i=0;i<size;i++)
	{
		temp=(size==16?ascii_16_8[ch][i]:ascii_32_16[ch][i]);
		for(j=0;j<8;j++)
		{
			if(temp&1) point_color(pin_color);
			else point_color(back_color);
			temp>>=1;
		}
	}
}

void LCD_showstring(u16 x,u16 y,char*str,u16 pin_color,u16 back_color,u16 sizey)
{
  u16 x0=x;
	while(*str)
	{
	  LCD_showdata(x,y,*str,pin_color,back_color,sizey);
		x=x+sizey/2;
		if(x>(240-sizey))
		{
		  x=x0;
			y=y+sizey;
		}
		str++;
	}
}

void LCD_CHINESE(u16 x,u16 y,char*str,u16 pin_color,u16 back_color,u16 sizey)
{
	int i,j;
	int flag=-1;
	u8 temp,size=0;
	for(i=0;i<sizeof(indexes)/sizeof(indexes[0]);i++)
	  if(*str==indexes[i][0]&&*(str+1)==indexes[i][1])
			flag=i;
		if(flag==-1) return;
		setadd(x,y,x+sizey-1,y+sizey-1);//字模框范围
		size=sizey*sizey/8; //字节数
		for(i=0;i<size;i++)
		{
			temp=GB2312[flag][i];
			for(j=0;j<8;j++)
			{
				if(temp&1) point_color(pin_color);
				else point_color(back_color);
				temp>>=1;
			}
		}
}

void LCD_CHINESES(u16 x,u16 y,char*str,u16 pin_color,u16 back_color,u16 sizey)
{	
	u16 x0=x,y0=y;
	char* sp=str;
	while(*str)
	{
	  if(*str<=127)
		{
			LCD_showdata(x,y,*str,pin_color,back_color,sizey);
			x=x+sizey/2;
			if(x>(240-sizey/2))
			{
				x=x0;
				y=y+sizey;
			}
			str++;
		}
		else
		{
			LCD_CHINESE(x,y,str,pin_color,back_color,sizey);
			x=x+sizey;
			if(x>(240-sizey))
			{
				x=x0;
				y=y+sizey;
			}
			str+=2;
		}
		if(y>240-sizey)
		{
			Delay_ms(500);
			LCD_Fill(0, 0, 240, 240, LCD_RED);//清屏
			x=x0-sizey;
			x0=x;
			y=y0;
			str=sp;//如果显示不完重新显示
		}
	}
}

void show_pic(u8 x0, u8 y0,u8 wide, u8 high,const unsigned char* pic)
{
	int i;
	setadd(x0,y0,x0+wide-1,y0+high-1);
	for(i=0;i<wide*high;i++)
	{
		point_color(*(pic+1)<<8|*pic);
		pic+= 2;
	}
}

void lcd_Init(void)
{
/*PA15--通用推挽输出--背光--1亮*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_15;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_2;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOD,&GPIO_InitStruct);
	spi3_Init();
	
	Delay_ms(100);
	
	//************* Start Initial Sequence **********//
	LCD_WR_REG(0x11); 			//Sleep out 
	Delay_ms(120);              //Delay 120ms 
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
	
	//LCD_Fill(0, 0, 240, 240, LCD_RED);	//清屏
	LCD_BLK = 1;

	
}

/********************************************************
功能：字库汉字显示
((*str-0xa1)*94+(*(str+1)-0xa1))*32  
(*str-0xa1)前面有多少个区
(*(str+1)-0xa1))有多少个字
********************************************************/
void LCD_FLASH_SHOW_CHINESE(u16 x,u16 y,char*str,u16 pin_color,u16 back_color,u16 sizey)
{
	int i,j;
	u8 temp,size;
	u8* sp=0;
//((*str-0xa1)*94+(*(str+1)-0xa1))*32 地址没有加偏移量，从0地址开始存储偏移量
		size=sizey*sizey/8; //字节数
	if(sizey==16)
	{
		sp=W25QXX_read_date(((*str-0xa1)*94+(*(str+1)-0xa1))*size,size);
	}	
	else sp=W25QXX_read_date(((*str-0xa1)*94+(*(str+1)-0xa1))*size+0x0003FE42,size);
		setadd(x,y,x+sizey-1,y+sizey-1);//字模框范围
	
	for(i=0;i<size;i++)
	{
		temp=sp[i];
		for(j=0;j<8;j++)
		{
			if(temp&1) point_color(pin_color);
			else point_color(back_color);
			temp>>=1;
		}
	}
}

/*********************************************
功能：字库多汉字显示
*********************************************/
void LCD_FLASH_SHOW_CHINESE_STR(u16 x,u16 y,char*str,u16 pin_color,u16 back_color,u16 sizey)
{
	u16 x0=x,y0=y;
	char* sp=str;
	while(*str)
	{  
		if(*str<=127)
		{
			LCD_showdata(x,y,*str,pin_color,back_color,sizey);
			x=x+sizey/2;
			if(x>=(240-sizey/2))
			{
				x=x0;
				y=y+sizey;
			}
			str++;
		}
		else
		{
			LCD_FLASH_SHOW_CHINESE(x,y,str,pin_color,back_color,sizey);
		  x=x+sizey;
			if(x>=(240-sizey))
			{
				x=x0;
				y=y+sizey;
			}
			str+=2;
		}
		if(y>240-sizey)
		{
			Delay_ms(500);
			LCD_Fill(0, 0, 240, 240, LCD_RED);//清屏
			x=x0-sizey;
			x0=x;
			y=y0;
			str=sp;//如果显示不完重新显示
		}
	}
}

