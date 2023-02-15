#include "spi.h"
/*********************************************************************************************************
* 函 数 名 : Spi1_WriteReadByte
* 功能说明 : 硬件SPI1读写一个字节
* 形  参 : data：写入的数据
* 返 回 值 : 读取到的数据
* 备  注 : 无
*********************************************************************************************************/ 
unsigned char Spi1_WriteReadByte(unsigned char data)
{
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)  == RESET);		//等待发送缓冲区为空
	SPI_I2S_SendData(SPI1, data);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);		//等待发送缓冲区不为空
	return SPI_I2S_ReceiveData(SPI1);
}
/*********************************************************************************************************
* 函 数 名 : Spi2_WriteReadByte
* 功能说明 : 硬件SPI2读写一个字节
* 形  参 : data：写入的数据
* 返 回 值 : 读取到的数据
* 备  注 : 无
*********************************************************************************************************/ 
unsigned char Spi2_WriteReadByte(unsigned char data)
{
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE)  == RESET);		//等待发送缓冲区为空
	SPI_I2S_SendData(SPI2, data);
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);		//等待发送缓冲区不为空
	return SPI_I2S_ReceiveData(SPI2);
}
/*********************************************************************************************************
* 函 数 名 : Spi3_WriteReadByte
* 功能说明 : 硬件SPI3读写一个字节
* 形  参 : data：写入的数据
* 返 回 值 : 读取到的数据
* 备  注 : 无
*********************************************************************************************************/ 
unsigned char Spi3_WriteReadByte(unsigned char data)
{				
	while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE)  == RESET);		//等待发送缓冲区为空
	SPI_I2S_SendData(SPI3, data);
	while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);	//等待发送缓冲区不为空
	return SPI_I2S_ReceiveData(SPI3);	  
}
/*********************************************************************************************************
* 函 数 名 : Spi1_Init
* 功能说明 : 初始化硬件SPI1
* 形  参 : 无
* 返 回 值 : 无
* 备  注 : 无
*********************************************************************************************************/ 
void Spi1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	SPI_InitTypeDef SPI_InitStruct		= {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	//开外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,  ENABLE);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;		//复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//50MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//浮空
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//初始化

	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;	//二分频
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;						//第二个时钟边沿采样
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;						//时钟空闲为高
	SPI_InitStruct.SPI_CRCPolynomial = 0x7;							//CRC多项式
	SPI_InitStruct.SPI_DataSize  = SPI_DataSize_8b;					//八位数据帧格式
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//双线双向全双工
	SPI_InitStruct.SPI_FirstBit  = SPI_FirstBit_MSB;				//高位先发
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;						//主模式
	SPI_InitStruct.SPI_NSS  = SPI_NSS_Soft;							//软件管理
	SPI_Init(SPI1, &SPI_InitStruct);
	
	SPI_Cmd(SPI1, ENABLE);											//使能外设
}

/*********************************************************************************************************
* 函 数 名 : Spi2_Init
* 功能说明 : 初始化硬件SPI2
* 形  参 : 无
* 返 回 值 : 无
* 备  注 : 无
*********************************************************************************************************/ 
void Spi2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	SPI_InitTypeDef SPI_InitStruct		= {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	//开外设时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,  ENABLE);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;		//复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//50MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//浮空
	GPIO_Init(GPIOB, &GPIO_InitStructure);				//初始化

	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;	//二分频
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;						//第二个时钟边沿采样
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;						//时钟空闲为高
	SPI_InitStruct.SPI_CRCPolynomial = 0x7;							//CRC多项式
	SPI_InitStruct.SPI_DataSize  = SPI_DataSize_8b;					//八位数据帧格式
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//双线双向全双工
	SPI_InitStruct.SPI_FirstBit  = SPI_FirstBit_MSB;				//高位先发
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;						//主模式
	SPI_InitStruct.SPI_NSS  = SPI_NSS_Soft;							//软件管理
	SPI_Init(SPI2, &SPI_InitStruct);
	SPI_Cmd(SPI2, ENABLE);											//使能外设
}
/*********************************************************************************************************
* 函 数 名 : Spi3_Init
* 功能说明 : 初始化硬件SPI3
* 形  参 : 无
* 返 回 值 : 无
* 备  注 : 无
*********************************************************************************************************/ 
void Spi3_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	SPI_InitTypeDef SPI_InitStruct		= {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	//开外设时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,  ENABLE);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI3);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//50MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//浮空
	GPIO_Init(GPIOB, &GPIO_InitStructure);				//初始化

	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;	//二分频
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;						//第二个时钟边沿采样
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;						//时钟空闲为高
	SPI_InitStruct.SPI_CRCPolynomial = 0x7;							//CRC多项式
	SPI_InitStruct.SPI_DataSize  = SPI_DataSize_8b;					//八位数据帧格式
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//双线双向全双工
	SPI_InitStruct.SPI_FirstBit  = SPI_FirstBit_MSB;				//高位先发
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;						//主模式
	SPI_InitStruct.SPI_NSS  = SPI_NSS_Soft;							//软件管理
	SPI_Init(SPI3, &SPI_InitStruct);
	
	SPI_Cmd(SPI3, ENABLE);											//使能外设
}

