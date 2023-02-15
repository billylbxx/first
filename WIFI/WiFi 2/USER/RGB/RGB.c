#include "RGB.h"
#include "stm32f4xx.h"
#include "systick.h"

u8 buff_RGB[RGB_num][24];

void RGB_TO_GRB(u8 LEDid,u32 color)
{
	int i=0;
	for(i=0;i<=7;i++)//��ȡG�����ݣ�green��
	{
		buff_RGB[LEDid][i]=((color>>8&0xff)&(1<<(7-i)))?(RGB_1):(RGB_0);
	}
	for(i=8;i<=15;i++)//��ȡR�����ݣ�red��
	{
		buff_RGB[LEDid][i]=((color>>16&0xff)&(1<<(15-i)))?(RGB_1):(RGB_0);
	}
	for(i=16;i<=23;i++)//��ȡB�����ݣ�blue��
	{
		buff_RGB[LEDid][i]=((color&0xff)&(1<<(23-i)))?(RGB_1):(RGB_0);
	}	
}

void RGB_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct={0};//ָ��Ĵ�С��4���ֽڣ����ܴ�ž����ֵ
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_15;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;//����
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//���������
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//SPI�ڲ�ģ��Ĵ�������
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	SPI_InitTypeDef SPI_InitStruct={0};
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//˫��˫��ȫ˫��
  SPI_InitStruct.SPI_Mode = SPI_Mode_Master;//��ģʽ
  SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//8λ����λ,16λ���ò���
  SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;  //���ԣ�ʱ���߿���Ϊ�ߵ�ƽ
  SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;//��λ,ģʽ3����������Ч,�ڶ������ش���
  SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;//�������Ƭѡ
  SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;//81/4
  SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//��λ��ǰ
  //SPI_InitStruct.SPI_CRCPolynomial = 0x7;
	SPI_Init(SPI2,&SPI_InitStruct);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2);
	SPI_Cmd(SPI2, ENABLE); 
	resetRGB();
	for(int i=0;i<4;i++)
	{
		RGB_TO_GRB(i,RGB_black);
		RGB_ENABLE();
		Delay_ms(500);
	}
}

u8 RGB_read_write(u8 data)
{
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==0);//�ȴ��������
	SPI_I2S_SendData(SPI2,data);
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE)==0);//�ȴ���Ч����
	return SPI_I2S_ReceiveData(SPI2);
}

void resetRGB(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_15);
	Delay_Us(50);
}

void RGB_ENABLE(void)
{
	int i,j;
	for(i=0;i<4;i++)
	{
		for(j=0;j<24;j++)
		{
			RGB_read_write(buff_RGB[i][j]);
		}
	}
}


