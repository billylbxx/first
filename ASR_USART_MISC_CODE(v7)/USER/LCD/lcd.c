#include "lcd.h"
#include "font.h"
#include "systick.h"
#include "spi.h"
#include "w25qxx.h"
/*********************************************************************************************************
* �� �� �� : LCD_ShowFlashString
* ����˵�� : ��ʾ�ַ���
* ��    �� : x��y����ֹ���꣬str����ʾ���ַ�����pin_color��������ɫ��back_color��������ɫ
* �� �� ֵ : ��
* ��    ע : �ֿ������Ӣ���ǵȿ����壬���ÿ�
*********************************************************************************************************/ 
void LCD_ShowFlashString(unsigned short x, unsigned short y, unsigned char *str, unsigned short pin_color, unsigned short back_color)
{
	while(*str != 0)
	{
		if(*str < 127)		//ASCII�ɼ��ַ����Ϊ126
		{
			LCD_ShowChar(x, y, *str, pin_color, back_color, 16);			//��ʾ�ַ�
			x += 8;			//�е�ַƫ��
			str++;
			if(x > 240-16)	//����ߵ��˱���
			{
				x = 0;		//�ص�����
				y += 16;	//�е�ַƫ��
			}
		}
		else
		{
			LCD_ShowFlashGB2312(x, y, (char *)str, pin_color, back_color);	//��ʾ����
			x += 16;			//�е�ַƫ��
			str += 2;			//һ�����������ֽ�
			if(x > 240-16)		//����ߵ��˱���
			{
				x = 0;			//�ص�����
				y += 16;		//�е�ַƫ��
			}
		}
	}
}
/*********************************************************************************************************
* �� �� �� : LCD_ShowFlashGB2312
* ����˵�� : ��flash�ж�ȡ������ģ���ݲ���ʾ
* ��    �� : x��y����ֹ���꣬ch����Ҫ��ʾ�����ģ�pen_color��������ɫ��back_color��������ɫ
* �� �� ֵ : ��
* ��    ע : ��Ҫ���Ȱ���ģ����д�뵽25Q64�������ģ������Ҫ�ӵ�0��������ʼ��ţ�������Ҫ����ƫ����
*********************************************************************************************************/ 
void LCD_ShowFlashGB2312(unsigned char x, unsigned char y, char* ch, unsigned short pen_color, unsigned short back_color)
{
	unsigned char j = 0, k = 0;
	unsigned char temp[32] = {0}, c = 0;
	W25QXX_ReadDatas(temp, ((*ch-0xA1)*94+(*(ch+1)-0xA1))*32, 32);
	LCD_Address_Set(x, y, x+16-1, y+16-1);				//�ںÿ�
	for(k=0; k<32; k++)
	{
		c = temp[k];
		for(j=0; j<8; j++)								//���λ���жϣ�����ʾ��Ӧ����ɫ
		{
			if(c&0x01)	LCD_WR_DATA(pen_color);			//�����λΪ1�ͻ�ǰ��ɫ
			else 		LCD_WR_DATA(back_color);		//�����λΪ0�ͻ�����ɫ
			c >>= 1;
		}
	}
}
/*********************************************************************************************************
* �� �� �� : LCD_ShowFlashChar
* ����˵�� : ��flash�ж�ȡ�ַ���ģ���ݲ���ʾ
* ��    �� : x,y����ֹ���꣬ch��Ҫ��ʾ���ַ���pin_color��������ɫ��back_color��������ɫ
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void LCD_ShowFlashChar(unsigned short x, unsigned short y, unsigned char ch, unsigned short pin_color, unsigned short back_color)
{
	unsigned short i = 0, j = 0;
	unsigned char temp[32] = {0}, c = 0;
	W25QXX_ReadDatas(temp, ((0XA3-0XA1)*94+(ch-33))*32, 32);
	LCD_Address_Set(x, y, x+16-1, y+16-1);				//�ںÿ�
	for(i=0; i<32; i++)									//һ��16*8���ַ���16���ֽڵ��������
	{
		c = temp[i];
		for(j=0; j<8; j++)								//���λ���жϣ�����ʾ��Ӧ����ɫ
		{
			if(c&0x01)		LCD_WR_DATA(pin_color);		//�����λΪ1�ͻ�ǰ��ɫ
			else 			LCD_WR_DATA(back_color);	//�����λΪ0�ͻ�����ɫ
			c >>= 1;
		}
	}
}
/*********************************************************************************************************
* �� �� �� : LCD_ShowFlashPic
* ����˵�� : ��flash�ж�ȡͼƬȡģ���ݲ���ʾ
* ��    �� : x0��y0����ֹ���꣬x1��y1��ͼƬ�Ŀ�Ⱥ͸߶ȣ�sector��ͼƬȡģ���ݴ�ŵ��������
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void LCD_ShowFlashPic(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short sector)
{
	unsigned char temp[40] = {0};
	unsigned int i = 0, j = 0;
	unsigned int addr = sector * 4096;			//����������ַ
	LCD_Address_Set(x0, y0, x0+x1-1, y0+y1-1);	//�ںÿ�
	for(i=0; i<x1*y1/20; i++)
	{
		W25QXX_ReadDatas(temp, addr, 40);
		for(j=0; j<40; j+=2)
			LCD_WR_DATA(temp[j+1]<<8 | temp[j]);
		addr += 40;
	}
}
/*********************************************************************************************************
* �� �� �� : LCD_ShowPic
* ����˵�� : ��ʾ�洢��rom�е�ͼƬ
* ��    �� : x0��y0����ֹ���꣬x1��y1��ͼƬ�Ŀ�Ⱥ͸߶ȣ�gimage��ͼƬȡģ����ָ��
* �� �� ֵ : ��
* ��    ע : ��ָ��ΪͼƬȡģ��������ĵ�ַ
*********************************************************************************************************/ 
void LCD_ShowPic(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, const unsigned char *gimage)
{
	unsigned int i = 0;
	LCD_Address_Set(x0, y0, x0+x1-1, y0+y1-1);	//�ںÿ�
	for(i=0; i<x1*y1; i++)
	{
		LCD_WR_DATA(*(gimage+1)<<8 | *gimage);
		gimage += 2;
	}
}
/*********************************************************************************************************
* �� �� �� : LCD_ShowGB2312
* ����˵�� : ��ʾ�洢��rom�е�����
* ��    �� : x0��y0����ֹ���꣬Ҫ��ʾ�����ģ�pen_color��������ɫ��back_color��������ɫ
* �� �� ֵ : ��
* ��    ע : �߱���������
*********************************************************************************************************/ 
void LCD_ShowGB2312(unsigned char x, unsigned char y, char* ch, unsigned short pen_color, unsigned short back_color)
{
	unsigned char i = 0,j = 0,k = 0;
	unsigned char temp = 0;
	for(i=0; i<sizeof(indexes)/sizeof(indexes[0]); i++)
	{
		if(*ch == indexes[i][0] && *(ch+1) == indexes[i][1])	//�ֱ�Ƚϸ����ĵ������ֽ��Ƿ����
			break;												//�����ȫ�����ô�͵õ��˸���������ģ�����е�λ��
	}
	if(i>=sizeof(indexes)/sizeof(indexes[0]))	return;
	LCD_Address_Set(x, y, x+32-1, y+32-1);				//�ںÿ�
	for(k=0; k<128; k++)
	{
		temp = GB2312[i][k];
		for(j=0; j<8; j++)								//���λ���жϣ�����ʾ��Ӧ����ɫ
		{
			if(temp&0x01)	LCD_WR_DATA(pen_color);		//�����λΪ1�ͻ�ǰ��ɫ
			else 			LCD_WR_DATA(back_color);	//�����λΪ0�ͻ�����ɫ
			temp >>= 1;
		}
	}
}
/*********************************************************************************************************
* �� �� �� : mypow
* ����˵�� : ��m��n�η�
* ��    �� : m��������n��ָ��
* �� �� ֵ : 
* ��    ע : 
*********************************************************************************************************/ 
unsigned int mypow(unsigned char m, unsigned char n)
{
	unsigned int result = 1;	 
	while(n--) result *= m;
	return result;
}
/*********************************************************************************************************
* �� �� �� : LCD_ShowIntNum
* ����˵�� : ��ʾ����
* ��    �� : x��y����ֹ���꣬num��Ҫ��ʾ�����֣�len����ʾ��λ����pin_color��������ɫ��back_color��������ɫ��sizey�������С
* �� �� ֵ : ��
* ��    ע : ��
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
* �� �� �� : LCD_ShowFloatNum
* ����˵�� : ��ʾ����λС���ĸ�����
* ��    �� : x��y����ֹ���꣬num����ʾ�ĸ�������len����ʾ����λ����pin_color��������ɫ��back_color��������ɫ��sizey�������С
* �� �� ֵ : 
* ��    ע : 
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
* �� �� �� : LCD_ShowString
* ����˵�� : ��ʾ�ַ���
* ��    �� : x��y����ֹ���꣬str����ʾ���ַ�����pin_color��������ɫ��back_color��������ɫ��sizey�������С������δʵ�ִ�Сѡ��
* �� �� ֵ : ��
* ��    ע : ����Ӣ���ַ���˵���ֿ����ָߵ�һ��
*********************************************************************************************************/ 
void LCD_ShowString(unsigned short x, unsigned short y, unsigned char *str, unsigned short pin_color, unsigned short back_color, unsigned char sizey)
{
	while(*str != 0)
	{
		if(*str < 127)	//ASCII�ɼ��ַ����Ϊ126
		{
			LCD_ShowChar(x, y, *str, pin_color, back_color, sizey);	//��ʾ�ַ�
			x += sizey/2;		//�е�ַƫ��size/2
			str++;
			if(x > 240-sizey/2)	//����ߵ��˱���
			{
				x = 0;			//�ص�����
				y += sizey;		//�е�ַƫ��size
			}
		}
		else
		{
			LCD_ShowGB2312(x, y, (char *)str, pin_color, back_color);	//��ʾ����
			x += sizey;			//�е�ַƫ��size
			str += 2;			//һ�����������ֽ�
			if(x > 240-sizey)	//����ߵ��˱���
			{
				x = 0;			//�ص�����
				y += sizey;		//�е�ַƫ��size
			}
		}
	}
}
/**********************************************************************
*��������LCD_ShowChar
*��  �ܣ���ʾ�����ַ�
*��  ����x��y����ֹ���꣬num����ʾ���ַ���pin_color��������ɫ��back_color��������ɫ��sizey�������С��16 or 32��
*��  �أ���
*��  ע����
**********************************************************************/
void LCD_ShowChar(unsigned short x, unsigned short y, unsigned char num, unsigned short pin_color, unsigned short back_color, unsigned char sizey)
{
	unsigned short i = 0, j = 0;
	unsigned char ch = num - 32;								//��������ַ�����ģ����λ��
	unsigned char temp = 0, size = 0;

	LCD_Address_Set(x, y, x+sizey/2-1, y+16*(sizey/16)-1);		//�ںÿ�
	size = sizey/2*16*sizey/16/8;								//�õ�һ���ַ�����ģ�ֽ���

	for(i=0; i<size; i++)										//һ��16*8���ַ���16���ֽڵ��������
	{
		temp = (size==16?ascii_16_8[ch][i]:ascii_32_16[ch][i]);	//���ȡ����ģ����
		for(j=0;j<8;j++)										//���λ���жϣ�����ʾ��Ӧ����ɫ
		{
			if(temp&0x01)	LCD_WR_DATA(pin_color);				//�����λΪ1�ͻ�ǰ��ɫ
			else 			LCD_WR_DATA(back_color);			//�����λΪ0�ͻ�����ɫ
			temp >>= 1;
		}
	}
}
/*********************************************************************************************************
* �� �� �� : LCD_Fill
* ����˵�� : ������
* ��    �� : xsta��ysta����ʼ���ꣻxend��yend����Ⱥ͸߶ȣ�color��������ɫ
* �� �� ֵ : ��
* ��    ע : ��ʵ�������Σ����������㡢��ˮƽ�ߡ�����ֱ�ߵȹ��ܻ���
*********************************************************************************************************/ 
void LCD_Fill(unsigned short xsta, unsigned short ysta, unsigned short xend, unsigned short yend, unsigned short color)
{          
	unsigned int i = 0;
	LCD_Address_Set(xsta, ysta, xsta+xend-1, ysta+yend-1);	//������ʾ��Χ
	for(i=0; i<xend*yend; i++)
		LCD_WR_DATA(color);					  	    
}
void LCD_Color_Fill(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, const unsigned short *pic)
{
	unsigned int i = 0;
	LCD_Address_Set(x0, y0, x0+x1, y0+y1);	//�ںÿ�
	for(i=0; i<x1*y1; i++)
	{
		LCD_WR_DATA(pic[i]);
	}
}
/*********************************************************************************************************
* �� �� �� : LCD_DrawPoint
* ����˵�� : ����
* ��    �� : x��y����ֹ���꣬color�������ɫ
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void LCD_DrawPoint(unsigned short x, unsigned short y, unsigned short color)
{
	LCD_Address_Set(x, y, x, y);	//���ù��λ�� 
	LCD_WR_DATA(color);
} 
/*********************************************************************************************************
* �� �� �� : LCD_Writ_Bus
* ����˵�� : ���ģ��SPI����д��
* ��    �� : dat��д�������
* �� �� ֵ : ��
* ��    ע : ��
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
* �� �� �� : LCD_WR_DATA8
* ����˵�� : д��8λ����
* ��    �� : dat��д�������
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void LCD_WR_DATA8(unsigned char dat)
{
	LCD_CS = 0;
	Spi3_WriteReadByte(dat);
	LCD_CS = 1;
}
/*********************************************************************************************************
* �� �� �� : LCD_WR_DATA
* ����˵�� : д��16λ����
* ��    �� : dat��д�������
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void LCD_WR_DATA(unsigned short dat)
{
	LCD_CS = 0;
	Spi3_WriteReadByte(dat>>8);
	Spi3_WriteReadByte(dat);
	LCD_CS = 1;
}
/*********************************************************************************************************
* �� �� �� : LCD_WR_REGд������
* ����˵�� : д������
* ��    �� : dat��д�������
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void LCD_WR_REG(unsigned char dat)
{
	LCD_CS = 0;
	LCD_DC = 0;		//д����
	Spi3_WriteReadByte(dat);
	LCD_DC = 1;		//д���ݣ�д�����Ĭ�ϻص�д���ݵ�״̬
	LCD_CS = 1;
}
/*********************************************************************************************************
* �� �� �� : LCD_Address_Set
* ����˵�� : ���ù��λ��
* ��    �� : x1��y1����ֹ���꣬x2��y2����ֹ����
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void LCD_Address_Set(unsigned short x1,unsigned short y1,unsigned short x2,unsigned short y2)
{
	LCD_WR_REG(0x2a);	//�е�ַ����
	LCD_WR_DATA(x1);
	LCD_WR_DATA(x2);
	LCD_WR_REG(0x2b);	//�е�ַ����
	LCD_WR_DATA(y1);
	LCD_WR_DATA(y2);
	LCD_WR_REG(0x2c);	//������д
}

void Lcd_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure = {0};

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;		//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//50MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//����
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//��ʼ�� BLK
	GPIO_ResetBits(GPIOA, GPIO_Pin_15);					//Ĭ�Ϲرձ���
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
	GPIO_Init(GPIOB, &GPIO_InitStructure);				//��ʼ�� CS		
	GPIO_SetBits(GPIOB, GPIO_Pin_4);					//Ĭ�ϲ�ѡ������
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);				//��ʼ�� DC		
	GPIO_SetBits(GPIOD, GPIO_Pin_2);					//Ĭ������ģʽ
	
	Spi3_Init();										//Ӳ��spi3��ʼ��
	
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
	
	LCD_BLK = 1;							//�򿪱���
	LCD_Fill(0, 0, 240, 240, LCD_WHITE);	//����
}
