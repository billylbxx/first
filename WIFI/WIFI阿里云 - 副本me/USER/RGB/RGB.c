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

//TIM12_CH2
//		//ʹ��ʱ��PB0
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//		
//		//IO�ڸ���
//		GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_TIM12);
//		
//		//�����������
//		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_14;
//		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
//		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
//		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
//		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
//		GPIO_Init(GPIOB,&GPIO_InitStruct);
//			
//		//��ʱ��ʱ��
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
//		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct={0};
//		TIM_TimeBaseInitStruct.TIM_Period = 500-1;//�Զ����ص����������Զ���װ�ؼĴ������ڵ�ֵ
//		TIM_TimeBaseInitStruct.TIM_Prescaler = 840-1;  //Ԥ��Ƶ��
//		TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//ʱ�ӷָ�
//		TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
//		TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;//���������ڼ�����ֵ0������
//		TIM_TimeBaseInit(TIM12,&TIM_TimeBaseInitStruct);
//		
//				
//		//ʹ��Ӱ�ӼĴ��������Բ�д
//		TIM_ARRPreloadConfig(TIM12,ENABLE);
//		TIM_PrescalerConfig(TIM12,840-1, ENABLE);
//		
//	//UGλ�����������
//		TIM_UpdateDisableConfig(TIM12, DISABLE);	
//		
//	//UGλ��1������һ��������£�Ŀ��ARRֵװ��Ӱ��
//		TIM_GenerateEvent(TIM12,TIM_EventSource_Update);
//		
//	//ѭ������
//	TIM_UpdateRequestConfig(TIM12,TIM_UpdateSource_Global);

//	TIM_OCInitTypeDef TIM_OCInitStruct={0};
//	//TIM_OCInitStruct.TIM_OCIdleState=TIM3;
//	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;//PWM1ģʽ
//	//TIM_OCInitStruct.TIM_OCNIdleState
//	//TIM_OCInitStruct.TIM_OCNPolarity
//	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;//��Ч��ƽ1,ͬ��
//	//TIM_OCInitStruct.TIM_OutputNState
//	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//ʹ�����
//	TIM_OCInitStruct.TIM_Pulse=(500-1)/2;//�Ƚ�ֵ��ռ�ձ�50%��
//	TIM_OC2Init(TIM12,&TIM_OCInitStruct);


//	//ʹ��CCR
//	TIM_OC2PreloadConfig(TIM12,TIM_OCPreload_Enable);

//	//����ʱ��
//	TIM_Cmd(TIM12,ENABLE);

	for(int i=0;i<4;i++)
	{
		RGB_TO_GRB(i,RGB_black);
		RGB_ENABLE();
		delay_ms(500);
	}
}

//void RGB_PWM(int CRR)
//{
////	RGB_TO_GRB(0,RGB_RED);
////	RGB_ENABLE();
//	TIM_SetCompare2(TIM12,CRR);
//}	

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


