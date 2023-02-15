#include "w25qxx.h"
#include "usart1.h"
#include "spi.h"
/*********************************************************************************************************
* 函 数 名 : W25QXX_WriteEnable
* 功能说明 : 器件写使能
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 将WEL置位 
*********************************************************************************************************/ 
void W25QXX_WriteEnable(void)   
{
	W25QXX_CS = 0;
	Spi1_WriteReadByte(W25QXX_WriteEn);
	W25QXX_CS = 1;
} 
/*********************************************************************************************************
* 函 数 名 : W25QXX_WriteDisable
* 功能说明 : 器件写失能
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 将WEL复位
*********************************************************************************************************/ 
void W25QXX_WriteDisable(void)   
{
	W25QXX_CS = 0;
	Spi1_WriteReadByte(W25QXX_WriteDis);
	W25QXX_CS = 1;
} 
/*********************************************************************************************************
* 函 数 名 : W25QXX_ReadStatusRegister
* 功能说明 : 读状态寄存器
* 形    参 : 无
* 返 回 值 : 读取到的值
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char W25QXX_ReadStatusRegister(void)
{
	unsigned char temp = 0;
	W25QXX_CS = 0;
	Spi1_WriteReadByte(W25QXX_ReadSR);
	temp = Spi1_WriteReadByte(0xff);
	W25QXX_CS = 1;
	return temp;
}
/*********************************************************************************************************
* 函 数 名 : W25QXX_WriteStatusRegister
* 功能说明 : 写状态寄存器
* 形    参 : data：写入的值
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void W25QXX_WriteStatusRegister(unsigned char data)
{
	W25QXX_CS = 0;
	Spi1_WriteReadByte(W25QXX_WriteSR);
	Spi1_WriteReadByte(data);
	W25QXX_CS = 1;
}
/*********************************************************************************************************
* 函 数 名 : W25QXX_ReadID
* 功能说明 : 读器件ID号
* 形    参 : 0XEF17,表示芯片型号为W25Q128 
* 返 回 值 : 0XEF13,表示芯片型号为W25Q08  0XEF14,表示芯片型号为W25Q16  
* 备    注 : 0XEF15,表示芯片型号为W25Q32  0XEF16,表示芯片型号为W25Q64 
*********************************************************************************************************/ 
unsigned short W25QXX_ReadID(void)
{
	unsigned short temp = 0;
	W25QXX_CS = 0;						//拉低片选
	Spi1_WriteReadByte(W25QXX_ReadId);	//发送0x90指令
	Spi1_WriteReadByte(0x00);			//发送24位地址的最高8位
	Spi1_WriteReadByte(0x00);			//发送24位地址的中间8位
	Spi1_WriteReadByte(0x00);			//发送24位地址的最低8位
	temp = Spi1_WriteReadByte(0xff);	//读取器件ID高8位
	temp <<= 8;
	temp |= Spi1_WriteReadByte(0xff);	//读取器件ID低8位
	W25QXX_CS = 1;						//拉高片选
	return temp;						//返回结果
} 
/*********************************************************************************************************
* 函 数 名 : W25QXX_WaitBusy
* 功能说明 : 等待器件空闲
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 在对器件进行读写操作时应先等待器件空闲
*********************************************************************************************************/ 
void W25QXX_WaitBusy(void)   
{   
	unsigned char temp = 0;
	do
	{
		temp = W25QXX_ReadStatusRegister();
	}while(temp&0x01);
}  
/*********************************************************************************************************
* 函 数 名 : W25QXX_EraseSector
* 功能说明 : 擦除一个扇区
* 形    参 : sector: 扇区编号 0 ~ （2048-1）
* 返 回 值 : 无
* 备    注 : 一个扇区4KB(4096个字节，擦除后每个字节都是0xff)
*********************************************************************************************************/ 
void W25QXX_EraseSector(unsigned int sector)   
{  
	unsigned int addr = sector * 4096;		//计算该扇区的地址
	W25QXX_WriteEnable();					//写使能
	W25QXX_WaitBusy();						//等待空闲
	W25QXX_CS = 0;
	Spi1_WriteReadByte(W25QXX_SectorErase);	//发送扇区擦除指令
	Spi1_WriteReadByte(addr>>16);			//发送24位地址的最高8位
	Spi1_WriteReadByte(addr>>8);			//发送24位地址的中间8位
	Spi1_WriteReadByte(addr);			    //发送24位地址的最低8位
	W25QXX_CS = 1;
	W25QXX_WaitBusy();						//等待空闲
}
/*********************************************************************************************************
* 函 数 名 : W25QXX_WritePage
* 功能说明 : 页写（在指定地址开始写入最大256字节的数据）
* 形    参 : buffer：数据存储区，read_address：开始写入的地址(24bit)，num_data：要写入的字节数(最大256)
* 返 回 值 : 无
* 备    注 : 必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
*********************************************************************************************************/ 
void W25QXX_WritePage(unsigned char* buffer,unsigned int addr,unsigned char num)
{
	unsigned char i = 0;
	W25QXX_WriteEnable();					//写使能
	W25QXX_WaitBusy();						//等待空闲
	W25QXX_CS = 0;
	Spi1_WriteReadByte(W25QXX_PageProgram);	//发送页写指令
	Spi1_WriteReadByte(addr>>16);			//发送24位地址的最高8位
	Spi1_WriteReadByte(addr>>8);			//发送24位地址的中间8位
	Spi1_WriteReadByte(addr);			    //发送24位地址的最低8位
	for(i=0; i<num; i++)
		Spi1_WriteReadByte(*buffer++);
	W25QXX_CS = 1;
	W25QXX_WaitBusy();						//等待空闲
} 
/*********************************************************************************************************
* 函 数 名 : W25QXX_ReadDatas
* 功能说明 : 读取数据
* 形    参 : buffer：数据缓存区，read_address：开始读取的地址(24bit)，num_data:要读取的字节数(最大32768)
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void W25QXX_ReadDatas(unsigned char* buffer,unsigned int addr,unsigned short num)   
{ 
	unsigned short i = 0;
	W25QXX_WaitBusy();						//等待空闲
 	W25QXX_CS = 0;
	Spi1_WriteReadByte(W25QXX_ReadData);	//发送读数据指令
	Spi1_WriteReadByte(addr>>16);			//发送24位地址的最高8位
	Spi1_WriteReadByte(addr>>8);			//发送24位地址的中间8位
	Spi1_WriteReadByte(addr);			    //发送24位地址的最低8位
	for(i=0; i<num; i++)
		*buffer++ = Spi1_WriteReadByte(0xff);
	W25QXX_CS = 1;
	W25QXX_WaitBusy();						//等待空闲
} 
/*********************************************************************************************************
* 函 数 名 : USART1_W25QXX
* 功能说明 : 通过串口往W25Q烧写数据 
* 形    参 : sector：起止扇区编号，n：准备写入的数据占据的扇区个数
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void USART1_W25QXX(unsigned short sector,unsigned short n)
{
	unsigned char temp = 0;
	unsigned short i = 0;
	unsigned int addr = sector * 4096;		//计算起止扇区的地址
	printf("\r\n从%d扇区开始擦除，共%d个扇区（%dKB）!\r\n",sector,n,n*4);
	for(i=0; i<n; i++)
		W25QXX_EraseSector(sector+i);
	printf("\r\n擦除完成，请发送需要烧写的文件！\r\n");
	USART_ITConfig(USART1, USART_IT_RXNE,DISABLE);	//关闭串口接收中断
	USART_ITConfig(USART1, USART_IT_IDLE,DISABLE);	//关闭串口空闲中断
	while(1)
	{
		if(USART1->SR&0x20) 					//等待收到数据
		{
			temp = USART1->DR;					//读取数据
			W25QXX_WritePage(&temp, addr++,1);	//写入数据
		}
//		else if(USART1->SR&0x10)
//		{
//			USART1->DR;
//			USART_ITConfig(USART1, USART_IT_RXNE,ENABLE);	//开启串口接收中断
//			USART_ITConfig(USART1, USART_IT_IDLE,ENABLE);	//开启串口空闲中断
//			break;
//		}
	}
}
/*********************************************************************************************************
* 函 数 名 : W25QXX_Init
* 功能说明 : 初始化W25Qxx端口
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : CS --PA15
*********************************************************************************************************/ 
void W25QXX_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	//使能PC外设时钟
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_7;		//初始化引脚
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;		//输出模式
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;		//推挽输出
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;	//2MHZ
	GPIO_Init(GPIOC, &GPIO_InitStruct);				//根据GPIO_InitStruct初始化
	
	Spi1_Init();									//初始化SPI
	W25QXX_CS = 1;									//片选默认拉高，不选中器件
	printf("\r\nFlash ID：%x\r\n", W25QXX_ReadID());
}  


