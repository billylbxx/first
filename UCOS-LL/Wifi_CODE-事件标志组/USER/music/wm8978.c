#include "wm8978.h"
#include "wmiic.h"
#include "systick.h"

//WM8978寄存器值缓存区(总共58个寄存器,0~57),占用116字节内存
//因为WM8978的IIC操作不支持读操作,所以在本地保存所有寄存器值
//写WM8978寄存器时,同步更新到本地寄存器值,读寄存器时,直接返回本地保存的寄存器值.
//注意:WM8978的寄存器值是9位的,所以要用u16来存储. 
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
* 函 数 名 : WM8978_Init
* 功能说明 : 初始化wm8978
* 形    参 : 无
* 返 回 值 : 0：初始化成功，其他：错误代码
* 备    注 : 无
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
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource2,GPIO_AF6_SPI2);	//PC2 ,AF6  I2S_ADCDATA  I2S2ext_SD是AF6!!!

	WM_IIC_Init();//初始化IIC接口
	res = WM8978_Write_Reg(0, 0);//软复位WM8978
	if(res)return 1;			 //发送指令失败,WM8978异常
	//以下为通用设置
	WM8978_Write_Reg(1, 0X1B);	 //R1,MICEN设置为1(MIC使能),BIASEN设置为1(模拟器工作),VMIDSEL[1:0]设置为:11(5K)
	WM8978_Write_Reg(2, 0X1B0);	 //R2,ROUT1,LOUT1输出使能(耳机可以工作),BOOSTENR,BOOSTENL使能
	WM8978_Write_Reg(3, 0X6C);	 //R3,LOUT2,ROUT2输出使能(喇叭工作),RMIX,LMIX使能	
	WM8978_Write_Reg(6, 0);		 //R6,MCLK由外部提供
	WM8978_Write_Reg(43, 1<<4);	 //R43,INVROUT2反向,驱动喇叭
	WM8978_Write_Reg(47, 1<<8);	 //R47设置,PGABOOSTL,左通道MIC获得20倍增益
	WM8978_Write_Reg(48, 1<<8);	 //R48设置,PGABOOSTR,右通道MIC获得20倍增益
	WM8978_Write_Reg(49, 1<<1);	 //R49,TSDEN,开启过热保护 
	WM8978_Write_Reg(10, 1<<3);	 //R10,SOFTMUTE关闭,128x采样,最佳SNR 
	WM8978_Write_Reg(14, 1<<3);	 //R14,ADC 128x采样率
	
	return 0;
} 
/*********************************************************************************************************
* 函 数 名 : WM8978_Write_Reg
* 功能说明 : wm8978写寄存器
* 形    参 : reg：寄存器地址，val：要写入寄存器的值 
* 返 回 值 : 0：成功，其他：错误代码
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char WM8978_Write_Reg(unsigned char reg, unsigned short val)
{ 
	WM_IIC_Start(); 
	WM_IIC_WriteByte((WM8978_ADDR<<1) | 0);			//发送器件地址+写命令	 
	if(WM_IIC_Wait_ACK())	return 1;				//等待应答(成功?/失败?) 
    WM_IIC_WriteByte((reg<<1) | ((val>>8)&0X01));	//写寄存器地址+数据的最高位
	if(WM_IIC_Wait_ACK())return 2;					//等待应答(成功?/失败?) 
	WM_IIC_WriteByte(val&0XFF);						//发送数据
	if(WM_IIC_Wait_ACK())	return 3;				//等待应答(成功?/失败?) 
    WM_IIC_Stop();
	WM8978_REGVAL_TBL[reg] = val;					//保存寄存器值到本地
	return 0;	
}  
/*********************************************************************************************************
* 函 数 名 : WM8978_Read_Reg
* 功能说明 : WM8978读寄存器
* 形    参 : reg：寄存器地址
* 返 回 值 : 寄存器值
* 备    注 : 读取本地寄存器值缓冲区内的对应值
*********************************************************************************************************/ 
unsigned short WM8978_Read_Reg(unsigned char reg)
{  
	return WM8978_REGVAL_TBL[reg];	
} 
/*********************************************************************************************************
* 函 数 名 : WM8978_ADDA_Cfg
* 功能说明 : wm8978 DAC/ADC配置
* 形    参 : adcen：adc使能(1)/关闭(0)，dacen：dac使能(1)/关闭(0)
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void WM8978_ADDA_Cfg(unsigned char dacen, unsigned char adcen)
{
	unsigned short regval = 0;
	
	regval = WM8978_Read_Reg(3);	//读取R3
	if(dacen)	regval |= 3<<0;		//R3最低2个位设置为1,开启DACR&DACL
	else 		regval&=~(3<<0);	//R3最低2个位清零,关闭DACR&DACL.
	WM8978_Write_Reg(3, regval);	//设置R3
	
	regval = WM8978_Read_Reg(2);	//读取R2
	if(adcen)	regval |= 3<<0;		//R2最低2个位设置为1,开启ADCR&ADCL
	else 		regval &= ~(3<<0);	//R2最低2个位清零,关闭ADCR&ADCL.
	WM8978_Write_Reg(2,regval);		//设置R2	
}
/*********************************************************************************************************
* 函 数 名 : WM8978_Input_Cfg
* 功能说明 : wm8978输入通道配置 
* 形    参 : micen：MIC开启(1)/关闭(0)，lineinen：Line In开启(1)/关闭(0)，auxen：aux开启(1)/关闭(0) 
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void WM8978_Input_Cfg(unsigned char micen, unsigned char lineinen, unsigned char auxen)
{
	unsigned short regval = 0;  
	
	regval = WM8978_Read_Reg(2);		//读取R2
	if(micen)	regval |= 3<<2;			//开启INPPGAENR,INPPGAENL(MIC的PGA放大)
	else 		regval &= ~(3<<2);		//关闭INPPGAENR,INPPGAENL.
 	WM8978_Write_Reg(2, regval);		//设置R2 
	
	regval = WM8978_Read_Reg(44);		//读取R44
	if(micen)	regval |= 3<<4|3<<0;	//开启LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
	else 		regval &= ~(3<<4|3<<0);	//关闭LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
	WM8978_Write_Reg(44, regval);		//设置R44
	
	if(lineinen)WM8978_LINEIN_Gain(5);	//LINE IN 0dB增益
	else 		WM8978_LINEIN_Gain(0);	//关闭LINE IN
	if(auxen)	WM8978_AUX_Gain(7);		//AUX 6dB增益
	else 		WM8978_AUX_Gain(0);		//关闭AUX输入  
}
/*********************************************************************************************************
* 函 数 名 : WM8978_Output_Cfg
* 功能说明 : wm8978输出配置 
* 形    参 : dacen：DAC输出(放音)开启(1)/关闭(0)，bpsen：Bypass输出(录音,包括MIC,LINE IN,AUX等)开启(1)/关闭(0) 
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void WM8978_Output_Cfg(unsigned char dacen, unsigned char bpsen)
{
	unsigned short regval = 0;
	
	if(dacen)	regval |= 1<<0;		//DAC输出使能
	if(bpsen)
	{
		regval |= 1<<1;				//BYPASS使能
		regval |= 5<<2;				//0dB增益
	} 
	WM8978_Write_Reg(50, regval);	//R50设置
	WM8978_Write_Reg(51, regval);	//R51设置 
}
/*********************************************************************************************************
* 函 数 名 : WM8978_MIC_Gain
* 功能说明 : wm8978 MIC增益设置(不包括BOOST的20dB，MIC-->ADC输入部分的增益)
* 形    参 : gain：0~63，对应-12dB~35.25dB，0.75dB/Step
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void WM8978_MIC_Gain(unsigned char gain)
{
	gain &= 0X3F;
	WM8978_Write_Reg(45, gain);			//R45,左通道PGA设置 
	WM8978_Write_Reg(46, gain|1<<8);	//R46,右通道PGA设置
}
/*********************************************************************************************************
* 函 数 名 : WM8978_LINEIN_Gain
* 功能说明 : WM8978 L2/R2(也就是Line In)增益设置(L2/R2-->ADC输入部分的增益)
* 形    参 : gain：0~7，0表示通道禁止，1~7对应-12dB~6dB，3dB/Step
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void WM8978_LINEIN_Gain(unsigned char gain)
{
	unsigned short regval = 0;
	
	gain &= 0X07;
	regval = WM8978_Read_Reg(47);			//读取R47
	regval &= ~(7<<4);						//清除原来的设置 
 	WM8978_Write_Reg(47, regval|gain<<4);	//设置R47
	regval = WM8978_Read_Reg(48);			//读取R48
	regval &= ~(7<<4);						//清除原来的设置 
 	WM8978_Write_Reg(48, regval|gain<<4);	//设置R48
} 
/*********************************************************************************************************
* 函 数 名 : WM8978_AUX_Gain
* 功能说明 : WM8978 AUXR,AUXL(PWM音频部分)增益设置(AUXR/L-->ADC输入部分的增益)
* 形    参 : gain：0~7，0表示通道禁止，1~7对应-12dB~6dB，3dB/Step
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void WM8978_AUX_Gain(unsigned char gain)
{
	unsigned short regval = 0;
	
	gain &= 0X07;
	regval = WM8978_Read_Reg(47);			//读取R47
	regval &= ~(7<<0);						//清除原来的设置 
 	WM8978_Write_Reg(47, regval|gain<<0);	//设置R47
	regval=WM8978_Read_Reg(48);				//读取R48
	regval &= ~(7<<0);						//清除原来的设置 
 	WM8978_Write_Reg(48, regval|gain<<0);	//设置R48
}  
/*********************************************************************************************************
* 函 数 名 : WM8978_I2S_Cfg
* 功能说明 : 设置I2S工作模式
* 形    参 : fmt：0，LSB(右对齐)；1，MSB(左对齐)；2，飞利浦标准I2S；3，PCM/DSP；
			 len：0，16位；1，20位；2，24位；3，32位；  
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void WM8978_I2S_Cfg(unsigned char fmt, unsigned char len)
{
	fmt &= 0X03;
	len &= 0X03;//限定范围
	WM8978_Write_Reg(4, (fmt<<3)|(len<<5));	//R4，WM8978工作模式设置	
}	
/*********************************************************************************************************
* 函 数 名 : WM8978_HPvol_Set
* 功能说明 : 设置耳机左右声道音量
* 形    参 : voll：左声道音量(0~63)；volr：右声道音量(0~63)
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void WM8978_HPvol_Set(unsigned char voll, unsigned char volr)
{
	voll &= 0X3F;
	volr &= 0X3F;						//限定范围
	if(voll==0)	voll |= 1<<6;			//音量为0时,直接mute
	if(volr==0)	volr |= 1<<6;			//音量为0时,直接mute 
	WM8978_Write_Reg(52,voll);			//R52,耳机左声道音量设置
	WM8978_Write_Reg(53,volr|(1<<8));	//R53,耳机右声道音量设置,同步更新(HPVU=1)
}
/*********************************************************************************************************
* 函 数 名 : WM8978_SPKvol_Set
* 功能说明 : 设置喇叭音量
* 形    参 : voll：左声道音量(0~63) 
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void WM8978_SPKvol_Set(u8 volx)
{ 
	volx &= 0X3F;						//限定范围
	if(volx == 0)	volx |= 1<<6;		//音量为0时，直接mute 
 	WM8978_Write_Reg(54, volx);			//R54，喇叭左声道音量设置
	WM8978_Write_Reg(55, volx|(1<<8));	//R55，喇叭右声道音量设置，同步更新(SPKVU=1)	
}
/*********************************************************************************************************
* 函 数 名 : WM8978_3D_Set
* 功能说明 : 设置3D环绕声
* 形    参 : depth：0~15(3D强度，0最弱，15最强)
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void WM8978_3D_Set(unsigned char depth)
{ 
	depth &= 0XF;					//限定范围 
 	WM8978_Write_Reg(41, depth);	//R41,3D环绕设置 	
}
/*********************************************************************************************************
* 函 数 名 : WM8978_EQ_3D_Dir
* 功能说明 : 设置EQ/3D作用方向
* 形    参 : dir：0，在ADC起作用；1，在DAC起作用(默认)
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void WM8978_EQ_3D_Dir(unsigned char dir)
{
	unsigned short regval = 0; 
	
	regval = WM8978_Read_Reg(0X12);
	if(dir)	regval |= 1<<8;
	else 	regval &= ~(1<<8); 
 	WM8978_Write_Reg(18, regval);	//R18，EQ1的第9位控制EQ/3D方向
}
/*********************************************************************************************************
* 函 数 名 : WM8978_EQ1_Set
* 功能说明 : 设置EQ1
* 形    参 : cfreq：截止频率，0~3分别对应80/105/135/175Hz；gain：增益，0~24对应-12~+12dB
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void WM8978_EQ1_Set(unsigned char cfreq, unsigned char gain)
{ 
	unsigned short regval = 0;
	
	cfreq &= 0X3;					//限定范围 
	if(gain > 24) gain = 24;
	gain = 24 - gain;
	regval = WM8978_Read_Reg(18);
	regval &= 0X100;
	regval |= cfreq<<5;				//设置截止频率 
	regval |= gain;					//设置增益	
 	WM8978_Write_Reg(18, regval);	//R18,EQ1设置 	
}
/*********************************************************************************************************
* 函 数 名 : WM8978_EQ2_Set
* 功能说明 : 设置EQ2
* 形    参 : cfreq：中心频率，0~3分别对应230/300/385/500Hz；gain：增益，0~24对应-12~+12dB
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void WM8978_EQ2_Set(unsigned char cfreq, unsigned char gain)
{ 
	unsigned short regval = 0;
	
	cfreq &= 0X3;					//限定范围 
	if(gain > 24) gain = 24;
	gain = 24 - gain; 
	regval |= cfreq<<5;				//设置截止频率 
	regval |= gain;					//设置增益	
 	WM8978_Write_Reg(19, regval);	//R19,EQ2设置 	
}
/*********************************************************************************************************
* 函 数 名 : WM8978_EQ3_Set
* 功能说明 : 设置EQ3
* 形    参 : cfreq：中心频率，0~3分别对应650/850/1100/1400Hz；gain：增益，0~24对应-12~+12dB
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void WM8978_EQ3_Set(unsigned char cfreq, unsigned char gain)
{ 
	unsigned short regval = 0;
	
	cfreq &= 0X3;					//限定范围 
	if(gain > 24) gain = 24;
	gain = 24 - gain; 
	regval |= cfreq<<5;				//设置截止频率 
	regval |= gain;					//设置增益	
 	WM8978_Write_Reg(20, regval);	//R20,EQ3设置 	
}
/*********************************************************************************************************
* 函 数 名 : WM8978_EQ4_Set
* 功能说明 : 设置EQ4
* 形    参 : cfreq：中心频率，0~3分别对应1800/2400/3200/4100Hz；gain：增益，0~24对应-12~+12dB
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void WM8978_EQ4_Set(unsigned char cfreq, unsigned char gain)
{ 
	unsigned short regval = 0;
	
	cfreq &= 0X3;					//限定范围 
	if(gain > 24) gain = 24;
	gain = 24-gain; 
	regval |= cfreq<<5;				//设置截止频率 
	regval |= gain;					//设置增益	
 	WM8978_Write_Reg(21, regval);	//R21,EQ4设置 	
}
/*********************************************************************************************************
* 函 数 名 : WM8978_EQ5_Set
* 功能说明 : 设置EQ5
* 形    参 : cfreq：中心频率，0~3分别对应5300/6900/9000/11700Hz；gain：增益，0~24对应-12~+12dB
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void WM8978_EQ5_Set(unsigned char cfreq, unsigned char gain)
{ 
	unsigned short regval = 0;
	
	cfreq &= 0X3;					//限定范围 
	if(gain > 24) gain = 24;
	gain = 24 - gain; 
	regval |= cfreq<<5;				//设置截止频率 
	regval |= gain;					//设置增益	
 	WM8978_Write_Reg(22, regval);	//R22,EQ5设置 	
}

