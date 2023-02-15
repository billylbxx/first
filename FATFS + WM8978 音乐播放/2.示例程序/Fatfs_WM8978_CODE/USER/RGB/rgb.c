#include "rgb.h"
#include "systick.h"

static void RGB_RgbToGrb(unsigned char LedId, const unsigned int Color);
static void RGB_SendArray(void);

unsigned char pixelBuffer[Pixel_NUM][24];	//灯珠颜色数据缓存区
/*********************************************************************************************************
* 函 数 名 : Rgb_Init
* 功能说明 : 初始化RGB端口，初始化SPI2，初始化DMA1
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : Data transefer time:（TH+TL = 1.25us+-600ns）
			 T0H 		0 code ,high voltage time 		0.4us ±150ns
			 T1H 		1 code ,high voltage time 		0.8us ±150ns
			 T0L 		0 code , low voltage time 		0.85us ±150ns
			 T1L 		1 code ,low voltage time 		0.45us ±150ns
			 RES 		low voltage time 				Above 50us
*********************************************************************************************************/ 
void Rgb_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure	= {0};
	SPI_InitTypeDef   SPI_InitStructure		= {0};
	DMA_InitTypeDef   DMA_InitStructure		= {0};

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  //使能GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,  ENABLE);  //使能SPI2时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,	 ENABLE);  //DMA1时钟使能 

	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_15;                 //PB15复用功能输出	
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;                //复用功能
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;               //推挽输出
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;           //100MHz
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;                //上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);                         //初始化

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);       //PB15复用为 SPI2

	SPI_InitStructure.SPI_Direction 		= SPI_Direction_1Line_Tx;    //SPI设置为单线单工
	SPI_InitStructure.SPI_Mode 				= SPI_Mode_Master;		     //SPI主机
	SPI_InitStructure.SPI_DataSize 		 	= SPI_DataSize_8b;		     //8位帧结构
	SPI_InitStructure.SPI_CPOL 				= SPI_CPOL_High;		     //时钟空闲状态为高电平
	SPI_InitStructure.SPI_CPHA 				= SPI_CPHA_2Edge;	         //第二个时钟边沿采样
	SPI_InitStructure.SPI_NSS  				= SPI_NSS_Soft;		         //软件管理片选
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;	 //42M/8 = 5.25M
	SPI_InitStructure.SPI_FirstBit 			= SPI_FirstBit_MSB;	         //先发MSB
	SPI_InitStructure.SPI_CRCPolynomial 	= 7;	                     //CRC值计算的多项式
	SPI_Init(SPI2, &SPI_InitStructure);                                  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);	         		 //使能SPI2的DMA发送 	
	SPI_Cmd(SPI2, ENABLE);                                               //使能SPI2外设

	DMA_DeInit(DMA1_Stream4);

	while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE){}                       //等待DMA可配置 

	DMA_InitStructure.DMA_Channel 			 = DMA_Channel_0;                 //通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = (unsigned int)&SPI2->DR;      //DMA外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr 	 = (unsigned int)pixelBuffer;    //DMA存储器地址
	DMA_InitStructure.DMA_DIR 				 = DMA_DIR_MemoryToPeripheral;    //存储器到外设模式
	DMA_InitStructure.DMA_BufferSize 		 = Pixel_NUM * 24;             	  //数据传输量 
	DMA_InitStructure.DMA_PeripheralInc 	 = DMA_PeripheralInc_Disable;     //外设非增量模式
	DMA_InitStructure.DMA_MemoryInc 		 = DMA_MemoryInc_Enable;          //存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;   //8位外设数据长度
	DMA_InitStructure.DMA_MemoryDataSize 	 = DMA_MemoryDataSize_Byte;       //8位存储器数据长度
	DMA_InitStructure.DMA_Mode 				 = DMA_Mode_Normal;               //使用普通模式 
	DMA_InitStructure.DMA_Priority 			 = DMA_Priority_Medium;           //中等优先级
	DMA_InitStructure.DMA_FIFOMode 			 = DMA_FIFOMode_Disable;          //不使用缓冲区
	DMA_InitStructure.DMA_FIFOThreshold 	 = DMA_FIFOThreshold_Full;		  //缓冲区满时触发传输
	DMA_InitStructure.DMA_MemoryBurst 		 = DMA_MemoryBurst_Single;        //存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst 	 = DMA_PeripheralBurst_Single;    //外设突发单次传输
	DMA_Init(DMA1_Stream4, &DMA_InitStructure);                               //初始化DMA Stream

	DMA_Cmd(DMA1_Stream4, DISABLE);                           	 //关闭DMA传输 	
	/*这里的传输是复位作用*/
	while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE){}	       	 //确保DMA可以被设置  		
	DMA_SetCurrDataCounter(DMA1_Stream4, Pixel_NUM * 24);   	 //数据传输量  
	DMA_Cmd(DMA1_Stream4, ENABLE); 								 //使能DMA1

	RGB_SetNumColor(Pixel_NUM, RGB_BLACK);                     	 //所有灯珠熄灭
}
/*********************************************************************************************************
* 函 数 名 : RGB_SetColor
* 功能说明 : 设置某个灯珠的颜色
* 形    参 : LedId：灯珠ID，Color：设置的颜色
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void RGB_SetColor(unsigned char LedId, const unsigned int Color)
{	
	RGB_RgbToGrb(LedId,Color);
	RGB_SendArray();
}
/*********************************************************************************************************
* 函 数 名 : RGB_SetNumColor
* 功能说明 : 设置指定数量的灯珠为相同颜色
* 形    参 : Pixel_LEN：要设置的灯珠数量，Color：设置的颜色
* 返 回 值 : 
* 备    注 : 
*********************************************************************************************************/ 
void RGB_SetNumColor(unsigned char Pixel_LEN, const unsigned int Color)
{	
	unsigned char i = 0;
	for(i=0; i<Pixel_LEN; i++)
		RGB_RgbToGrb(i, Color);
	RGB_SendArray();
}
/*********************************************************************************************************
* 函 数 名 : RGB_RgbToGrb
* 功能说明 : 将RGB转换成GRB存储到数据缓冲区中
* 形    参 : LedId：灯珠ID，Color：原始的RGB
* 返 回 值 : 无
* 备    注 : spi模拟ws2812时序的原理即通过spi发送一个具体的数值来表示‘0’码和‘1’码的电平时间
例如本例中spi预分频8分频，即42M/8 = 5.25M，时钟周期T = 1/5.25M = 190ns，也就是说一个CLK周期为190ns，
那么传输一个字节需要190*8 = 1520ns = 1.52us，这个时间周期刚好坐落在ws2812的1.25us+-600ns区间里。
本例程中通过发送0xC0表示‘0’码，那么CLK时序呈现1100 0000，190*2 = 380ns，190*6 = 1140ns。
也就是说380us的高电平，1140us的低电平，这个脉冲信号是不是刚好符合‘0’码的要求？‘1’码的0xF8同理。
*********************************************************************************************************/ 
static void RGB_RgbToGrb(unsigned char LedId, const unsigned int Color)
{	
	unsigned char i = 0;
	if(LedId > Pixel_NUM)
		return;                             
	for(i=0; i<=7; i++)
		pixelBuffer[LedId][i]= ( ( ( (Color>>8)&0xff) & (1 << (7 -i) ) ) ? (CODE1):(CODE0));
	for(i=8; i<=15; i++)
		pixelBuffer[LedId][i]= ( ( ( (Color>>16)&0xff) & (1 << (15-i) ) ) ? (CODE1):(CODE0));
	for(i=16; i<=23; i++)
		pixelBuffer[LedId][i]= ( ( ( (Color>>0)&0xff) & (1 << (23-i) ) ) ? (CODE1):(CODE0));
}
/*********************************************************************************************************
* 函 数 名 : RGB_SendArray
* 功能说明 : 启动DMA传输，把生成好的颜色数据通过DMA传输到SPI的MOSI
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
static void RGB_SendArray(void)
{	
	if(DMA_GetFlagStatus(DMA1_Stream4, DMA_FLAG_TCIF4) != RESET)	 //等待DMA1_Steam4传输完成		
	{	
		DMA_ClearFlag(DMA1_Stream4, DMA_FLAG_TCIF4);               	 //清除DMA1_Steam4传输完成标志，先预想DMA_FLAG_TCIF0的零，代表的是Channel		
		DMA_Cmd(DMA1_Stream4, DISABLE);                           	 //关闭DMA传输 	
		while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE){}	         //确保DMA可以被设置  		
		DMA_SetCurrDataCounter(DMA1_Stream4, Pixel_NUM * 24);   	 //数据传输量  
		DMA_Cmd(DMA1_Stream4, ENABLE);                            	 //开启DMA传输 
	}
}
