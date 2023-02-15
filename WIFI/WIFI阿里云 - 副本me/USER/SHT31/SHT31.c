#include "SHT31.h"//温湿度
#include "IIC.h"
#include "uart1.h"
#include "systick.h"
#include "stm32f4xx.h"
int count=0;
t_h temp={0.0,0.0};
void SHT31_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_25MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);	
	
	IIC_SDA=1;
	IIC_SCL=1;//空闲状态拉高
}

void SHT31_read_T_H(void)
{
	u8 date[6]={0};
	IIC_Start();
	IIC_write_byte((0x44<<1)+0); //写（write）器件指令
	if(IIC_Wait_ACK()==1)
	{
		printf("无应答，检查总线是否挂有从机或者从机损坏\r\n");
		 return;
	}
	IIC_write_byte(0x24);// 0x240b设置时钟延伸失能（一秒采样次数）
	if(IIC_Wait_ACK()==1)
	{
		printf("从机不响应\r\n");
		return;
	}
	IIC_write_byte(0x0b);
	if(IIC_Wait_ACK()==1)
	{
		printf("从机不响应\r\n");
		return;
	}
	VIP:
	IIC_Stop();
	Delay_Ms(5);
	IIC_Start();
	IIC_write_byte((0x44<<1)+1);  //读（read）器件指令
	if(IIC_Wait_ACK()==1)
	{
		count++;
		Delay_Ms(5);
		if(count>20){printf("从机不响应\r\n");return;}
		goto VIP;
	}
	date[0]=IIC_read_byte();
	IIC_ACK_NACK(0);//主机应答
	date[1]=IIC_read_byte();
	IIC_ACK_NACK(0);//主机应答
	date[2]=IIC_read_byte();
	IIC_ACK_NACK(0);//主机应答
	date[3]=IIC_read_byte();
	IIC_ACK_NACK(0);//主机应答
	date[4]=IIC_read_byte();
	IIC_ACK_NACK(0);//主机应答
	date[5]=IIC_read_byte();
	IIC_ACK_NACK(0);//主机应答
	IIC_Stop();
	if(crc8(date, 2) == date[2] && crc8(&date[3], 2) == date[5])
{
	temp.t=(date[0]<<8|date[1])/65535.0*175-45;
	temp.h= (date[3]<<8|date[4])/65535.0*100;
}
}

unsigned char crc8(const unsigned char *data, int len)
{
	const unsigned char POLYNOMIAL = 0x31;
	unsigned char crc = 0xFF;
	int j, i;

	for (j=0; j<len; j++)
	{                  
		crc ^= *data++;
		for ( i = 0; i <8; i++ )
		{
			crc = ( crc & 0x80 ) ? (crc << 1) ^ POLYNOMIAL : (crc << 1); 
		}                                                            
	}
	return crc;
}