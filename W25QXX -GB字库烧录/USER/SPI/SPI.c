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
/*ͬ������ͨѶ��ͬ��ʱ���ߣ����ӻ�ʹ��ͬһ��ʱ�ӣ�����ͬ����
spi ��3�ߣ�4�ߣ�6�ߣ�����4�ߣ�
CS--NSS:Ƭѡ��ʹ�ܣ�PB4--ͨ���������
CLK--SCL:ʱ��       PB3--���츴�����   
MOSI:����������ӻ�����  PB5--���츴�����
MISO���������룬�ӻ����*/
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
	//SPI�ڲ�ģ��Ĵ�������
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	SPI_InitTypeDef SPI_InitStruct={0};
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//˫��˫��ȫ˫��
  SPI_InitStruct.SPI_Mode = SPI_Mode_Master;//��ģʽ
  SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//8λ����λ
  SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;  //���ԣ�ʱ���߿���Ϊ�ߵ�ƽ
  SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;//��λ,ģʽ3����������Ч,�ڶ������ش���
  SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;//�������Ƭѡ
  SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//81/4
  SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//��λ��ǰ
  //SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_Init(SPI3,&SPI_InitStruct);
	
	SPI_Cmd(SPI3, ENABLE);
}

void spi1_Init(void)
{
	//����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct={0};
	
	GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_AF;//���츴�����
	GPIO_InitStruct.GPIO_OType  = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin    = GPIO_Pin_7|GPIO_Pin_5;  // SCL/SDI
	GPIO_InitStruct.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_PuPd   =GPIO_PuPd_NOPULL;//������������������
	GPIO_InitStruct.GPIO_Pin    = GPIO_Pin_6;  // MISO
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1);
	
	//SPI1�ڲ�ģ��Ĵ�������--����SPI1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	
	SPI_InitTypeDef SPI_InitStruct ={0};
	SPI_InitStruct.SPI_BaudRatePrescaler  = SPI_BaudRatePrescaler_2; // 84/2=42M
	SPI_InitStruct.SPI_CPHA               = SPI_CPHA_2Edge;    //ѡ�ڶ������ش���
	SPI_InitStruct.SPI_CPOL               = SPI_CPOL_High;     //ʱ���߿���Ϊ�ߵ�ƽ��ģʽ3����������Ч
//	SPI_InitStruct.SPI_CRCPolynomial      = 7;
	SPI_InitStruct.SPI_DataSize           = SPI_DataSize_8b;   //SPI���ͽ���8λ�ṹ
	SPI_InitStruct.SPI_Direction          = SPI_Direction_2Lines_FullDuplex;//˫��˫��ȫ˫��
	SPI_InitStruct.SPI_FirstBit           = SPI_FirstBit_MSB;  //���ݴ���Ӹ�λ��ʼ
	SPI_InitStruct.SPI_Mode               = SPI_Mode_Master;   //����Ϊ��SPI
	SPI_InitStruct.SPI_NSS                = SPI_NSS_Soft;      //�������CS
	SPI_Init(SPI1,&SPI_InitStruct);
	SPI_Cmd(SPI1,ENABLE);
}







//spi��д����
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

/*���ܣ�д�����
CMD:����
LCD_CS��ʹ��0��ʧ��1
LCD_DC������0������1
*/
void LCD_WR_REG(u8 CMD)
{
	LCD_CS=0;
	LCD_DC=0;
	spi_r_w_data(CMD);
	LCD_CS=1;
	
}

/*���ܣ�д���ݺ���
CMD:����
LCD_CS��ʹ��0��ʧ��1
LCD_DC������0������1
*/
void LCD_WR_DATA8(u8 data)
{
	LCD_CS=0;
	LCD_DC=1;
	spi_r_w_data(data);
	LCD_CS=1;
}

/*���ܣ����õ�ַ
������x0��x����ʼ���ꣻx1��x��������ꣻ
      y0��y����ʼ���ꣻy1��y���������
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

/*���ܣ���㺯���������ɫ��
color:16λ��ɫ����
*/
void point_color(u16 color)
{
	LCD_CS=0;
	LCD_DC=1;
	spi_r_w_data(color>>8);
	spi_r_w_data(color);
	LCD_CS=1;
}

/*�������ܣ����*/
//void draw_point(u8 x, u8 y, u16 color)
//{
//	
//	setadd(x,y,x,y);
//	point_color(color);
//}

/*���ܣ�����
������x0��x����ʼ���ꣻx1��x��������ꣻ
      y0��y����ʼ���ꣻy1��y���������
color�������ɫ����
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
	setadd(x,y,x+sizey/2-1,y+sizey-1);//��ģ��Χ
	size=sizey/2*sizey/8; //�ֽ���
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
		setadd(x,y,x+sizey-1,y+sizey-1);//��ģ��Χ
		size=sizey*sizey/8; //�ֽ���
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
			LCD_Fill(0, 0, 240, 240, LCD_RED);//����
			x=x0-sizey;
			x0=x;
			y=y0;
			str=sp;//�����ʾ����������ʾ
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
/*PA15--ͨ���������--����--1��*/
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
	
	//LCD_Fill(0, 0, 240, 240, LCD_RED);	//����
	LCD_BLK = 1;

	
}

/********************************************************
���ܣ��ֿ⺺����ʾ
((*str-0xa1)*94+(*(str+1)-0xa1))*32  
(*str-0xa1)ǰ���ж��ٸ���
(*(str+1)-0xa1))�ж��ٸ���
********************************************************/
void LCD_FLASH_SHOW_CHINESE(u16 x,u16 y,char*str,u16 pin_color,u16 back_color,u16 sizey)
{
	int i,j;
	u8 temp,size;
	u8* sp=0;
//((*str-0xa1)*94+(*(str+1)-0xa1))*32 ��ַû�м�ƫ��������0��ַ��ʼ�洢ƫ����
		size=sizey*sizey/8; //�ֽ���
	if(sizey==16)
	{
		sp=W25QXX_read_date(((*str-0xa1)*94+(*(str+1)-0xa1))*size,size);
	}	
	else sp=W25QXX_read_date(((*str-0xa1)*94+(*(str+1)-0xa1))*size+0x0003FE42,size);
		setadd(x,y,x+sizey-1,y+sizey-1);//��ģ��Χ
	
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
���ܣ��ֿ�຺����ʾ
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
			LCD_Fill(0, 0, 240, 240, LCD_RED);//����
			x=x0-sizey;
			x0=x;
			y=y0;
			str=sp;//�����ʾ����������ʾ
		}
	}
}

