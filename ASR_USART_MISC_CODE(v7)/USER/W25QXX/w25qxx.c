#include "w25qxx.h"
#include "usart1.h"
#include "spi.h"
/*********************************************************************************************************
* �� �� �� : W25QXX_WriteEnable
* ����˵�� : ����дʹ��
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ��WEL��λ 
*********************************************************************************************************/ 
void W25QXX_WriteEnable(void)   
{
	W25QXX_CS = 0;
	Spi1_WriteReadByte(W25QXX_WriteEn);
	W25QXX_CS = 1;
} 
/*********************************************************************************************************
* �� �� �� : W25QXX_WriteDisable
* ����˵�� : ����дʧ��
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ��WEL��λ
*********************************************************************************************************/ 
void W25QXX_WriteDisable(void)   
{
	W25QXX_CS = 0;
	Spi1_WriteReadByte(W25QXX_WriteDis);
	W25QXX_CS = 1;
} 
/*********************************************************************************************************
* �� �� �� : W25QXX_ReadStatusRegister
* ����˵�� : ��״̬�Ĵ���
* ��    �� : ��
* �� �� ֵ : ��ȡ����ֵ
* ��    ע : ��
*********************************************************************************************************/ 
unsigned char W25QXX_ReadStatusRegister(void)
{
	unsigned char temp = 0;
	W25QXX_CS = 0;
	Spi1_WriteReadByte(W25QXX_ReadSR);
	temp = Spi1_WriteReadByte(0xff);
	W25QXX_CS = 1;
	return temp;
}
/*********************************************************************************************************
* �� �� �� : W25QXX_WriteStatusRegister
* ����˵�� : д״̬�Ĵ���
* ��    �� : data��д���ֵ
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void W25QXX_WriteStatusRegister(unsigned char data)
{
	W25QXX_CS = 0;
	Spi1_WriteReadByte(W25QXX_WriteSR);
	Spi1_WriteReadByte(data);
	W25QXX_CS = 1;
}
/*********************************************************************************************************
* �� �� �� : W25QXX_ReadID
* ����˵�� : ������ID��
* ��    �� : 0XEF17,��ʾоƬ�ͺ�ΪW25Q128 
* �� �� ֵ : 0XEF13,��ʾоƬ�ͺ�ΪW25Q08  0XEF14,��ʾоƬ�ͺ�ΪW25Q16  
* ��    ע : 0XEF15,��ʾоƬ�ͺ�ΪW25Q32  0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
*********************************************************************************************************/ 
unsigned short W25QXX_ReadID(void)
{
	unsigned short temp = 0;
	W25QXX_CS = 0;						//����Ƭѡ
	Spi1_WriteReadByte(W25QXX_ReadId);	//����0x90ָ��
	Spi1_WriteReadByte(0x00);			//����24λ��ַ�����8λ
	Spi1_WriteReadByte(0x00);			//����24λ��ַ���м�8λ
	Spi1_WriteReadByte(0x00);			//����24λ��ַ�����8λ
	temp = Spi1_WriteReadByte(0xff);	//��ȡ����ID��8λ
	temp <<= 8;
	temp |= Spi1_WriteReadByte(0xff);	//��ȡ����ID��8λ
	W25QXX_CS = 1;						//����Ƭѡ
	return temp;						//���ؽ��
} 
/*********************************************************************************************************
* �� �� �� : W25QXX_WaitBusy
* ����˵�� : �ȴ���������
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : �ڶ��������ж�д����ʱӦ�ȵȴ���������
*********************************************************************************************************/ 
void W25QXX_WaitBusy(void)   
{   
	unsigned char temp = 0;
	do
	{
		temp = W25QXX_ReadStatusRegister();
	}while(temp&0x01);
}  
/*********************************************************************************************************
* �� �� �� : W25QXX_EraseSector
* ����˵�� : ����һ������
* ��    �� : sector: ������� 0 ~ ��2048-1��
* �� �� ֵ : ��
* ��    ע : һ������4KB(4096���ֽڣ�������ÿ���ֽڶ���0xff)
*********************************************************************************************************/ 
void W25QXX_EraseSector(unsigned int sector)   
{  
	unsigned int addr = sector * 4096;		//����������ĵ�ַ
	W25QXX_WriteEnable();					//дʹ��
	W25QXX_WaitBusy();						//�ȴ�����
	W25QXX_CS = 0;
	Spi1_WriteReadByte(W25QXX_SectorErase);	//������������ָ��
	Spi1_WriteReadByte(addr>>16);			//����24λ��ַ�����8λ
	Spi1_WriteReadByte(addr>>8);			//����24λ��ַ���м�8λ
	Spi1_WriteReadByte(addr);			    //����24λ��ַ�����8λ
	W25QXX_CS = 1;
	W25QXX_WaitBusy();						//�ȴ�����
}
/*********************************************************************************************************
* �� �� �� : W25QXX_WritePage
* ����˵�� : ҳд����ָ����ַ��ʼд�����256�ֽڵ����ݣ�
* ��    �� : buffer�����ݴ洢����read_address����ʼд��ĵ�ַ(24bit)��num_data��Ҫд����ֽ���(���255)
* �� �� ֵ : ��
* ��    ע : ����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
*********************************************************************************************************/ 
void W25QXX_WritePage(unsigned char* buffer,unsigned int addr,unsigned char num)
{
	unsigned char i = 0;
	W25QXX_WriteEnable();					//дʹ��
	W25QXX_WaitBusy();						//�ȴ�����
	W25QXX_CS = 0;
	Spi1_WriteReadByte(W25QXX_PageProgram);	//����ҳдָ��
	Spi1_WriteReadByte(addr>>16);			//����24λ��ַ�����8λ
	Spi1_WriteReadByte(addr>>8);			//����24λ��ַ���м�8λ
	Spi1_WriteReadByte(addr);			    //����24λ��ַ�����8λ
	for(i=0; i<num; i++)
		Spi1_WriteReadByte(*buffer++);
	W25QXX_CS = 1;
	W25QXX_WaitBusy();						//�ȴ�����
} 
/*********************************************************************************************************
* �� �� �� : W25QXX_ReadDatas
* ����˵�� : ��ȡ����
* ��    �� : buffer�����ݻ�������read_address����ʼ��ȡ�ĵ�ַ(24bit)��num_data:Ҫ��ȡ���ֽ���(���32768)
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void W25QXX_ReadDatas(unsigned char* buffer,unsigned int addr,unsigned short num)   
{ 
	unsigned short i = 0;
	W25QXX_WaitBusy();						//�ȴ�����
 	W25QXX_CS = 0;
	Spi1_WriteReadByte(W25QXX_ReadData);	//���Ͷ�����ָ��
	Spi1_WriteReadByte(addr>>16);			//����24λ��ַ�����8λ
	Spi1_WriteReadByte(addr>>8);			//����24λ��ַ���м�8λ
	Spi1_WriteReadByte(addr);			    //����24λ��ַ�����8λ
	for(i=0; i<num; i++)
		*buffer++ = Spi1_WriteReadByte(0xff);
	W25QXX_CS = 1;
	W25QXX_WaitBusy();						//�ȴ�����
} 
/*********************************************************************************************************
* �� �� �� : USART1_W25QXX
* ����˵�� : ͨ��������W25Q��д���� 
* ��    �� : sector����ֹ������ţ�n��׼��д�������ռ�ݵ���������
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void USART1_W25QXX(unsigned short sector,unsigned short n)
{
	unsigned char temp = 0;
	unsigned short i = 0;
	unsigned int addr = sector * 4096;		//������ֹ�����ĵ�ַ
	printf("\r\n��%d������ʼ��������%d��������%dKB��!\r\n",sector,n,n*4);
	for(i=0; i<n; i++)
		W25QXX_EraseSector(sector+i);
	printf("\r\n������ɣ��뷢����Ҫ��д���ļ���\r\n");
	USART_ITConfig(USART1, USART_IT_RXNE,DISABLE);	//�رմ��ڽ����ж�
	USART_ITConfig(USART1, USART_IT_IDLE,DISABLE);	//�رմ��ڿ����ж�
	while(1)
	{
		if(USART1->SR&0x20) 					//�ȴ��յ�����
		{
			temp = USART1->DR;					//��ȡ����
			W25QXX_WritePage(&temp, addr++,1);	//д������
		}
		else if(USART1->SR&0x10)
		{
			USART1->DR;
			USART_ITConfig(USART1, USART_IT_RXNE,ENABLE);	//�������ڽ����ж�
			USART_ITConfig(USART1, USART_IT_IDLE,ENABLE);	//�������ڿ����ж�
			break;
		}
	}
}
/*********************************************************************************************************
* �� �� �� : W25QXX_Init
* ����˵�� : ��ʼ��W25Qxx�˿�
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : CS --PA15
*********************************************************************************************************/ 
void W25QXX_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	//ʹ��PC����ʱ��
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_7;		//��ʼ������
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;		//���ģʽ
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;		//�������
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;	//2MHZ
	GPIO_Init(GPIOC, &GPIO_InitStruct);				//����GPIO_InitStruct��ʼ��
	
	Spi1_Init();									//��ʼ��SPI
	W25QXX_CS = 1;									//ƬѡĬ�����ߣ���ѡ������
	printf("\r\nFlash ID��%x\r\n", W25QXX_ReadID());
}  

