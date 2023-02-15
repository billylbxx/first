#include "mlx90614.h"
#include "systick.h"
#include "iic.h"
/*********************************************************************************************************
* �� �� �� : Mlx90614_Init
* ����˵�� : Mlx90614�˿ڳ�ʼ��
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : IIC_SCL��PB6��IIC_SDA��PB7
*********************************************************************************************************/ 
void Mlx90614_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  
    GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_6 | GPIO_Pin_7;	
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;    //��©
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  //�ٶ�����
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL; //���գ�����������
		GPIO_Init(GPIOB, &GPIO_InitStructure);

    IIC_SCL = 1;
		IIC_SDA = 1;
}
/*********************************************************************************************************
* �� �� �� : Mlx90614_ReadTemperature
* ����˵�� : ��RAM�ж�ȡ�¶�ֵ����ת���󷵻�
* ��    �� : ��
* �� �� ֵ : �¶�ֵ
* ��    ע : ��
*********************************************************************************************************/ 
void Mlx90614_ReadTemperature(float *temp)
{
    unsigned char DataL = 0;			
    unsigned char DataH = 0;			
    unsigned char ErrorCounter = 0;	
	
repeat:
	IIC_Stop();			   
	--ErrorCounter;				   
	if(!ErrorCounter)	//��ʱǿ���˳������������ѭ��				
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
				
				/*ת����ʵ���¶�*/
				*temp = (DataH<<8 | DataL) * 0.02 - 273.15;
			}else goto	repeat;             	
		}else goto	repeat;		    	
	}else goto	repeat;		
	
}

