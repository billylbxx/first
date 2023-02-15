#include "wmiic.h"
#include "systick.h"

void WM_IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;             
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9;	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;    //通用输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;    //开漏
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  //速度配置
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL; //浮空（无上下拉）
	GPIO_Init(GPIOB, &GPIO_InitStructure);            //初始化
	
	WM_IIC_SCL = 1;
	WM_IIC_SDA = 1;
}
/*起止信号：SCL为高电平期间，SDA由高电平向低电平的跳变。*/
void WM_IIC_Start(void)
{
	WM_IIC_SDA_OUT();	//输出模式
	WM_IIC_SCL = 1;	//时钟线为高
	WM_IIC_SDA = 1;	//数据线为高
	Delay_Us(5);	//保持，为了产生下沿的动作
	WM_IIC_SDA = 0;	//SCL为高电平期间，拉低SDA，产生起止信号
	Delay_Us(5);
	WM_IIC_SCL = 0;	//占用总线，准备发送数据
	Delay_Us(5);
}
/*终止信号：SCL为高电平期间，SDA由低电平向高电平的跳变。*/
void WM_IIC_Stop(void)
{
	WM_IIC_SDA_OUT();	//输出模式
	WM_IIC_SCL = 0;	//SCL为低电平时才能改变SDA的状态
	WM_IIC_SDA = 0;	//拉低SDA，为产生上沿做准备
	Delay_Us(5);
	WM_IIC_SCL = 1;	//在SCL为高电平期间
	WM_IIC_SDA = 1;	//SDA由低向高跳变
	Delay_Us(5);
}
/*应答信号：每个字节（8bit）传输完成后的下一个时钟信号，*/
/*在SCL为高电平期间，SDA为低，则表示一个应答信号（ACK）。*/
void WM_IIC_ACK(void)
{	
	WM_IIC_SCL = 0;	//SCL为低电平时才能改变SDA的状态
	WM_IIC_SDA_OUT();	//输出模式
	WM_IIC_SDA = 0;	//产生应答信号
	Delay_Us(5);
	WM_IIC_SCL = 1;	//拉高SCL，从机读取应答信号
	Delay_Us(5);
	WM_IIC_SCL = 0;	//拉低SCL，准备接收下一个数据
}
/*应答信号：每个字节（8bit）传输完成后的下一个时钟信号，*/
/*在SCL为高电平期间，SDA为高，则表示一个非应答信号（NACK）。*/
void WM_IIC_NACK(void)
{
	WM_IIC_SCL = 0;	//SCL为低电平时才能改变SDA的状态
	WM_IIC_SDA_OUT();	//输出模式
	WM_IIC_SDA = 1;	//产生非应答信号
	Delay_Us(5);
	WM_IIC_SCL = 1;	//拉高SCL，从机读取应答信号
	Delay_Us(5);
	WM_IIC_SCL = 0;	//拉低SCL，准备接收下一个数据
}
/*返回0：应答，返回1：非应答*/
unsigned char WM_IIC_Wait_ACK(void)
{
	WM_IIC_SDA_IN();	//输入模式
	WM_IIC_SCL = 1;	//拉高SCL，SDA保持稳定
	Delay_Us(5);
	if(WM_IIC_SDAIN)	//如果数据线为高，那么表示非应答信号
	{
		WM_IIC_Stop();
		return 1;
	}
	WM_IIC_SCL = 0;	//拉低SCL，准备下一个数据的接受
	return 0;
}
/*MSB（高位在前）*/
void WM_IIC_WriteByte(unsigned char data)
{
	unsigned char count = 0;
	WM_IIC_SDA_OUT();	//输出模式
	for(count=0;count<8;count++)
	{
		WM_IIC_SCL = 0;	//SCL为低电平时才能改变SDA的状态
		if(data&0x80)	WM_IIC_SDA = 1;
		else			WM_IIC_SDA = 0;
		data <<= 1;
		Delay_Us(5);
		WM_IIC_SCL = 1;	//数据已准备好，通知从机读取
		Delay_Us(5);
	}
	WM_IIC_SCL = 0;
	Delay_Us(5);
}
/*读取一个字节，ack = 1，发送应答信号，ack = 0，发送非应答信号*/
unsigned char WM_IIC_ReadByte(unsigned char ack)
{
	unsigned char count = 0, data = 0;
	WM_IIC_SDA_IN();	//输入模式
	for(count=0;count<8;count++)
	{
		WM_IIC_SCL = 0;	//通知从机准备数据
		Delay_Us(5);
		WM_IIC_SCL = 1;	//从机已经准备好数据了
		data <<= 1;		//准备一个空位
		data |= !!WM_IIC_SDAIN;	//二值化，确保结果只有1和0
		Delay_Us(5);
	}
	if(ack)	WM_IIC_ACK();		//根据形参决定发送应答和非应答信号
	else	WM_IIC_NACK();	
	return data;
}
