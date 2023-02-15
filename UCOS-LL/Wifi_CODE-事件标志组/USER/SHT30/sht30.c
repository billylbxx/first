#include "iic.h"  
#include "sht30.h"
#include "systick.h"
/*********************************************************************************************************
* �� �� �� : Sht30_Init
* ����˵�� : ��ʼ��SHT30�˿�
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : IIC_SCL��PB6��IIC_SDA��PB7
*********************************************************************************************************/ 
void Sht30_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};    
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;    //ͨ�����
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;    //��©
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  //�ٶ�����
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL; //���գ�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);            //��ʼ��
	
	IIC_SCL = 1;
	IIC_SDA = 1;
}
/*********************************************************************************************************
* �� �� �� : Sht30_ReadData
* ����˵�� : ��ȡSHT30��ʪ������
* ��    �� : HT���洢��ʪ�ȵĽṹ��ָ��
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void Sht30_ReadData(SHT30_TypeDef *HT)
{
	unsigned short tem = 0, hum = 0;
	unsigned short buff[6] = {0};
	unsigned char ErrorCounter = 0;	

repeat:	
	IIC_Stop();
	--ErrorCounter;				   
	if(!ErrorCounter) 	//��ʱǿ���˳������������ѭ��		
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
					
					tem = ((buff[0]<<8) | buff[1]);		//�¶�ƴ��
					hum = ((buff[3]<<8) | buff[4]);		//ʪ��ƴ��
					
					/*ת��ʵ���¶�*/
					HT->Temperature = (175.0*tem/65535.0-45.0) ;	// T = -45 + 175 * tem / (2^16-1)
					HT->Humidity = (100.0*hum/65535.0);				// RH = hum*100 / (2^16-1)
				}else goto repeat;
			}else goto repeat;
		}else goto repeat;
	}else goto repeat;
}

