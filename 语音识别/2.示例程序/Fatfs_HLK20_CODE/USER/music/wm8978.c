#include "wm8978.h"
#include "wmiic.h"
#include "systick.h"

//WM8978�Ĵ���ֵ������(�ܹ�58���Ĵ���,0~57),ռ��116�ֽ��ڴ�
//��ΪWM8978��IIC������֧�ֶ�����,�����ڱ��ر������мĴ���ֵ
//дWM8978�Ĵ���ʱ,ͬ�����µ����ؼĴ���ֵ,���Ĵ���ʱ,ֱ�ӷ��ر��ر���ļĴ���ֵ.
//ע��:WM8978�ļĴ���ֵ��9λ��,����Ҫ��u16���洢. 
static unsigned short WM8978_REGVAL_TBL[58]=
{
	0X0000,0X0000,0X0000,0X0000,0X0050,0X0000,0X0140,0X0000,
	0X0000,0X0000,0X0000,0X00FF,0X00FF,0X0000,0X0100,0X00FF,
	0X00FF,0X0000,0X012C,0X002C,0X002C,0X002C,0X002C,0X0000,
	0X0032,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
	0X0038,0X000B,0X0032,0X0000,0X0008,0X000C,0X0093,0X00E9,
	0X0000,0X0000,0X0000,0X0000,0X0003,0X0010,0X0010,0X0100,
	0X0100,0X0002,0X0001,0X0001,0X0039,0X0039,0X0039,0X0039,
	0X0001,0X0001
}; 
/*********************************************************************************************************
* �� �� �� : WM8978_Init
* ����˵�� : ��ʼ��wm8978
* ��    �� : ��
* �� �� ֵ : 0����ʼ���ɹ����������������
* ��    ע : ��
*********************************************************************************************************/ 
unsigned char WM8978_Init(void)
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
/*********************************************************************************************************
* �� �� �� : WM8978_Write_Reg
* ����˵�� : wm8978д�Ĵ���
* ��    �� : reg���Ĵ�����ַ��val��Ҫд��Ĵ�����ֵ 
* �� �� ֵ : 0���ɹ����������������
* ��    ע : ��
*********************************************************************************************************/ 
unsigned char WM8978_Write_Reg(unsigned char reg, unsigned short val)
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
/*********************************************************************************************************
* �� �� �� : WM8978_Read_Reg
* ����˵�� : WM8978���Ĵ���
* ��    �� : reg���Ĵ�����ַ
* �� �� ֵ : �Ĵ���ֵ
* ��    ע : ��ȡ���ؼĴ���ֵ�������ڵĶ�Ӧֵ
*********************************************************************************************************/ 
unsigned short WM8978_Read_Reg(unsigned char reg)
{  
	return WM8978_REGVAL_TBL[reg];	
} 
/*********************************************************************************************************
* �� �� �� : WM8978_ADDA_Cfg
* ����˵�� : wm8978 DAC/ADC����
* ��    �� : adcen��adcʹ��(1)/�ر�(0)��dacen��dacʹ��(1)/�ر�(0)
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void WM8978_ADDA_Cfg(unsigned char dacen, unsigned char adcen)
{
	unsigned short regval = 0;
	
	regval = WM8978_Read_Reg(3);	//��ȡR3
	if(dacen)	regval |= 3<<0;		//R3���2��λ����Ϊ1,����DACR&DACL
	else 		regval&=~(3<<0);	//R3���2��λ����,�ر�DACR&DACL.
	WM8978_Write_Reg(3, regval);	//����R3
	
	regval = WM8978_Read_Reg(2);	//��ȡR2
	if(adcen)	regval |= 3<<0;		//R2���2��λ����Ϊ1,����ADCR&ADCL
	else 		regval &= ~(3<<0);	//R2���2��λ����,�ر�ADCR&ADCL.
	WM8978_Write_Reg(2,regval);		//����R2	
}
/*********************************************************************************************************
* �� �� �� : WM8978_Input_Cfg
* ����˵�� : wm8978����ͨ������ 
* ��    �� : micen��MIC����(1)/�ر�(0)��lineinen��Line In����(1)/�ر�(0)��auxen��aux����(1)/�ر�(0) 
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void WM8978_Input_Cfg(unsigned char micen, unsigned char lineinen, unsigned char auxen)
{
	unsigned short regval = 0;  
	
	regval = WM8978_Read_Reg(2);		//��ȡR2
	if(micen)	regval |= 3<<2;			//����INPPGAENR,INPPGAENL(MIC��PGA�Ŵ�)
	else 		regval &= ~(3<<2);		//�ر�INPPGAENR,INPPGAENL.
 	WM8978_Write_Reg(2, regval);		//����R2 
	
	regval = WM8978_Read_Reg(44);		//��ȡR44
	if(micen)	regval |= 3<<4|3<<0;	//����LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
	else 		regval &= ~(3<<4|3<<0);	//�ر�LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
	WM8978_Write_Reg(44, regval);		//����R44
	
	if(lineinen)WM8978_LINEIN_Gain(5);	//LINE IN 0dB����
	else 		WM8978_LINEIN_Gain(0);	//�ر�LINE IN
	if(auxen)	WM8978_AUX_Gain(7);		//AUX 6dB����
	else 		WM8978_AUX_Gain(0);		//�ر�AUX����  
}
/*********************************************************************************************************
* �� �� �� : WM8978_Output_Cfg
* ����˵�� : wm8978������� 
* ��    �� : dacen��DAC���(����)����(1)/�ر�(0)��bpsen��Bypass���(¼��,����MIC,LINE IN,AUX��)����(1)/�ر�(0) 
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void WM8978_Output_Cfg(unsigned char dacen, unsigned char bpsen)
{
	unsigned short regval = 0;
	
	if(dacen)	regval |= 1<<0;		//DAC���ʹ��
	if(bpsen)
	{
		regval |= 1<<1;				//BYPASSʹ��
		regval |= 5<<2;				//0dB����
	} 
	WM8978_Write_Reg(50, regval);	//R50����
	WM8978_Write_Reg(51, regval);	//R51���� 
}
/*********************************************************************************************************
* �� �� �� : WM8978_MIC_Gain
* ����˵�� : wm8978 MIC��������(������BOOST��20dB��MIC-->ADC���벿�ֵ�����)
* ��    �� : gain��0~63����Ӧ-12dB~35.25dB��0.75dB/Step
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void WM8978_MIC_Gain(unsigned char gain)
{
	gain &= 0X3F;
	WM8978_Write_Reg(45, gain);			//R45,��ͨ��PGA���� 
	WM8978_Write_Reg(46, gain|1<<8);	//R46,��ͨ��PGA����
}
/*********************************************************************************************************
* �� �� �� : WM8978_LINEIN_Gain
* ����˵�� : WM8978 L2/R2(Ҳ����Line In)��������(L2/R2-->ADC���벿�ֵ�����)
* ��    �� : gain��0~7��0��ʾͨ����ֹ��1~7��Ӧ-12dB~6dB��3dB/Step
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void WM8978_LINEIN_Gain(unsigned char gain)
{
	unsigned short regval = 0;
	
	gain &= 0X07;
	regval = WM8978_Read_Reg(47);			//��ȡR47
	regval &= ~(7<<4);						//���ԭ�������� 
 	WM8978_Write_Reg(47, regval|gain<<4);	//����R47
	regval = WM8978_Read_Reg(48);			//��ȡR48
	regval &= ~(7<<4);						//���ԭ�������� 
 	WM8978_Write_Reg(48, regval|gain<<4);	//����R48
} 
/*********************************************************************************************************
* �� �� �� : WM8978_AUX_Gain
* ����˵�� : WM8978 AUXR,AUXL(PWM��Ƶ����)��������(AUXR/L-->ADC���벿�ֵ�����)
* ��    �� : gain��0~7��0��ʾͨ����ֹ��1~7��Ӧ-12dB~6dB��3dB/Step
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void WM8978_AUX_Gain(unsigned char gain)
{
	unsigned short regval = 0;
	
	gain &= 0X07;
	regval = WM8978_Read_Reg(47);			//��ȡR47
	regval &= ~(7<<0);						//���ԭ�������� 
 	WM8978_Write_Reg(47, regval|gain<<0);	//����R47
	regval=WM8978_Read_Reg(48);				//��ȡR48
	regval &= ~(7<<0);						//���ԭ�������� 
 	WM8978_Write_Reg(48, regval|gain<<0);	//����R48
}  
/*********************************************************************************************************
* �� �� �� : WM8978_I2S_Cfg
* ����˵�� : ����I2S����ģʽ
* ��    �� : fmt��0��LSB(�Ҷ���)��1��MSB(�����)��2�������ֱ�׼I2S��3��PCM/DSP��
			 len��0��16λ��1��20λ��2��24λ��3��32λ��  
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void WM8978_I2S_Cfg(unsigned char fmt, unsigned char len)
{
	fmt &= 0X03;
	len &= 0X03;//�޶���Χ
	WM8978_Write_Reg(4, (fmt<<3)|(len<<5));	//R4��WM8978����ģʽ����	
}	
/*********************************************************************************************************
* �� �� �� : WM8978_HPvol_Set
* ����˵�� : ���ö���������������
* ��    �� : voll������������(0~63)��volr������������(0~63)
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void WM8978_HPvol_Set(unsigned char voll, unsigned char volr)
{
	voll &= 0X3F;
	volr &= 0X3F;						//�޶���Χ
	if(voll==0)	voll |= 1<<6;			//����Ϊ0ʱ,ֱ��mute
	if(volr==0)	volr |= 1<<6;			//����Ϊ0ʱ,ֱ��mute 
	WM8978_Write_Reg(52,voll);			//R52,������������������
	WM8978_Write_Reg(53,volr|(1<<8));	//R53,������������������,ͬ������(HPVU=1)
}
/*********************************************************************************************************
* �� �� �� : WM8978_SPKvol_Set
* ����˵�� : ������������
* ��    �� : voll������������(0~63) 
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void WM8978_SPKvol_Set(u8 volx)
{ 
	volx &= 0X3F;						//�޶���Χ
	if(volx == 0)	volx |= 1<<6;		//����Ϊ0ʱ��ֱ��mute 
 	WM8978_Write_Reg(54, volx);			//R54��������������������
	WM8978_Write_Reg(55, volx|(1<<8));	//R55�������������������ã�ͬ������(SPKVU=1)	
}
/*********************************************************************************************************
* �� �� �� : WM8978_3D_Set
* ����˵�� : ����3D������
* ��    �� : depth��0~15(3Dǿ�ȣ�0������15��ǿ)
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void WM8978_3D_Set(unsigned char depth)
{ 
	depth &= 0XF;					//�޶���Χ 
 	WM8978_Write_Reg(41, depth);	//R41,3D�������� 	
}
/*********************************************************************************************************
* �� �� �� : WM8978_EQ_3D_Dir
* ����˵�� : ����EQ/3D���÷���
* ��    �� : dir��0����ADC�����ã�1����DAC������(Ĭ��)
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void WM8978_EQ_3D_Dir(unsigned char dir)
{
	unsigned short regval = 0; 
	
	regval = WM8978_Read_Reg(0X12);
	if(dir)	regval |= 1<<8;
	else 	regval &= ~(1<<8); 
 	WM8978_Write_Reg(18, regval);	//R18��EQ1�ĵ�9λ����EQ/3D����
}
/*********************************************************************************************************
* �� �� �� : WM8978_EQ1_Set
* ����˵�� : ����EQ1
* ��    �� : cfreq����ֹƵ�ʣ�0~3�ֱ��Ӧ80/105/135/175Hz��gain�����棬0~24��Ӧ-12~+12dB
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void WM8978_EQ1_Set(unsigned char cfreq, unsigned char gain)
{ 
	unsigned short regval = 0;
	
	cfreq &= 0X3;					//�޶���Χ 
	if(gain > 24) gain = 24;
	gain = 24 - gain;
	regval = WM8978_Read_Reg(18);
	regval &= 0X100;
	regval |= cfreq<<5;				//���ý�ֹƵ�� 
	regval |= gain;					//��������	
 	WM8978_Write_Reg(18, regval);	//R18,EQ1���� 	
}
/*********************************************************************************************************
* �� �� �� : WM8978_EQ2_Set
* ����˵�� : ����EQ2
* ��    �� : cfreq������Ƶ�ʣ�0~3�ֱ��Ӧ230/300/385/500Hz��gain�����棬0~24��Ӧ-12~+12dB
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void WM8978_EQ2_Set(unsigned char cfreq, unsigned char gain)
{ 
	unsigned short regval = 0;
	
	cfreq &= 0X3;					//�޶���Χ 
	if(gain > 24) gain = 24;
	gain = 24 - gain; 
	regval |= cfreq<<5;				//���ý�ֹƵ�� 
	regval |= gain;					//��������	
 	WM8978_Write_Reg(19, regval);	//R19,EQ2���� 	
}
/*********************************************************************************************************
* �� �� �� : WM8978_EQ3_Set
* ����˵�� : ����EQ3
* ��    �� : cfreq������Ƶ�ʣ�0~3�ֱ��Ӧ650/850/1100/1400Hz��gain�����棬0~24��Ӧ-12~+12dB
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void WM8978_EQ3_Set(unsigned char cfreq, unsigned char gain)
{ 
	unsigned short regval = 0;
	
	cfreq &= 0X3;					//�޶���Χ 
	if(gain > 24) gain = 24;
	gain = 24 - gain; 
	regval |= cfreq<<5;				//���ý�ֹƵ�� 
	regval |= gain;					//��������	
 	WM8978_Write_Reg(20, regval);	//R20,EQ3���� 	
}
/*********************************************************************************************************
* �� �� �� : WM8978_EQ4_Set
* ����˵�� : ����EQ4
* ��    �� : cfreq������Ƶ�ʣ�0~3�ֱ��Ӧ1800/2400/3200/4100Hz��gain�����棬0~24��Ӧ-12~+12dB
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void WM8978_EQ4_Set(unsigned char cfreq, unsigned char gain)
{ 
	unsigned short regval = 0;
	
	cfreq &= 0X3;					//�޶���Χ 
	if(gain > 24) gain = 24;
	gain = 24-gain; 
	regval |= cfreq<<5;				//���ý�ֹƵ�� 
	regval |= gain;					//��������	
 	WM8978_Write_Reg(21, regval);	//R21,EQ4���� 	
}
/*********************************************************************************************************
* �� �� �� : WM8978_EQ5_Set
* ����˵�� : ����EQ5
* ��    �� : cfreq������Ƶ�ʣ�0~3�ֱ��Ӧ5300/6900/9000/11700Hz��gain�����棬0~24��Ӧ-12~+12dB
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void WM8978_EQ5_Set(unsigned char cfreq, unsigned char gain)
{ 
	unsigned short regval = 0;
	
	cfreq &= 0X3;					//�޶���Χ 
	if(gain > 24) gain = 24;
	gain = 24 - gain; 
	regval |= cfreq<<5;				//���ý�ֹƵ�� 
	regval |= gain;					//��������	
 	WM8978_Write_Reg(22, regval);	//R22,EQ5���� 	
}

