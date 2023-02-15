#include "wm8978.h"
#include "wmiic.h"

/*
	wm8978寄存器缓存
	由于WM8978的I2C两线接口不支持读取操作，因此寄存器值缓存在内存中，当写寄存器时同步更新缓存，读寄存器时
	直接返回缓存中的值。
	寄存器MAP 在WM8978.pdf 的第67页，寄存器地址是7bit， 寄存器数据是9bit
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
*  函数名：  u8 WM8978_Init(void)
*  功能描述: WM8978芯片初始化
*  输入参数: 无  
*  输出参数: 无 
*  返回值:   WM8978初始化结果。0：初始化成功。1：初始化不成功     
*  其他:        
*  作者:  
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

/********************************************************************
*  函数名：  u8 WM8978_Write_Reg(u8 reg,u16 val)
*  功能描述: WM8978芯片写寄存器的值
*  输入参数: reg:寄存器地址
					  val:要写入寄存器的值 
*  输出参数: 无 
*  返回值:   0,写成功  1，写放失败     
*  其他:        
*  作者:  
*********************************************************************/
u8 WM8978_Write_Reg(u8 reg,u16 val)
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

/********************************************************************
*  函数名：  u16 WM8978_Read_Reg(u8 reg)
*  功能描述: WM8978芯片读寄存器的值
*  输入参数: reg:寄存器地址
*  输出参数: 无 
*  返回值:   寄存器值     
*  其他:        
*  作者:  
*********************************************************************/
u16 WM8978_Read_Reg(u8 reg)
{  
	return WM8978_REGVAL_TBL[reg];	
} 

/********************************************************************
*  函数名： void WM8978_DAC_Init(void)
*  功能描述: 开启WM8978芯片DAC功能
*  输入参数: 无  
*  输出参数: 无 
*  返回值:   无     
*  其他:        
*  作者:  
*********************************************************************/
void WM8978_DAC_Init(void)
{
	u16 regval;
	regval=WM8978_Read_Reg(3);	//读取R3
	regval|=3<<0;								//R3最低2个位设置为1,开启DACR&DACL
	WM8978_Write_Reg(3,regval);	//设置R3
	regval=WM8978_Read_Reg(2);	//读取R2
	
	WM8978_Write_Reg(50,0x01);  //R50设置
	WM8978_Write_Reg(51,0x01);  //R51设置 	
}

/********************************************************************
*  函数名： void WM8978_I2S_Mode(u8 fmt,u8 len)
*  功能描述: 设置WM8978芯片IIS工作模式
*  输入参数: fmt:0,LSB(右对齐);1,MSB(左对齐);2,飞利浦标准I2S;3,PCM/DSP;  
						 len:0,16位;1,20位;2,24位;3,32位
*  输出参数: 无 
*  返回值:   无     
*  其他:        
*  作者:  
*********************************************************************/
void WM8978_I2S_Mode(u8 fmt,u8 len)
{
	fmt&=0X03;//限定范围
	len&=0X03;//限定范围
	WM8978_Write_Reg(4,(fmt<<3)|(len<<5));	//R4,WM8978工作模式设置	
}	


/********************************************************************
*  函数名： void WM8978_Set_Vol(u8 voll,u8 volr)
*  功能描述: 设置耳机音量
*  输入参数:  voll ，左声道音量
						 volr ，右声道音量
*  输出参数: 无 
*  返回值:   无     
*  其他:        
*  作者:  
*********************************************************************/
void WM8978_Set_Vol(u8 voll,u8 volr)
{
	voll&=0X3F;
	volr&=0X3F;//限定范围
	WM8978_Write_Reg(52,voll);				//R52,耳机左声道音量设置
	WM8978_Write_Reg(53,volr|(1<<8));	//R53,耳机右声道音量设置,同步更新(HPVU=1)
}

/********************************************************************
*  函数名： void WM8978_Set_Vol(u8 voll,u8 volr)
*  功能描述: 设置喇叭音量
*  输入参数:  voll ，左声道音量
						 volr ，右声道音量
*  输出参数: 无 
*  返回值:   无     
*  其他:        
*  作者:  
*********************************************************************/
void WM8978_Set_LBVol(u8 voll,u8 volr)
{
	voll&=0X3F;
	volr&=0X3F;//限定范围
	WM8978_Write_Reg(54,voll);				//R54,左声道音量设置
	WM8978_Write_Reg(55,volr|(1<<8));	//R55,声道音量设置,同步更新(HPVU=1)
}

//音乐播放器初始化
void Music_Init(void)
{
	WM8978_Init();						//初始化WM8978
	//WM8978_Set_Vol(30,30);	//耳机音量设置
	WM8978_Set_LBVol(30,30);
	WM8978_DAC_Init();      //DA相关配置
}
