#include "RGB.h"
#include "stm32f4xx.h"
#include "systick.h"

u8 buff_RGB[RGB_num][24];

void RGB_TO_GRB(u8 LEDid,u32 color)
 {
	int i=0;
	for(i=0;i<=7;i++)//提取G的数据（green）
	{
		buff_RGB[LEDid][i]=((color>>8&0xff)&(1<<(7-i)))?(RGB_1):(RGB_0);
	}
	for(i=8;i<=15;i++)//提取R的数据（red）
	{
		buff_RGB[LEDid][i]=((color>>16&0xff)&(1<<(15-i)))?(RGB_1):(RGB_0);
	}
	for(i=16;i<=23;i++)//提取B的数据（blue）
	{
		buff_RGB[LEDid][i]=((color&0xff)&(1<<(23-i)))?(RGB_1):(RGB_0);
	}	
}

void RGB_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct={0};//指针的大小是4个字节，不能存放具体的值
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_15;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;//复用
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//推挽电流大
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//SPI内部模块寄存器配置
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	SPI_InitTypeDef SPI_InitStruct={0};
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//双线双向全双工
  SPI_InitStruct.SPI_Mode = SPI_Mode_Master;//主模式
  SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//8位数据位,16位不好操作
  SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;  //极性，时钟线空闲为高电平
  SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;//相位,模式3，上升沿有效,第二个边沿触发
  SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;//软件管理片选
  SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;//81/4
  SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//高位在前
  //SPI_InitStruct.SPI_CRCPolynomial = 0x7;
	SPI_Init(SPI2,&SPI_InitStruct);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2);
	SPI_Cmd(SPI2, ENABLE); 
	resetRGB();

//TIM12_CH2
//		//使能时钟PB0
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//		
//		//IO口复用
//		GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_TIM12);
//		
//		//复用推挽输出
//		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_14;
//		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
//		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
//		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
//		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
//		GPIO_Init(GPIOB,&GPIO_InitStruct);
//			
//		//定时器时钟
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
//		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct={0};
//		TIM_TimeBaseInitStruct.TIM_Period = 500-1;//自动重载递增计数器自动重装载寄存器周期的值
//		TIM_TimeBaseInitStruct.TIM_Prescaler = 840-1;  //预分频器
//		TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//时钟分割
//		TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
//		TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;//设置了周期计数器值0不翻倍
//		TIM_TimeBaseInit(TIM12,&TIM_TimeBaseInitStruct);
//		
//				
//		//使能影子寄存器，可以不写
//		TIM_ARRPreloadConfig(TIM12,ENABLE);
//		TIM_PrescalerConfig(TIM12,840-1, ENABLE);
//		
//	//UG位软件触发更新
//		TIM_UpdateDisableConfig(TIM12, DISABLE);	
//		
//	//UG位置1，产生一次软件更新，目的ARR值装入影子
//		TIM_GenerateEvent(TIM12,TIM_EventSource_Update);
//		
//	//循环计数
//	TIM_UpdateRequestConfig(TIM12,TIM_UpdateSource_Global);

//	TIM_OCInitTypeDef TIM_OCInitStruct={0};
//	//TIM_OCInitStruct.TIM_OCIdleState=TIM3;
//	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;//PWM1模式
//	//TIM_OCInitStruct.TIM_OCNIdleState
//	//TIM_OCInitStruct.TIM_OCNPolarity
//	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;//有效电平1,同相
//	//TIM_OCInitStruct.TIM_OutputNState
//	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//使能输出
//	TIM_OCInitStruct.TIM_Pulse=(500-1)/2;//比较值（占空比50%）
//	TIM_OC2Init(TIM12,&TIM_OCInitStruct);


//	//使能CCR
//	TIM_OC2PreloadConfig(TIM12,TIM_OCPreload_Enable);

//	//开定时器
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
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==0);//等待发送完成
	SPI_I2S_SendData(SPI2,data);
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE)==0);//等待有效数据
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


