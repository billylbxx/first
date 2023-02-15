#include "sd_driver.h"
#include "usart1.h"
#include "SPI.h"

unsigned char SD_Type = 0;	//SD卡的类型 

/*******************************************移植修改区***************************************************/
/*********************************************************************************************************
* 函 数 名 : SD_PORT_Init
* 功能说明 : 初始化SD卡的SPI端口
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void SD_PORT_Init()
{	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin 	= GPIO_Pin_4;			
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_OUT;		
	GPIO_InitStruct.GPIO_OType 	= GPIO_OType_PP;		
	GPIO_InitStruct.GPIO_PuPd 	= GPIO_PuPd_NOPULL;	
	GPIO_InitStruct.GPIO_Speed 	= GPIO_Speed_2MHz;	
	GPIO_Init(GPIOA, &GPIO_InitStruct);				
	
	spi1_Init();
	
	SD_CS = 1;
}
/*********************************************************************************************************
* 函 数 名 : SD_SPI_ReadWriteByte
* 功能说明 : 读写SPI的函数接口
* 形    参 : data：写入的数据
* 返 回 值 : 读取到的数据
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char SD_SPI_ReadWriteByte(unsigned char data)
{
	return spi1_r_w_data(data);
}	  
/*********************************************************************************************************
* 函 数 名 : SD_SPI_SpeedLow
* 功能说明 : 设置SPI低速
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : D卡初始化的时候,需要低速
*********************************************************************************************************/ 
void SD_SPI_SpeedLow(void)
{
	SPI1->CR1 &= 0XFFC7;					//位3-5清零，用来设置波特率
	SPI1->CR1 |= SPI_BaudRatePrescaler_256;	//设置SPI1速度 
	SPI_Cmd(SPI1, ENABLE); 					//使能SPI1	
}
/*********************************************************************************************************
* 函 数 名 : SD_SPI_SpeedHigh
* 功能说明 : 设置SPI高速
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void SD_SPI_SpeedHigh(void)
{
	SPI1->CR1 &= 0XFFC7;					//位3-5清零，用来设置波特率
	SPI1->CR1 |= SPI_BaudRatePrescaler_16;	//设置SPI1速度 
	SPI_Cmd(SPI1, ENABLE); 					//使能SPI1	
}
/***********************************************END******************************************************/

/*********************************************************************************************************
* 函 数 名 : SD_DisSelect
* 功能说明 : 取消片选，释放SPI总线
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 提供额外的8个时钟
*********************************************************************************************************/ 
void SD_DisSelect(void)
{
	SD_CS = 1;
 	SD_SPI_ReadWriteByte(0xff);	
}
/*********************************************************************************************************
* 函 数 名 : SD_Select
* 功能说明 : 选择sd卡,并且等待卡准备OK
* 形    参 : 无
* 返 回 值 : 0：成功，1：失败
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char SD_Select(void)
{
	SD_CS = 0;
	if(SD_WaitReady() == 0)	return 0;	//等待成功
	SD_DisSelect();
	return 1;							//等待失败
}
/*********************************************************************************************************
* 函 数 名 : SD_WaitReady
* 功能说明 : 等待卡准备好
* 形    参 : 无
* 返 回 值 : 0:准备好了，其他：错误代码
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char SD_WaitReady(void)
{
	unsigned int t = 0;
	
	do
	{
		if(SD_SPI_ReadWriteByte(0XFF) == 0XFF)	return 0;	//OK
		t++;		  	
	}while(t<0XFFFFFF);										//等待 
	return 1;
}
/*********************************************************************************************************
* 函 数 名 : SD_GetResponse
* 功能说明 : 等待SD卡回应
* 形    参 : Response：要得到的回应值
* 返 回 值 : 0：得到希望的应答；其他：错误代码
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char SD_GetResponse(unsigned char Response)
{
	unsigned short Count = 0xFFFF;					//等待次数	   						  
	while((SD_SPI_ReadWriteByte(0XFF) != Response) && Count) Count--;	//等待得到准确的回应  	  
	if (Count == 0)	return MSD_RESPONSE_FAILURE;	//得到回应失败   
	else 			return MSD_RESPONSE_NO_ERROR;	//正确回应
}
/*********************************************************************************************************
* 函 数 名 : SD_RecvData
* 功能说明 : 从SD卡读取一个数据包的内容
* 形    参 : buf：数据缓存区；len：需要读取的长度
* 返 回 值 : 0：成功；其他：错误代码
* 备    注 : 无
*********************************************************************************************************/ 	
unsigned char SD_RecvData(unsigned char*buf, unsigned short len)
{			  	  
	if(SD_GetResponse(0xFE))	return 1;	//等待SD卡发回数据起始令牌0xFE
    while(len--)	//开始接收数据
    {
        *buf = SD_SPI_ReadWriteByte(0xFF);
        buf++;
    }
    //下面是2个伪CRC（dummy CRC）
    SD_SPI_ReadWriteByte(0xFF);
    SD_SPI_ReadWriteByte(0xFF);									  					    
    return 0;		//读取成功
}
/*********************************************************************************************************
* 函 数 名 : SD_SendBlock
* 功能说明 : 向SD卡写入一个512字节的数据包的内容 
* 形    参 : buf：数据缓存区；cmd：指令
* 返 回 值 : 0：成功，其他：错误代码
* 备    注 : 写入时以块为单位
*********************************************************************************************************/ 
unsigned char SD_SendBlock(unsigned char*buf, unsigned char cmd)
{	
	unsigned short t = 0;	
	
	if(SD_WaitReady())	return 1;	//等待准备失效
	SD_SPI_ReadWriteByte(cmd);
	if(cmd != 0XFD)					//不是结束指令
	{
		for(t=0; t<512; t++)	SD_SPI_ReadWriteByte(buf[t]);	//提高速度,减少函数传参时间
	    SD_SPI_ReadWriteByte(0xFF); 	//忽略crc
	    SD_SPI_ReadWriteByte(0xFF);
		t=SD_SPI_ReadWriteByte(0xFF);	//接收响应
		if((t&0x1F)!=0x05)return 2;		//响应错误									  					    
	}						 									  					    
    return 0;//写入成功
}
/*********************************************************************************************************
* 函 数 名 : SD_SendCmd
* 功能说明 : 向SD卡发送一个命令
* 形    参 : cmd：发送的命令；arg：命令参数，crc：校验值
* 返 回 值 : SD卡返回的相应
* 备    注 : 无
*********************************************************************************************************/ 														  
unsigned char SD_SendCmd(unsigned char cmd, unsigned int arg, unsigned char crc)
{
    unsigned char r1 	= 0;	
	unsigned char Retry = 0;
	
	SD_DisSelect();						//取消上次片选
	if(SD_Select())	return 0XFF;		//片选失效 
    SD_SPI_ReadWriteByte(cmd | 0x40);	//分别写入命令
    SD_SPI_ReadWriteByte(arg >> 24);
    SD_SPI_ReadWriteByte(arg >> 16);
    SD_SPI_ReadWriteByte(arg >> 8);
    SD_SPI_ReadWriteByte(arg);	  
    SD_SPI_ReadWriteByte(crc); 
	if(cmd == CMD12)	SD_SPI_ReadWriteByte(0xff);	//Skip a stuff byte when stop reading
    
	Retry = 0X1F;	//等待响应，或超时退出
	do
	{
		r1 = SD_SPI_ReadWriteByte(0xFF);
	}while((r1&0X80) && Retry--);	 
	
    return r1;		//返回状态值
}	
/*********************************************************************************************************
* 函 数 名 : SD_GetCID
* 功能说明 : 获取SD卡的CID信息，包括制造商信息
* 形    参 : cid_data：存放CID的内存，至少16Byte
* 返 回 值 : 0：NO_ERR；1：ERR
* 备    注 : 无
*********************************************************************************************************/ 														   
unsigned char SD_GetCID(unsigned char *cid_data)
{
    unsigned char r1 = 0;
	
    r1 = SD_SendCmd(CMD10, 0, 0x01);	//发CMD10命令，读CID
    if(r1 == 0x00)
	{
		r1 = SD_RecvData(cid_data, 16);	//接收16个字节的数据	 
    }
	SD_DisSelect();	//取消片选
	
	if(r1)	return 1;
	else 	return 0;
}		
/*********************************************************************************************************
* 函 数 名 : SD_GetCSD
* 功能说明 : 获取SD卡的CSD信息，包括容量和速度信息
* 形    参 : cid_data：存放CID的内存，至少16Byte
* 返 回 值 : 0：NO_ERR；1：ERR
* 备    注 : 无
*********************************************************************************************************/ 													   
unsigned char SD_GetCSD(unsigned char *csd_data)
{
    unsigned char r1 = 0;	
	
    r1 = SD_SendCmd(CMD9, 0, 0x01);		//发CMD9命令，读CSD
    if(r1 == 0)
	{
    	r1 = SD_RecvData(csd_data, 16);	//接收16个字节的数据 
    }
	SD_DisSelect();	//取消片选
	
	if(r1)	return 1;
	else 	return 0;
}  
/*********************************************************************************************************
* 函 数 名 : SD_GetSectorCount
* 功能说明 : 获取SD卡的总扇区数（扇区数）   
* 形    参 : 无
* 返 回 值 : 0：读取容量出错；其他：SD卡的容量(扇区数/512字节)
* 备    注 : 每扇区的字节数必为512，因为如果不是512，则初始化不能通过.
*********************************************************************************************************/ 													  
unsigned int SD_GetSectorCount(void)
{
    unsigned char csd[16] = {0};
	unsigned char n = 0;
	unsigned short csize  = 0;  
    unsigned int Capacity = 0;  		    
	
    if(SD_GetCSD(csd) != 0) return 0;	//取CSD信息，如果期间出错，返回0    
    //如果为SDHC卡，按照下面方式计算
    if((csd[0]&0xC0) == 0x40)	 //V2.00的卡
    {	
		csize = csd[9] + ((unsigned short)csd[8] << 8) + 1;
		Capacity = (unsigned int)csize << 10;		//得到扇区数	 		   
    }else						//V1.XX的卡
    {	
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((unsigned short)csd[7] << 2) + ((unsigned short)(csd[6] & 3) << 10) + 1;
		Capacity = (unsigned int)csize << (n - 9);	//得到扇区数   
    }
    return Capacity;
}
/*********************************************************************************************************
* 函 数 名 : SD_Initialize
* 功能说明 : SD卡驱动层初始化
* 形    参 : 无
* 返 回 值 : 0：成功；其他：错误代码
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char SD_Initialize(void)
{
	unsigned char buf[4] = {0};  
    unsigned char r1 = 0;      // 存放SD卡的返回值
    unsigned short retry = 0;  // 用来进行超时计数
	unsigned short i = 0;

	SD_PORT_Init();		//初始化IO
 	SD_SPI_SpeedLow();	//设置到低速模式 
 	for(i=0; i<10; i++)
		SD_SPI_ReadWriteByte(0XFF);		//发送最少74个脉冲
	retry = 20;
	do
	{
		r1=SD_SendCmd(CMD0, 0, 0x95);	//进入IDLE状态
	}while((r1!=0X01) && retry--);
	
 	SD_Type = 0;//默认无卡
	if(r1 == 0X01)
	{
		if(SD_SendCmd(CMD8, 0x1AA, 0x87) == 1)	//SD V2.0
		{
			//Get trailing return value of R7 resp
			for(i=0; i<4; i++)	buf[i] = SD_SPI_ReadWriteByte(0XFF);	
			if(buf[2]==0X01 && buf[3]==0XAA)	//卡是否支持2.7~3.6V
			{
				retry = 0XFFFE;
				do
				{
					SD_SendCmd(CMD55, 0, 0X01);					//发送CMD55
					r1 = SD_SendCmd(CMD41, 0x40000000, 0X01);	//发送CMD41
				}while(r1&&retry--);
				if(retry&&SD_SendCmd(CMD58, 0, 0X01) == 0)		//鉴别SD2.0卡版本开始
				{
					for(i=0; i<4; i++)	buf[i] = SD_SPI_ReadWriteByte(0XFF);	//得到OCR值
					if(buf[0] & 0x40)	SD_Type = SD_TYPE_V2HC; //检查CCS
					else 				SD_Type = SD_TYPE_V2;   
				}
			}
		}else	//SD V1.x/ MMC	V3
		{
			SD_SendCmd(CMD55, 0, 0X01);			//发送CMD55
			r1 = SD_SendCmd(CMD41, 0, 0X01);	//发送CMD41
			if(r1 <= 1)
			{		
				SD_Type = SD_TYPE_V1;
				retry = 0XFFFE;
				do 	//等待退出IDLE模式
				{
					SD_SendCmd(CMD55, 0, 0X01);		//发送CMD55
					r1=SD_SendCmd(CMD41, 0, 0X01);	//发送CMD41
				}while(r1 && retry--);
			}else	//MMC卡不支持CMD55+CMD41识别
			{
				SD_Type = SD_TYPE_MMC;	//MMC V3
				retry = 0XFFFE;
				do //等待退出IDLE模式
				{											    
					r1 = SD_SendCmd(CMD1, 0, 0X01);	//发送CMD1
				}while(r1 && retry--);  
			}
			if(retry==0 || SD_SendCmd(CMD16, 512, 0X01)!=0)	SD_Type = SD_TYPE_ERR;	//错误的卡
		}
	}
	SD_DisSelect();		//取消片选
	SD_SPI_SpeedHigh();	//高速
	if(SD_Type)	return 0;
	else if(r1)	return r1; 	   
	return 0xaa;		//其他错误
}
/*********************************************************************************************************
* 函 数 名 : SD_ReadDisk
* 功能说明 : 读SD卡，以扇区为单位
* 形    参 : buf：数据缓存区；block：起止块；cnt：读取块数
* 返 回 值 : 0：成功；其他：错误代码
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char SD_ReadDisk(unsigned char*buf,unsigned int block,unsigned short cnt)
{
	unsigned char r1 = 0;
	
	if(SD_Type != SD_TYPE_V2HC)	block <<= 9;	//转换为字节地址
	if(cnt == 1)
	{
		r1 = SD_SendCmd(CMD17, block, 0X01);		//读命令
		if(r1 == 0)//指令发送成功
		{
			r1 = SD_RecvData(buf, 512);			//接收512个字节	   
		}
	}else
	{
		r1 = SD_SendCmd(CMD18, block, 0X01);	//连续读命令
		do
		{
			r1 = SD_RecvData(buf, 512);			//接收512个字节	 
			buf += 512;  
		}while(--cnt && r1==0); 	
		SD_SendCmd(CMD12, 0, 0X01);				//发送停止命令
	}   
	SD_DisSelect();								//取消片选
	return r1;
}
/*********************************************************************************************************
* 函 数 名 : SD_WriteDisk
* 功能说明 : 写SD卡，以扇区为单位
* 形    参 : block：起止块；cnt：写入块数
* 返 回 值 : 0：成功；其他：错误代码
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char SD_WriteDisk(unsigned char*buf, unsigned int block, unsigned short cnt)
{
	unsigned char r1 = 0;
	
	if(SD_Type != SD_TYPE_V2HC)	block *= 512;		//转换为字节地址
	if(cnt == 1)
	{
		r1 = SD_SendCmd(CMD24,block, 0X01);		//读命令
		if(r1 == 0)//指令发送成功
		{
			r1 = SD_SendBlock(buf,0xFE);			//写512个字节	   
		}
	}else
	{
		if(SD_Type != SD_TYPE_MMC)
		{
			SD_SendCmd(CMD55, 0, 0X01);	
			SD_SendCmd(CMD23, cnt, 0X01);			//发送指令	
		}
 		r1 = SD_SendCmd(CMD25, block, 0X01);		//连续读命令
		if(r1 == 0)
		{
			do
			{
				r1 = SD_SendBlock(buf, 0xFC);		//接收512个字节	 
				buf += 512;  
			}while(--cnt && r1==0);
			r1 = SD_SendBlock(0, 0xFD);				//接收512个字节 
		}
	}   
	SD_DisSelect();									//取消片选
	return r1;
}	   



