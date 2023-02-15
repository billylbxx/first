#include "mlx90614.h"
#include "systick.h"
#include "iic.h"
/*********************************************************************************************************
* 函 数 名 : Mlx90614_Init
* 功能说明 : Mlx90614端口初始化
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : IIC_SCL：PB6，IIC_SDA：PB7
*********************************************************************************************************/ 
void Mlx90614_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  
    GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_6 | GPIO_Pin_7;	
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;    //开漏
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  //速度配置
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL; //浮空（无上下拉）
		GPIO_Init(GPIOB, &GPIO_InitStructure);

    IIC_SCL = 1;
		IIC_SDA = 1;
}
/*********************************************************************************************************
* 函 数 名 : Mlx90614_ReadTemperature
* 功能说明 : 从RAM中读取温度值，并转换后返回
* 形    参 : 无
* 返 回 值 : 温度值
* 备    注 : 无
*********************************************************************************************************/ 
void Mlx90614_ReadTemperature(float *temp)
{
    unsigned char DataL = 0;			
    unsigned char DataH = 0;			
    unsigned char ErrorCounter = 0;	
	
repeat:
	IIC_Stop();			   
	--ErrorCounter;				   
	if(!ErrorCounter)	//超时强制退出，避免出现死循环				
	{
		return;					   
	}

	IIC_Start();				
	IIC_WriteByte(MLX90614_ADDR << 1|0);
	if(IIC_Wait_ACK() == ACK)
	{
		IIC_WriteByte(RAM_ACCESS|RAM_TOBJ1);
		if(IIC_Wait_ACK() == ACK)	   
		{
			IIC_Start();					
			IIC_WriteByte(MLX90614_ADDR<<1 | 1);
			if(IIC_Wait_ACK() == ACK)	
			{
				DataL = IIC_ReadByte(ACK);	
				DataH = IIC_ReadByte(ACK); 
				IIC_ReadByte(NACK);
				IIC_Stop();	
				
				/*转换出实际温度*/
				*temp = (DataH<<8 | DataL) * 0.02 - 273.15;
			}else goto	repeat;             	
		}else goto	repeat;		    	
	}else goto	repeat;		
	
}

