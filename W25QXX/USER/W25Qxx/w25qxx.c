#include "stm32f4xx.h"//SPI FLASH
#include "W25QXX.h"
#include "SPI.h"

void w25q64_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct={0};
	GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_OUT;//推挽输出
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
	ID=spi1_r_w_data(0xff)<<8;//0xff是伪指令，可以定义为其他的
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
	while(w25q64_BUSY()&1);//BUSY位判断有没有结束
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
*函数信息 ：void W25Q64_Write_NoCheck(u8 *Buffer,u32 WriteAddress,u32 Length)               
*功能描述 ：写Length个字节到W25Q64
*输入参数 ：Buffer,需要写入数据缓冲区；
*					  WriteAddress,W25Q64内部地址；
*						Length写入的数据长度
*输出参数 ：无
*函数返回:  无
*调用提示 ：无  
*  作者:    陈醒醒   
*  其他:   必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败! 
					 具有自动换页功能 
*  编写日期:  2020.02.08   
*********************************************************************/	
void W25Q64_Write_NoCheck(u8 *Buffer,u32 WriteAddress,u32 Length)//举具体的列子，带入分析   
{ 			 		                            //100           //453            
	u16 pageremain;	   
	pageremain=256-WriteAddress%256;/*单页剩余的字节数*/	
	printf("pageremain=%d\r\n",pageremain);	
	if(pageremain>=Length)pageremain=Length;/*不大于256个字节*/
	while(1)
	{	   
		w25q64_write_page(WriteAddress,(char*)Buffer,pageremain);
		printf("Length=%d\r\n",Length);
	
		if(Length==pageremain)break;/*写入结束了*/
	 	else /*NumByteToWrite>pageremain*/
		{
			Buffer+=pageremain;
			WriteAddress+=pageremain;	

			Length-=pageremain;			  /*减去已经写入了的字节数*/
			if(Length>256)pageremain=256; /*一次可以写入256个字节*/
			else pageremain=Length; 	  /*不够256个字节了*/
		}
	}    
}


