#include "iic.h"  
#include "sht30.h"
#include "systick.h"
/*********************************************************************************************************
* 函 数 名 : Sht30_Init
* 功能说明 : 初始化SHT30端口
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : IIC_SCL：PB6，IIC_SDA：PB7
*********************************************************************************************************/ 
void Sht30_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};    
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;    //通用输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;    //开漏
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  //速度配置
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL; //浮空（无上下拉）
	GPIO_Init(GPIOB, &GPIO_InitStructure);            //初始化
	
	IIC_SCL = 1;
	IIC_SDA = 1;
}
/*********************************************************************************************************
* 函 数 名 : Sht30_ReadData
* 功能说明 : 读取SHT30温湿度数据
* 形    参 : HT：存储温湿度的结构体指针
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Sht30_ReadData(SHT30_TypeDef *HT)
{
	unsigned short tem = 0, hum = 0;
	unsigned short buff[6] = {0};
	unsigned char ErrorCounter = 0;	

repeat:	
	IIC_Stop();
	--ErrorCounter;				   
	if(!ErrorCounter) 	//超时强制退出，避免出现死循环		
	{
		return;					   
	}
	
	IIC_Start();
	IIC_WriteByte(SHT30_ADDR<<1 | 0);	
	if(IIC_Wait_ACK() == ACK)
	{
		IIC_WriteByte(0x2C);
		if(IIC_Wait_ACK() == ACK)
		{
			IIC_WriteByte(0x06);
			if(IIC_Wait_ACK() == ACK)
			{
				IIC_Stop();
				Delay_Ms(15);
				IIC_Start();
				IIC_WriteByte(SHT30_ADDR<<1 | 1);	
				if(IIC_Wait_ACK() == ACK)
				{
					buff[0]=IIC_ReadByte(ACK);
					buff[1]=IIC_ReadByte(ACK);
					buff[2]=IIC_ReadByte(ACK);
					buff[3]=IIC_ReadByte(ACK);
					buff[4]=IIC_ReadByte(ACK);
					buff[5]=IIC_ReadByte(NACK);
					IIC_Stop();
					
					tem = ((buff[0]<<8) | buff[1]);		//温度拼接
					hum = ((buff[3]<<8) | buff[4]);		//湿度拼接
					
					/*转换实际温度*/
					HT->Temperature = (175.0*tem/65535.0-45.0) ;	// T = -45 + 175 * tem / (2^16-1)
					HT->Humidity = (100.0*hum/65535.0);				// RH = hum*100 / (2^16-1)
				}else goto repeat;
			}else goto repeat;
		}else goto repeat;
	}else goto repeat;
}

