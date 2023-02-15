#include "wm8978.h"
#include "wmiic.h"

/*
	wm8978�Ĵ�������
	����WM8978��I2C���߽ӿڲ�֧�ֶ�ȡ��������˼Ĵ���ֵ�������ڴ��У���д�Ĵ���ʱͬ�����»��棬���Ĵ���ʱ
	ֱ�ӷ��ػ����е�ֵ��
	�Ĵ���MAP ��WM8978.pdf �ĵ�67ҳ���Ĵ�����ַ��7bit�� �Ĵ���������9bit
*/
static uint16_t WM8978_REGVAL_TBL[] = {
	0x000, 0x000, 0x000, 0x000, 0x050, 0x000, 0x140, 0x000,
	0x000, 0x000, 0x000, 0x0FF, 0x0FF, 0x000, 0x100, 0x0FF,
	0x0FF, 0x000, 0x12C, 0x02C, 0x02C, 0x02C, 0x02C, 0x000,
	0x032, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x038, 0x00B, 0x032, 0x000, 0x008, 0x00C, 0x093, 0x0E9,
	0x000, 0x000, 0x000, 0x000, 0x003, 0x010, 0x010, 0x100,
	0x100, 0x002, 0x001, 0x001, 0x039, 0x039, 0x039, 0x039,
	0x001, 0x001
};


/********************************************************************
*  ��������  u8 WM8978_Init(void)
*  ��������: WM8978оƬ��ʼ��
*  �������: ��  
*  �������: �� 
*  ����ֵ:   WM8978��ʼ�������0����ʼ���ɹ���1����ʼ�����ɹ�     
*  ����:        
*  ����:  
*********************************************************************/
u8 WM8978_Init(void)
{
	unsigned char res = 0;

	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);	
  
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_SPI2);  //PB12,AF5  I2S_LRCK
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2);	//PB13,AF5  I2S_SCLK 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource3,GPIO_AF_SPI2);	//PC3 ,AF5  I2S_DACDATA 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_SPI2);	//PC6 ,AF5  I2S_MCK
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource2,GPIO_AF6_SPI2);	//PC2 ,AF6  I2S_ADCDATA  I2S2ext_SD��AF6!!!

	WM_IIC_Init();//��ʼ��IIC�ӿ�
	res = WM8978_Write_Reg(0, 0);//��λWM8978
	if(res)return 1;			 //����ָ��ʧ��,WM8978�쳣
	//����Ϊͨ������
	WM8978_Write_Reg(1, 0X1B);	 //R1,MICEN����Ϊ1(MICʹ��),BIASEN����Ϊ1(ģ��������),VMIDSEL[1:0]����Ϊ:11(5K)
	WM8978_Write_Reg(2, 0X1B0);	 //R2,ROUT1,LOUT1���ʹ��(�������Թ���),BOOSTENR,BOOSTENLʹ��
	WM8978_Write_Reg(3, 0X6C);	 //R3,LOUT2,ROUT2���ʹ��(���ȹ���),RMIX,LMIXʹ��	
	WM8978_Write_Reg(6, 0);		 //R6,MCLK���ⲿ�ṩ
	WM8978_Write_Reg(43, 1<<4);	 //R43,INVROUT2����,��������
	WM8978_Write_Reg(47, 1<<8);	 //R47����,PGABOOSTL,��ͨ��MIC���20������
	WM8978_Write_Reg(48, 1<<8);	 //R48����,PGABOOSTR,��ͨ��MIC���20������
	WM8978_Write_Reg(49, 1<<1);	 //R49,TSDEN,�������ȱ��� 
	WM8978_Write_Reg(10, 1<<3);	 //R10,SOFTMUTE�ر�,128x����,���SNR 
	WM8978_Write_Reg(14, 1<<3);	 //R14,ADC 128x������
	
	return 0;
} 

/********************************************************************
*  ��������  u8 WM8978_Write_Reg(u8 reg,u16 val)
*  ��������: WM8978оƬд�Ĵ�����ֵ
*  �������: reg:�Ĵ�����ַ
					  val:Ҫд��Ĵ�����ֵ 
*  �������: �� 
*  ����ֵ:   0,д�ɹ�  1��д��ʧ��     
*  ����:        
*  ����:  
*********************************************************************/
u8 WM8978_Write_Reg(u8 reg,u16 val)
{ 
	WM_IIC_Start(); 
	WM_IIC_WriteByte((WM8978_ADDR<<1) | 0);			//����������ַ+д����	 
	if(WM_IIC_Wait_ACK())	return 1;				//�ȴ�Ӧ��(�ɹ�?/ʧ��?) 
    WM_IIC_WriteByte((reg<<1) | ((val>>8)&0X01));	//д�Ĵ�����ַ+���ݵ����λ
	if(WM_IIC_Wait_ACK())return 2;					//�ȴ�Ӧ��(�ɹ�?/ʧ��?) 
	WM_IIC_WriteByte(val&0XFF);						//��������
	if(WM_IIC_Wait_ACK())	return 3;				//�ȴ�Ӧ��(�ɹ�?/ʧ��?) 
    WM_IIC_Stop();
	WM8978_REGVAL_TBL[reg] = val;					//����Ĵ���ֵ������
	return 0;	
	
}   

/********************************************************************
*  ��������  u16 WM8978_Read_Reg(u8 reg)
*  ��������: WM8978оƬ���Ĵ�����ֵ
*  �������: reg:�Ĵ�����ַ
*  �������: �� 
*  ����ֵ:   �Ĵ���ֵ     
*  ����:        
*  ����:  
*********************************************************************/
u16 WM8978_Read_Reg(u8 reg)
{  
	return WM8978_REGVAL_TBL[reg];	
} 

/********************************************************************
*  �������� void WM8978_DAC_Init(void)
*  ��������: ����WM8978оƬDAC����
*  �������: ��  
*  �������: �� 
*  ����ֵ:   ��     
*  ����:        
*  ����:  
*********************************************************************/
void WM8978_DAC_Init(void)
{
	u16 regval;
	regval=WM8978_Read_Reg(3);	//��ȡR3
	regval|=3<<0;								//R3���2��λ����Ϊ1,����DACR&DACL
	WM8978_Write_Reg(3,regval);	//����R3
	regval=WM8978_Read_Reg(2);	//��ȡR2
	
	WM8978_Write_Reg(50,0x01);  //R50����
	WM8978_Write_Reg(51,0x01);  //R51���� 	
}

/********************************************************************
*  �������� void WM8978_I2S_Mode(u8 fmt,u8 len)
*  ��������: ����WM8978оƬIIS����ģʽ
*  �������: fmt:0,LSB(�Ҷ���);1,MSB(�����);2,�����ֱ�׼I2S;3,PCM/DSP;  
						 len:0,16λ;1,20λ;2,24λ;3,32λ
*  �������: �� 
*  ����ֵ:   ��     
*  ����:        
*  ����:  
*********************************************************************/
void WM8978_I2S_Mode(u8 fmt,u8 len)
{
	fmt&=0X03;//�޶���Χ
	len&=0X03;//�޶���Χ
	WM8978_Write_Reg(4,(fmt<<3)|(len<<5));	//R4,WM8978����ģʽ����	
}	


/********************************************************************
*  �������� void WM8978_Set_Vol(u8 voll,u8 volr)
*  ��������: ���ö�������
*  �������:  voll ������������
						 volr ������������
*  �������: �� 
*  ����ֵ:   ��     
*  ����:        
*  ����:  
*********************************************************************/
void WM8978_Set_Vol(u8 voll,u8 volr)
{
	voll&=0X3F;
	volr&=0X3F;//�޶���Χ
	WM8978_Write_Reg(52,voll);				//R52,������������������
	WM8978_Write_Reg(53,volr|(1<<8));	//R53,������������������,ͬ������(HPVU=1)
}

/********************************************************************
*  �������� void WM8978_Set_Vol(u8 voll,u8 volr)
*  ��������: ������������
*  �������:  voll ������������
						 volr ������������
*  �������: �� 
*  ����ֵ:   ��     
*  ����:        
*  ����:  
*********************************************************************/
void WM8978_Set_LBVol(u8 voll,u8 volr)
{
	voll&=0X3F;
	volr&=0X3F;//�޶���Χ
	WM8978_Write_Reg(54,voll);				//R54,��������������
	WM8978_Write_Reg(55,volr|(1<<8));	//R55,������������,ͬ������(HPVU=1)
}

//���ֲ�������ʼ��
void Music_Init(void)
{
	WM8978_Init();						//��ʼ��WM8978
	//WM8978_Set_Vol(30,30);	//������������
	WM8978_Set_LBVol(30,30);
	WM8978_DAC_Init();      //DA�������
}
