#include "rgb.h"
#include "systick.h"

static void RGB_RgbToGrb(unsigned char LedId, const unsigned int Color);
static void RGB_SendArray(void);

unsigned char pixelBuffer[Pixel_NUM][24];	//������ɫ���ݻ�����
/*********************************************************************************************************
* �� �� �� : Rgb_Init
* ����˵�� : ��ʼ��RGB�˿ڣ���ʼ��SPI2����ʼ��DMA1
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : Data transefer time:��TH+TL = 1.25us+-600ns��
			 T0H 		0 code ,high voltage time 		0.4us ��150ns
			 T1H 		1 code ,high voltage time 		0.8us ��150ns
			 T0L 		0 code , low voltage time 		0.85us ��150ns
			 T1L 		1 code ,low voltage time 		0.45us ��150ns
			 RES 		low voltage time 				Above 50us
*********************************************************************************************************/ 
void Rgb_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure	= {0};
	SPI_InitTypeDef   SPI_InitStructure		= {0};
	DMA_InitTypeDef   DMA_InitStructure		= {0};

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  //ʹ��GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,  ENABLE);  //ʹ��SPI2ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,	 ENABLE);  //DMA1ʱ��ʹ�� 

	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_15;                 //PB15���ù������	
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;                //���ù���
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;               //�������
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;           //100MHz
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;                //����
	GPIO_Init(GPIOB, &GPIO_InitStructure);                         //��ʼ��

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);       //PB15����Ϊ SPI2

	SPI_InitStructure.SPI_Direction 		= SPI_Direction_1Line_Tx;    //SPI����Ϊ���ߵ���
	SPI_InitStructure.SPI_Mode 				= SPI_Mode_Master;		     //SPI����
	SPI_InitStructure.SPI_DataSize 		 	= SPI_DataSize_8b;		     //8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL 				= SPI_CPOL_High;		     //ʱ�ӿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA 				= SPI_CPHA_2Edge;	         //�ڶ���ʱ�ӱ��ز���
	SPI_InitStructure.SPI_NSS  				= SPI_NSS_Soft;		         //�������Ƭѡ
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;	 //42M/8 = 5.25M
	SPI_InitStructure.SPI_FirstBit 			= SPI_FirstBit_MSB;	         //�ȷ�MSB
	SPI_InitStructure.SPI_CRCPolynomial 	= 7;	                     //CRCֵ����Ķ���ʽ
	SPI_Init(SPI2, &SPI_InitStructure);                                  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);	         		 //ʹ��SPI2��DMA���� 	
	SPI_Cmd(SPI2, ENABLE);                                               //ʹ��SPI2����

	DMA_DeInit(DMA1_Stream4);

	while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE){}                       //�ȴ�DMA������ 

	DMA_InitStructure.DMA_Channel 			 = DMA_Channel_0;                 //ͨ��ѡ��
	DMA_InitStructure.DMA_PeripheralBaseAddr = (unsigned int)&SPI2->DR;      //DMA�����ַ
	DMA_InitStructure.DMA_Memory0BaseAddr 	 = (unsigned int)pixelBuffer;    //DMA�洢����ַ
	DMA_InitStructure.DMA_DIR 				 = DMA_DIR_MemoryToPeripheral;    //�洢��������ģʽ
	DMA_InitStructure.DMA_BufferSize 		 = Pixel_NUM * 24;             	  //���ݴ����� 
	DMA_InitStructure.DMA_PeripheralInc 	 = DMA_PeripheralInc_Disable;     //���������ģʽ
	DMA_InitStructure.DMA_MemoryInc 		 = DMA_MemoryInc_Enable;          //�洢������ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;   //8λ�������ݳ���
	DMA_InitStructure.DMA_MemoryDataSize 	 = DMA_MemoryDataSize_Byte;       //8λ�洢�����ݳ���
	DMA_InitStructure.DMA_Mode 				 = DMA_Mode_Normal;               //ʹ����ͨģʽ 
	DMA_InitStructure.DMA_Priority 			 = DMA_Priority_Medium;           //�е����ȼ�
	DMA_InitStructure.DMA_FIFOMode 			 = DMA_FIFOMode_Disable;          //��ʹ�û�����
	DMA_InitStructure.DMA_FIFOThreshold 	 = DMA_FIFOThreshold_Full;		  //��������ʱ��������
	DMA_InitStructure.DMA_MemoryBurst 		 = DMA_MemoryBurst_Single;        //�洢��ͻ�����δ���
	DMA_InitStructure.DMA_PeripheralBurst 	 = DMA_PeripheralBurst_Single;    //����ͻ�����δ���
	DMA_Init(DMA1_Stream4, &DMA_InitStructure);                               //��ʼ��DMA Stream

	DMA_Cmd(DMA1_Stream4, DISABLE);                           	 //�ر�DMA���� 	
	/*����Ĵ����Ǹ�λ����*/
	while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE){}	       	 //ȷ��DMA���Ա�����  		
	DMA_SetCurrDataCounter(DMA1_Stream4, Pixel_NUM * 24);   	 //���ݴ�����  
	DMA_Cmd(DMA1_Stream4, ENABLE); 								 //ʹ��DMA1

	RGB_SetNumColor(Pixel_NUM, RGB_BLACK);                     	 //���е���Ϩ��
}
/*********************************************************************************************************
* �� �� �� : RGB_SetColor
* ����˵�� : ����ĳ���������ɫ
* ��    �� : LedId������ID��Color�����õ���ɫ
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void RGB_SetColor(unsigned char LedId, const unsigned int Color)
{	
	RGB_RgbToGrb(LedId,Color);
	RGB_SendArray();
}
/*********************************************************************************************************
* �� �� �� : RGB_SetNumColor
* ����˵�� : ����ָ�������ĵ���Ϊ��ͬ��ɫ
* ��    �� : Pixel_LEN��Ҫ���õĵ���������Color�����õ���ɫ
* �� �� ֵ : 
* ��    ע : 
*********************************************************************************************************/ 
void RGB_SetNumColor(unsigned char Pixel_LEN, const unsigned int Color)
{	
	unsigned char i = 0;
	for(i=0; i<Pixel_LEN; i++)
		RGB_RgbToGrb(i, Color);
	RGB_SendArray();
}
/*********************************************************************************************************
* �� �� �� : RGB_RgbToGrb
* ����˵�� : ��RGBת����GRB�洢�����ݻ�������
* ��    �� : LedId������ID��Color��ԭʼ��RGB
* �� �� ֵ : ��
* ��    ע : spiģ��ws2812ʱ���ԭ��ͨ��spi����һ���������ֵ����ʾ��0����͡�1����ĵ�ƽʱ��
���籾����spiԤ��Ƶ8��Ƶ����42M/8 = 5.25M��ʱ������T = 1/5.25M = 190ns��Ҳ����˵һ��CLK����Ϊ190ns��
��ô����һ���ֽ���Ҫ190*8 = 1520ns = 1.52us�����ʱ�����ڸպ�������ws2812��1.25us+-600ns�����
��������ͨ������0xC0��ʾ��0���룬��ôCLKʱ�����1100 0000��190*2 = 380ns��190*6 = 1140ns��
Ҳ����˵380us�ĸߵ�ƽ��1140us�ĵ͵�ƽ����������ź��ǲ��Ǹպ÷��ϡ�0�����Ҫ�󣿡�1�����0xF8ͬ��
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
* �� �� �� : RGB_SendArray
* ����˵�� : ����DMA���䣬�����ɺõ���ɫ����ͨ��DMA���䵽SPI��MOSI
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
static void RGB_SendArray(void)
{	
	if(DMA_GetFlagStatus(DMA1_Stream4, DMA_FLAG_TCIF4) != RESET)	 //�ȴ�DMA1_Steam4�������		
	{	
		DMA_ClearFlag(DMA1_Stream4, DMA_FLAG_TCIF4);               	 //���DMA1_Steam4������ɱ�־����Ԥ��DMA_FLAG_TCIF0���㣬�������Channel		
		DMA_Cmd(DMA1_Stream4, DISABLE);                           	 //�ر�DMA���� 	
		while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE){}	         //ȷ��DMA���Ա�����  		
		DMA_SetCurrDataCounter(DMA1_Stream4, Pixel_NUM * 24);   	 //���ݴ�����  
		DMA_Cmd(DMA1_Stream4, ENABLE);                            	 //����DMA���� 
	}
}
