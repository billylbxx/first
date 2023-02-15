#include "stm32f4xx.h"//SPI FLASH
#include "W25QXX.h"
#include "SPI.h"

void w25q64_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct={0};
	GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_OUT;//�������
	GPIO_InitStruct.GPIO_OType  = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin    = GPIO_Pin_7;//CS
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	flash_cs=1;
	spi1_Init();
}


u16 W25Q64_readID(void)
{
	u16 ID;
	flash_cs=0;
	spi1_r_w_data(0x90);
	spi1_r_w_data(0);
	spi1_r_w_data(0);
	spi1_r_w_data(0);
	ID=spi1_r_w_data(0xff)<<8;//0xff��αָ����Զ���Ϊ������
	ID|=spi1_r_w_data(0xff);
	flash_cs=1;
	return ID;
}

void write_ENABLE(void)
{
	flash_cs=0;
	spi1_r_w_data(0x06);
	flash_cs=1;
}

u16 w25q64_BUSY(void)
{
	u16 dat=0;
	flash_cs=0;
	spi1_r_w_data(0x05);
	dat=spi1_r_w_data(0xff);
	flash_cs=1;
	return dat;
}

void w25q64_clean_all(void)
{
	printf("clearing\r\n");
	write_ENABLE();
	flash_cs=0;
	spi1_r_w_data(0xC7);
	flash_cs=1;
	while(w25q64_BUSY()&1);//BUSYλ�ж���û�н���
	printf("finish clearing\r\n");
}

void w25q64_write_page(u32 addr,const char *sp,u32 Length)
{
	write_ENABLE();
	flash_cs=0;	
	spi1_r_w_data(0x02);
	spi1_r_w_data((u8)(addr>>16));
	spi1_r_w_data((u8)(addr>>8));
	spi1_r_w_data((u8)addr);
	for(int i=0;i<Length;i++)
	{
		spi1_r_w_data(*sp++);
	}
	flash_cs=1;
	while(w25q64_BUSY()&1);
}

u8* W25QXX_read_date(u32 addr,int len)
{
	static u8 buf[100]={0};
  flash_cs=0;
	spi1_r_w_data(0x03);
	spi1_r_w_data((u8)(addr>>16));
	spi1_r_w_data((u8)(addr>>8));
	spi1_r_w_data((u8)addr);
	for(int i=0;i<len;i++)
	{
		buf[i]=spi1_r_w_data(0xff);
	}
	flash_cs=1;
	return buf;
}

/********************************************************************
*������Ϣ ��void W25Q64_Write_NoCheck(u8 *Buffer,u32 WriteAddress,u32 Length)               
*�������� ��дLength���ֽڵ�W25Q64
*������� ��Buffer,��Ҫд�����ݻ�������
*					  WriteAddress,W25Q64�ڲ���ַ��
*						Lengthд������ݳ���
*������� ����
*��������:  ��
*������ʾ ����  
*  ����:    ������   
*  ����:   ����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��! 
					 �����Զ���ҳ���� 
*  ��д����:  2020.02.08   
*********************************************************************/	
void W25Q64_Write_NoCheck(u8 *Buffer,u32 WriteAddress,u32 Length)//�پ�������ӣ��������   
{ 			 		                            //100           //453            
	u16 pageremain;	   
	pageremain=256-WriteAddress%256;/*��ҳʣ����ֽ���*/	
	printf("pageremain=%d\r\n",pageremain);	
	if(pageremain>=Length)pageremain=Length;/*������256���ֽ�*/
	while(1)
	{	   
		w25q64_write_page(WriteAddress,(char*)Buffer,pageremain);
		printf("Length=%d\r\n",Length);
	
		if(Length==pageremain)break;/*д�������*/
	 	else /*NumByteToWrite>pageremain*/
		{
			Buffer+=pageremain;
			WriteAddress+=pageremain;	

			Length-=pageremain;			  /*��ȥ�Ѿ�д���˵��ֽ���*/
			if(Length>256)pageremain=256; /*һ�ο���д��256���ֽ�*/
			else pageremain=Length; 	  /*����256���ֽ���*/
		}
	}    
}


