#include "stm32f4xx.h" 
#include "sd.h"
#include "string.h"	 
#include "usart1.h"	 
#include "led.h"

//关闭所有中断(但是不包括fault和NMI中断)
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
//开启所有中断
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}

/*
volatile提醒编译器它后面所定义的变量随时都有可能改变，
因此编译后的程序每次需要存储或读取这个变量的时候，
都会直接从变量地址中读取数据。如果没有volatile关键字，
则编译器可能优化读取和存储，可能暂时使用寄存器中的值，
如果这个变量由别的程序更新了的话，将出现不一致的现象。
*/

SDIO_DataInitTypeDef SDIO_DataInitStruct;
SDIO_InitTypeDef 	 SDIO_InitStruct;
SDIO_CmdInitTypeDef  SDIO_CmdInitStruct;

static unsigned char CardType = SDIO_STD_CAPACITY_SD_CARD_V1_1;		//SD卡类型（默认为1.x卡）
static unsigned int CSD_Tab[4], CID_Tab[4], RCA = 0;				//SD卡CSD、CID以及相对地址(RCA)数据
static unsigned char DeviceMode = SD_DMA_MODE;		   				//工作模式，注意：工作模式必须通过SD_SetDeviceMode后才算数。这里只是定义一个默认的模式(SD_DMA_MODE)
static unsigned char StopCondition = 0; 							//是否发送停止传输标志位，DMA多块读写的时候用到  
volatile SD_Error TransferError = SD_OK;							//数据传输错误标志，DMA读写时使用	    
volatile unsigned char TransferEnd = 0;								//传输结束标志，DMA读写时使用
SD_CardInfo SDCardInfo;												//SD卡信息

//SD_ReadDisk/SD_WriteDisk函数专用buf，当这两个函数的数据缓存区地址不是4字节对齐的时候，
//需要用到该数组，确保数据缓存区地址是4字节对齐的。
__align(4) unsigned char SDIO_DATA_BUFFER[512];						  
 
//初始化SD卡
//返回值:错误代码;(0,无错误)
SD_Error SD_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	SD_Error errorstatus = SD_OK;	   
 	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SDIO, ENABLE);
	
	//配置PC口,复用推挽输出、无上下拉、100M
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruct);

	//配置PD口,复用推挽输出、无上下拉、100M
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD,&GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8, GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource9, GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10, GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11, GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12, GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2, GPIO_AF_SDIO);
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SDIO, DISABLE);
	
	//SDIO外设寄存器设置为默认值 	
 	SDIO->POWER = 0x00000000;
	SDIO->CLKCR =0x00000000;
	SDIO->ARG = 0x00000000;
	SDIO->CMD = 0x00000000;
	SDIO->DTIMER = 0x00000000;
	SDIO->DLEN = 0x00000000;
	SDIO->DCTRL = 0x00000000;
	SDIO->ICR = 0x00C007FF;
	SDIO->MASK = 0x00000000;	 										   
	
	//NVIC配置
	NVIC_InitStruct.NVIC_IRQChannel = SDIO_IRQn;			//中断源
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//使能中断源
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级0
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;			//响应优先级0
	NVIC_Init(&NVIC_InitStruct);	
	
	errorstatus = SD_PowerON();						//SD卡上电
 	if(errorstatus == SD_OK){
		errorstatus = SD_InitializeCards();			//初始化SD卡	
	}		
	if(errorstatus == SD_OK){
		errorstatus = SD_GetCardInfo(&SDCardInfo);	//获取卡信息
	}
 	if(errorstatus == SD_OK){
		errorstatus = SD_SelectDeselect((unsigned int)(SDCardInfo.RCA << 16));	//选中SD卡   
	}
	if(errorstatus == SD_OK){
		errorstatus = SD_EnableWideBusOperation(1);	//4位宽度,如果是MMC卡，则不能用4位模式 
	}
	if((errorstatus==SD_OK) || (SDIO_MULTIMEDIA_CARD==CardType)){  		    
		SDIO_Clock_Set(SDIO_TRANSFER_CLK_DIV);			//设置时钟频率,SDIO时钟计算公式:SDIO_CK时钟=SDIOCLK/[clkdiv+2];其中,SDIOCLK固定为48Mhz 
		//errorstatus=SD_SetDeviceMode(SD_DMA_MODE);	//设置为DMA模式
		errorstatus=SD_SetDeviceMode(SD_POLLING_MODE);	//设置为查询模式
 	}
	return errorstatus;		 
}
//SDIO时钟初始化设置
//clkdiv:时钟分频系数
//CK时钟=SDIOCLK/[clkdiv+2];(SDIOCLK时钟固定为48Mhz)
void SDIO_Clock_Set(unsigned char clkdiv)
{
	unsigned int tmpreg = SDIO->CLKCR; 
	tmpreg &= 0XFFFFFF00; 
 	tmpreg |= clkdiv;   
	SDIO->CLKCR = tmpreg;
} 
//卡上电
//查询所有SDIO接口上的卡设备,并查询其电压和配置时钟
//返回值:错误代码;(0,无错误)
SD_Error SD_PowerON(void)
{
 	unsigned char i = 0;
	SD_Error errorstatus = SD_OK;
	unsigned int response = 0, count = 0;
	unsigned int validvoltage = 0;  			//电压支持判断，如果1 SD卡启动成功
	unsigned int SDType = SD_STD_CAPACITY;		//标准容量 ACMD41
	/*初始化时的时钟不能大于400KHz*/ 
	SDIO_InitStruct.SDIO_ClockDiv = SDIO_INIT_CLK_DIV;							/* SDIOCLK = 48MHz, SDIO_CK = HCLK/(118 + 2) = 400 KHz */
	SDIO_InitStruct.SDIO_ClockEdge = SDIO_ClockEdge_Rising;						//在主时钟 SDIOCLK 的上升沿产生 SDIO_CK
	SDIO_InitStruct.SDIO_ClockBypass = SDIO_ClockBypass_Disable;  				//不使用bypass模式，直接用HCLK进行分频得到SDIO_CK
	SDIO_InitStruct.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;			// 空闲时不关闭时钟电源
	SDIO_InitStruct.SDIO_BusWide = SDIO_BusWide_1b;	 							//1位数据线
	SDIO_InitStruct.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;//硬件流
	SDIO_Init(&SDIO_InitStruct);

	SDIO_SetPowerState(SDIO_PowerState_ON);	//上电状态,开启卡时钟   
	
	SDIO->CLKCR |= 1<<8;					//SDIOCK使能   
	for(i=0; i<74; i++){
		SDIO_CmdInitStruct.SDIO_Argument = 0x0;						//发送CMD0进入IDLE STAGE模式命令.
		SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_GO_IDLE_STATE; 	//cmd0
		SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_No;  		//无响应
		SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;				//无等待
		SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;  			//则CPSM在开始发送命令之前等待数据传输结束。 
		SDIO_SendCommand(&SDIO_CmdInitStruct);	  					//写命令进命令寄存器											  
		errorstatus = CmdError();
		if(errorstatus == SD_OK){
			break;
		}
 	}
 	if(errorstatus){
		return errorstatus;//返回错误状态
	}
	SDIO_CmdInitStruct.SDIO_Argument = SD_CHECK_PATTERN;	//发送CMD8,短响应,检查SD卡接口特性
	SDIO_CmdInitStruct.SDIO_CmdIndex = SDIO_SEND_IF_COND;	//cmd8
	SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;	//r7
	SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;			//关闭等待中断
	SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;		//写命令进命令寄存器
	SDIO_SendCommand(&SDIO_CmdInitStruct);
	errorstatus = CmdResp7Error();							//等待R7响应
 	if(errorstatus == SD_OK){ 								//R7响应正常
	
		CardType = SDIO_STD_CAPACITY_SD_CARD_V2_0;			//SD 2.0卡
		SDType = SD_HIGH_CAPACITY;			   		  		//高容量卡
	}
	SDIO_CmdInitStruct.SDIO_Argument = 0x00;				//发送CMD55,短响应	
    SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_APP_CMD;		//cmd55
    SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;	//r1
    SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;			//关闭等待中断
    SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;		//写命令进命令寄存器
    SDIO_SendCommand(&SDIO_CmdInitStruct);		
	errorstatus = CmdResp1Error(SD_CMD_APP_CMD); 		 	//等待R1响应   
	if(errorstatus == SD_OK){								//SD2.0/SD 1.1,否则为MMC卡												  
		//SD卡,发送ACMD41 SD_APP_OP_COND,参数为:0x80100000 
		while((!validvoltage) && (count<SD_MAX_VOLT_TRIAL)){	   										   
			SDIO_CmdInitStruct.SDIO_Argument = 0x00;				//发送CMD55,短响应
			SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_APP_CMD;	  	//CMD55
			SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;	//r1
			SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;			//关闭等待中断
			SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;		//写命令进命令寄存器
			SDIO_SendCommand(&SDIO_CmdInitStruct);			
			errorstatus = CmdResp1Error(SD_CMD_APP_CMD); 	 		//等待R1响应   
 			if(errorstatus!=SD_OK){
				return errorstatus;   								//响应错误
			}
			
			SDIO_CmdInitStruct.SDIO_Argument = SD_VOLTAGE_WINDOW_SD | SDType;	//发送ACMD41,短响应	
			SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_SD_APP_OP_COND;			//ACMD41
			SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;  			//r3
			SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;						//关闭等待中断
			SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;					//写命令进命令寄存器
			SDIO_SendCommand(&SDIO_CmdInitStruct);
			errorstatus = CmdResp3Error(); 					//等待R3响应   
 			if(errorstatus!=SD_OK){
				return errorstatus;   						//响应错误 
			}
			
			response = SDIO->RESP1;			   				//得到响应
			validvoltage=(((response>>31)==1)?1:0);			//判断SD卡上电是否完成
			count++;
		}
		if(count >= SD_MAX_VOLT_TRIAL){
			errorstatus = SD_INVALID_VOLTRANGE;
			return errorstatus;
		}	 
		if(response &= SD_HIGH_CAPACITY){  					//判断卡容量
		
			CardType = SDIO_HIGH_CAPACITY_SD_CARD;
		}
 	}
	else	//MMC卡
	{
		//MMC卡,发送CMD1 SDIO_SEND_OP_COND,参数为:0x80FF8000 
		while((!validvoltage)&&(count<SD_MAX_VOLT_TRIAL)){	   										   				   
			SDIO_CmdInitStruct.SDIO_Argument = SD_VOLTAGE_WINDOW_MMC;	//发送CMD1,短响应	   
			SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_SEND_OP_COND;		//CMD1
			SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;  	//r3
			SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;				//关闭等待中断
			SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;			//写命令进命令寄存器
			SDIO_SendCommand(&SDIO_CmdInitStruct);
			errorstatus = CmdResp3Error(); 					//等待R3响应   
 			if(errorstatus != SD_OK){
				return errorstatus;   						//响应错误  
			}
			
			response = SDIO->RESP1;			   				//得到响应
			validvoltage = (((response>>31)==1)?1:0);
			count++;
		}
		if(count>=SD_MAX_VOLT_TRIAL){
			errorstatus = SD_INVALID_VOLTRANGE;
			return errorstatus;
		}	 			    
		CardType = SDIO_MULTIMEDIA_CARD;	  
  	}  
  	return (errorstatus);		
}
//SD卡 Power OFF
//返回值:错误代码;(0,无错误)
SD_Error SD_PowerOFF(void)
{
  	SDIO_SetPowerState(SDIO_PowerState_OFF);	//SDIO电源关闭,时钟停止	
	return SD_OK;		  
}   
//初始化所有的卡,并让卡进入就绪状态
//返回值:错误代码
SD_Error SD_InitializeCards(void)
{
 	SD_Error errorstatus = SD_OK;
	unsigned short rca = 0x01;
	
 	if((SDIO->POWER & 0X03) == 0){
		return SD_REQUEST_NOT_APPLICABLE;						//检查电源状态,确保为上电状态
 	}
	if(SDIO_SECURE_DIGITAL_IO_CARD != CardType){				//非SECURE_DIGITAL_IO_CARD
		SDIO_CmdInitStruct.SDIO_Argument = 0x0;					//发送CMD2,取得CID,长响应
		SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_ALL_SEND_CID;	//CMD2
		SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Long;	//r2
		SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;			//关闭等待中断
		SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;		//写命令进命令寄存器
		SDIO_SendCommand(&SDIO_CmdInitStruct);
		errorstatus=CmdResp2Error(); 							//等待R2响应   
		if(errorstatus != SD_OK){
			return errorstatus;   								//响应错误		
		}
  		
 		CID_Tab[0] = SDIO->RESP1;
		CID_Tab[1] = SDIO->RESP2;
		CID_Tab[2] = SDIO->RESP3;
		CID_Tab[3] = SDIO->RESP4;
	}
	if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType) || 
	   (SDIO_SECURE_DIGITAL_IO_COMBO_CARD==CardType) || (SDIO_HIGH_CAPACITY_SD_CARD==CardType)){	//判断卡类型
		SDIO_CmdInitStruct.SDIO_Argument = 0x00;				//发送CMD3,短响应 
		SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_SET_REL_ADDR;	//cmd3
		SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short; //r6
		SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;			//关闭等待中断
		SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;		//写命令进命令寄存器
		SDIO_SendCommand(&SDIO_CmdInitStruct);	
		errorstatus = CmdResp6Error(SD_CMD_SET_REL_ADDR,&rca);	//等待R6响应 
		if(errorstatus != SD_OK){
			return errorstatus;   								//响应错误	
		}			
	}   
	if (SDIO_MULTIMEDIA_CARD == CardType){
		SDIO_CmdInitStruct.SDIO_Argument = (unsigned int)(rca<<16);		//发送CMD3,短响应 
		SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_SET_REL_ADDR;	//cmd3
		SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short; //r6
		SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;			//关闭等待中断
		SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;		//写命令进命令寄存器
		SDIO_SendCommand(&SDIO_CmdInitStruct);	
		errorstatus = CmdResp2Error(); 							//等待R2响应   
		if(errorstatus != SD_OK){
			return errorstatus;   								//响应错误	 
		}
	}
	if (SDIO_SECURE_DIGITAL_IO_CARD!=CardType){					//非SECURE_DIGITAL_IO_CARD
		RCA = rca;
		SDIO_CmdInitStruct.SDIO_Argument = (uint32_t)(rca << 16);	//发送CMD9+卡RCA,取得CSD,长响应 
		SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_SEND_CSD;			//CMD9
		SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Long;		//r2
		SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;				//关闭等待中断
		SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;			//写命令进命令寄存器
		SDIO_SendCommand(&SDIO_CmdInitStruct);	   
		errorstatus = CmdResp2Error(); 								//等待R2响应   
		if(errorstatus != SD_OK){
			return errorstatus;   									//响应错误
		}			
		
		CSD_Tab[0] = SDIO->RESP1;
		CSD_Tab[1] = SDIO->RESP2;
		CSD_Tab[2] = SDIO->RESP3;						
		CSD_Tab[3] = SDIO->RESP4;					    
	}
	return SD_OK;//卡初始化成功
} 
//得到卡信息
//cardinfo:卡信息存储区
//返回值:错误状态
SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo)
{
 	SD_Error errorstatus = SD_OK;
	unsigned char tmp = 0;	  
	
	cardinfo->CardType = (unsigned char)CardType; 				//卡类型
	cardinfo->RCA = (unsigned short)RCA;						//卡RCA值
	tmp = (unsigned char)((CSD_Tab[0]&0xFF000000)>>24);
	cardinfo->SD_csd.CSDStruct = (tmp&0xC0)>>6;					//CSD结构
	cardinfo->SD_csd.SysSpecVersion = (tmp&0x3C)>>2;			//2.0协议还没定义这部分(为保留),应该是后续协议定义的
	cardinfo->SD_csd.Reserved1 = tmp&0x03;						//2个保留位  
	tmp = (unsigned char)((CSD_Tab[0]&0x00FF0000)>>16);			//第1个字节
	cardinfo->SD_csd.TAAC = tmp;				   				//数据读时间1
	tmp = (unsigned char)((CSD_Tab[0]&0x0000FF00)>>8);	  		//第2个字节
	cardinfo->SD_csd.NSAC = tmp;		  						//数据读时间2
	tmp = (unsigned char)(CSD_Tab[0]&0x000000FF);				//第3个字节
	cardinfo->SD_csd.MaxBusClkFrec = tmp;		  				//传输速度	   
	tmp = (unsigned char)((CSD_Tab[1]&0xFF000000)>>24);			//第4个字节
	cardinfo->SD_csd.CardComdClasses = tmp<<4;    				//卡指令类高四位
	tmp = (unsigned char)((CSD_Tab[1]&0x00FF0000)>>16);	 		//第5个字节
	cardinfo->SD_csd.CardComdClasses |= (tmp&0xF0)>>4;			//卡指令类低四位
	cardinfo->SD_csd.RdBlockLen = tmp&0x0F;	    				//最大读取数据长度
	tmp = (unsigned char)((CSD_Tab[1]&0x0000FF00)>>8);			//第6个字节
	cardinfo->SD_csd.PartBlockRead = (tmp&0x80)>>7;				//允许分块读
	cardinfo->SD_csd.WrBlockMisalign = (tmp&0x40)>>6;			//写块错位
	cardinfo->SD_csd.RdBlockMisalign = (tmp&0x20)>>5;			//读块错位
	cardinfo->SD_csd.DSRImpl = (tmp&0x10)>>4;
	cardinfo->SD_csd.Reserved2 = 0; 							//保留
 	if((CardType==SDIO_STD_CAPACITY_SD_CARD_V1_1) || (CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0) || 
	   (SDIO_MULTIMEDIA_CARD==CardType)){						//标准1.1/2.0卡/MMC卡
		cardinfo->SD_csd.DeviceSize = (tmp&0x03)<<10;				//C_SIZE(12位)
	 	tmp = (unsigned char)(CSD_Tab[1]&0x000000FF); 			//第7个字节	
		cardinfo->SD_csd.DeviceSize |= (tmp)<<2;
 		tmp = (unsigned char)((CSD_Tab[2]&0xFF000000)>>24);		//第8个字节	
		cardinfo->SD_csd.DeviceSize |= (tmp&0xC0)>>6;
 		cardinfo->SD_csd.MaxRdCurrentVDDMin = (tmp&0x38)>>3;
		cardinfo->SD_csd.MaxRdCurrentVDDMax = (tmp&0x07);
 		tmp = (unsigned char)((CSD_Tab[2]&0x00FF0000)>>16);		//第9个字节	
		cardinfo->SD_csd.MaxWrCurrentVDDMin = (tmp&0xE0)>>5;
		cardinfo->SD_csd.MaxWrCurrentVDDMax = (tmp&0x1C)>>2;
		cardinfo->SD_csd.DeviceSizeMul = (tmp&0x03)<<1;			//C_SIZE_MULT
 		tmp = (unsigned char)((CSD_Tab[2]&0x0000FF00)>>8);	  	//第10个字节	
		cardinfo->SD_csd.DeviceSizeMul |= (tmp&0x80)>>7;
 		cardinfo->CardCapacity = (cardinfo->SD_csd.DeviceSize+1);		//计算卡容量
		cardinfo->CardCapacity *= (1<<(cardinfo->SD_csd.DeviceSizeMul+2));
		cardinfo->CardBlockSize = 1<<(cardinfo->SD_csd.RdBlockLen);		//块大小
		cardinfo->CardCapacity *= cardinfo->CardBlockSize;
	}else if(CardType == SDIO_HIGH_CAPACITY_SD_CARD){		//高容量卡
 		tmp = (unsigned char)(CSD_Tab[1]&0x000000FF); 		//第7个字节	
		cardinfo->SD_csd.DeviceSize = (tmp&0x3F)<<16;		//C_SIZE
 		tmp = (unsigned char)((CSD_Tab[2]&0xFF000000)>>24); //第8个字节	
 		cardinfo->SD_csd.DeviceSize |= (tmp<<8);
 		tmp = (unsigned char)((CSD_Tab[2]&0x00FF0000)>>16);	//第9个字节	
 		cardinfo->SD_csd.DeviceSize |= (tmp);
 		tmp = (unsigned char)((CSD_Tab[2]&0x0000FF00)>>8); 	//第10个字节	
 		cardinfo->CardCapacity = (long long)(cardinfo->SD_csd.DeviceSize+1)*512*1024;//计算卡容量
		cardinfo->CardBlockSize = 512; 						//块大小固定为512字节
	}	  
	cardinfo->SD_csd.EraseGrSize = (tmp&0x40)>>6;
	cardinfo->SD_csd.EraseGrMul = (tmp&0x3F)<<1;	   
	tmp = (unsigned char)(CSD_Tab[2]&0x000000FF);			//第11个字节	
	cardinfo->SD_csd.EraseGrMul |= (tmp&0x80)>>7;
	cardinfo->SD_csd.WrProtectGrSize = (tmp&0x7F);
 	tmp = (unsigned char)((CSD_Tab[3]&0xFF000000)>>24);		//第12个字节	
	cardinfo->SD_csd.WrProtectGrEnable = (tmp&0x80)>>7;
	cardinfo->SD_csd.ManDeflECC = (tmp&0x60)>>5;
	cardinfo->SD_csd.WrSpeedFact = (tmp&0x1C)>>2;
	cardinfo->SD_csd.MaxWrBlockLen = (tmp&0x03)<<2;	 
	tmp = (unsigned char)((CSD_Tab[3]&0x00FF0000)>>16);		//第13个字节
	cardinfo->SD_csd.MaxWrBlockLen |= (tmp&0xC0)>>6;
	cardinfo->SD_csd.WriteBlockPaPartial = (tmp&0x20)>>5;
	cardinfo->SD_csd.Reserved3 = 0;
	cardinfo->SD_csd.ContentProtectAppli = (tmp&0x01);  
	tmp = (unsigned char)((CSD_Tab[3]&0x0000FF00)>>8);		//第14个字节
	cardinfo->SD_csd.FileFormatGrouop = (tmp&0x80)>>7;
	cardinfo->SD_csd.CopyFlag = (tmp&0x40)>>6;
	cardinfo->SD_csd.PermWrProtect = (tmp&0x20)>>5;
	cardinfo->SD_csd.TempWrProtect = (tmp&0x10)>>4;
	cardinfo->SD_csd.FileFormat = (tmp&0x0C)>>2;
	cardinfo->SD_csd.ECC = (tmp&0x03);  
	tmp = (unsigned char)(CSD_Tab[3]&0x000000FF);			//第15个字节
	cardinfo->SD_csd.CSD_CRC = (tmp&0xFE)>>1;
	cardinfo->SD_csd.Reserved4=1;		 
	tmp = (unsigned char)((CID_Tab[0]&0xFF000000)>>24);		//第0个字节
	cardinfo->SD_cid.ManufacturerID = tmp;		    
	tmp = (unsigned char)((CID_Tab[0]&0x00FF0000)>>16);		//第1个字节
	cardinfo->SD_cid.OEM_AppliID = tmp<<8;	  
	tmp = (unsigned char)((CID_Tab[0]&0x000000FF00)>>8);		//第2个字节
	cardinfo->SD_cid.OEM_AppliID |= tmp;	    
	tmp = (unsigned char)(CID_Tab[0]&0x000000FF);			//第3个字节	
	cardinfo->SD_cid.ProdName1 = tmp<<24;				  
	tmp = (unsigned char)((CID_Tab[1]&0xFF000000)>>24); 	//第4个字节
	cardinfo->SD_cid.ProdName1 |= tmp<<16;	  
	tmp = (unsigned char)((CID_Tab[1]&0x00FF0000)>>16);	   	//第5个字节
	cardinfo->SD_cid.ProdName1 |= tmp<<8;		 
	tmp = (unsigned char)((CID_Tab[1]&0x0000FF00)>>8);		//第6个字节
	cardinfo->SD_cid.ProdName1 |= tmp;		   
	tmp = (unsigned char)(CID_Tab[1]&0x000000FF);	  		//第7个字节
	cardinfo->SD_cid.ProdName2 = tmp;			  
	tmp = (unsigned char)((CID_Tab[2]&0xFF000000)>>24); 	//第8个字节
	cardinfo->SD_cid.ProdRev = tmp;		 
	tmp = (unsigned char)((CID_Tab[2]&0x00FF0000)>>16);		//第9个字节
	cardinfo->SD_cid.ProdSN = tmp<<24;	   
	tmp = (unsigned char)((CID_Tab[2]&0x0000FF00)>>8); 		//第10个字节
	cardinfo->SD_cid.ProdSN |= tmp<<16;	   
	tmp=(unsigned char)(CID_Tab[2]&0x000000FF);   			//第11个字节
	cardinfo->SD_cid.ProdSN |= tmp<<8;		   
	tmp = (unsigned char)((CID_Tab[3]&0xFF000000)>>24); 	//第12个字节
	cardinfo->SD_cid.ProdSN |= tmp;			     
	tmp = (unsigned char)((CID_Tab[3]&0x00FF0000)>>16);	 	//第13个字节
	cardinfo->SD_cid.Reserved1 |= (tmp&0xF0)>>4;
	cardinfo->SD_cid.ManufactDate = (tmp&0x0F)<<8;    
	tmp = (unsigned char)((CID_Tab[3]&0x0000FF00)>>8);		//第14个字节
	cardinfo->SD_cid.ManufactDate |= tmp;		 	  
	tmp = (unsigned char)(CID_Tab[3]&0x000000FF);			//第15个字节
	cardinfo->SD_cid.CID_CRC = (tmp&0xFE)>>1;
	cardinfo->SD_cid.Reserved2 = 1;	 
	return errorstatus;
}
//设置SDIO总线宽度(MMC卡不支持4bit模式)
//wmode:位宽模式.0,1位数据宽度;1,4位数据宽度;2,8位数据宽度
//返回值:SD卡错误状态
SD_Error SD_EnableWideBusOperation(unsigned int wmode)
{
  	SD_Error errorstatus = SD_OK;
	
 	if(SDIO_MULTIMEDIA_CARD == CardType){
		return SD_UNSUPPORTED_FEATURE;		//MMC卡不支持
	}else if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType) ||
		    (SDIO_HIGH_CAPACITY_SD_CARD==CardType)){
		if(wmode >= 2){
			return SD_UNSUPPORTED_FEATURE;	//不支持8位模式
		}
 		else{
			errorstatus = SDEnWideBus(wmode);
 			if(SD_OK == errorstatus){
				SDIO->CLKCR &= ~(3<<11);					//清除之前的位宽设置    
				SDIO->CLKCR |= (unsigned short)wmode<<11;	//1位/4位总线宽度 
				SDIO->CLKCR |= 0<<14;						//不开启硬件流控制
			}
		}  
	}
	return errorstatus; 
}
//设置SD卡工作模式
//Mode:
//返回值:错误状态
SD_Error SD_SetDeviceMode(unsigned int Mode)
{
	SD_Error errorstatus = SD_OK;
	
 	if((Mode==SD_DMA_MODE) || (Mode==SD_POLLING_MODE)){
		DeviceMode=Mode;
	}else{
		errorstatus = SD_INVALID_PARAMETER;
	}
	
	return errorstatus;	    
}
//选卡
//发送CMD7,选择相对地址(rca)为addr的卡,取消其他卡.如果为0,则都不选择.
//addr:卡的RCA地址
SD_Error SD_SelectDeselect(unsigned int addr)
{
	SDIO_CmdInitStruct.SDIO_Argument = addr;					//发送CMD7,选择卡,短响应	
	SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_SEL_DESEL_CARD;	//CMD7
	SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;		//r1
	SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;				//关闭等待中断
	SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;			//写命令进命令寄存器
	SDIO_SendCommand(&SDIO_CmdInitStruct);
	
   	return CmdResp1Error(SD_CMD_SEL_DESEL_CARD);	  
}
//SD卡读取一个块 
//buf:读数据缓存区(必须4字节对齐!!)
//addr:读取地址
//blksize:块大小
SD_Error SD_ReadBlock(unsigned char *buf, long long addr, unsigned short blksize)
{	  
	SD_Error errorstatus = SD_OK;
	unsigned char power;
   	unsigned int count = 0, *tempbuff = (unsigned int*)buf;	//转换为unsigned int指针 
	unsigned int timeout = SDIO_DATATIMEOUT;  
	
   	if(NULL==buf){
		return SD_INVALID_PARAMETER; 
	}
   	SDIO->DCTRL = 0x0;							//数据控制寄存器清零(关DMA)   
	if(CardType == SDIO_HIGH_CAPACITY_SD_CARD){	//大容量卡
		blksize = 512;
		addr >>= 9;
	}   
	
	SDIO_DataInitStruct.SDIO_DataBlockSize = SDIO_DataBlockSize_1b ;	//数据块大小为1位
	SDIO_DataInitStruct.SDIO_DataLength = 0;							//要传输的数据字节数量为0
	SDIO_DataInitStruct.SDIO_DataTimeOut = SD_DATATIMEOUT;				//数据超时周期为最大值
	SDIO_DataInitStruct.SDIO_DPSM = SDIO_DPSM_Enable;					//数据传输开始
	SDIO_DataInitStruct.SDIO_TransferDir = SDIO_TransferDir_ToCard;		//从控制器到卡
	SDIO_DataInitStruct.SDIO_TransferMode = SDIO_TransferMode_Block;	//块数据传输
	SDIO_DataConfig(&SDIO_DataInitStruct);								//清除DPSM状态机配置
	
	if(SDIO->RESP1&SD_CARD_LOCKED){
		return SD_LOCK_UNLOCK_FAILED;		//卡锁了
	}
	
	if((blksize>0) && (blksize<=2048) && ((blksize&(blksize-1))==0)){
		power = convert_from_bytes_to_power_of_two(blksize);	    
	
		SDIO_CmdInitStruct.SDIO_Argument =  blksize;
		SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
		SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStruct);				//发送CMD16+设置数据长度为blksize,短响应
		
		errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);	//等待R1响应   
		if(errorstatus != SD_OK){
			return errorstatus;   							//响应错误	 
		}
	}else{
		return SD_INVALID_PARAMETER;	 
	}
	
	SDIO_DataInitStruct.SDIO_DataBlockSize = power<<4 ;				
	SDIO_DataInitStruct.SDIO_DataLength = blksize ;
	SDIO_DataInitStruct.SDIO_DataTimeOut = SD_DATATIMEOUT ;
	SDIO_DataInitStruct.SDIO_DPSM = SDIO_DPSM_Enable;
	SDIO_DataInitStruct.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
	SDIO_DataInitStruct.SDIO_TransferMode = SDIO_TransferMode_Block;
	SDIO_DataConfig(&SDIO_DataInitStruct);  			//清除DPSM状态机配置
	
   	SDIO_CmdInitStruct.SDIO_Argument = addr;
    SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_READ_SINGLE_BLOCK;
    SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStruct);				//发送CMD17+从addr地址出读取数据,短响应 
	
	errorstatus = CmdResp1Error(SD_CMD_READ_SINGLE_BLOCK);	//等待R1响应   
	if(errorstatus != SD_OK){
		return errorstatus;   								//响应错误	
	}		
 	if(DeviceMode == SD_POLLING_MODE){						//查询模式,轮询数据	 
 		INTX_DISABLE();										//关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)
		while(!(SDIO->STA&((1<<5)|(1<<1)|(1<<3)|(1<<10)|(1<<9)))){	//无上溢/CRC/超时/完成(标志)/起始位错误
			if(SDIO->STA & (1<<15)){					//接收区半满,表示至少存了8个字
				for(count=0; count<8; count++){			//循环读取数据
					*(tempbuff+count)=SDIO->FIFO;
				}
				tempbuff += 8;	 
				timeout = 0X7FFFFF; 	//读数据溢出时间
			}else{ 						//处理超时
				if(timeout == 0){
					return SD_DATA_TIMEOUT;
				}
				timeout--;
			}
		} 
		if(SDIO->STA & (1<<3)){			//数据超时错误									   
	 		SDIO->ICR |= 1<<3; 			//清错误标志
			return SD_DATA_TIMEOUT;
	 	}else if(SDIO->STA & (1<<1)){	//数据块CRC错误
	 		SDIO->ICR |= 1<<1; 			//清错误标志
			return SD_DATA_CRC_FAIL;		   
		}else if(SDIO->STA & (1<<5)){ 	//接收fifo上溢错误
	 		SDIO->ICR |= 1<<5; 			//清错误标志
			return SD_RX_OVERRUN;		 
		}else if(SDIO->STA & (1<<9)){ 	//接收起始位错误
	 		SDIO->ICR |= 1<<9; 			//清错误标志
			return SD_START_BIT_ERR;		 
		}   
		while(SDIO->STA & (1<<21)){		//FIFO里面,还存在可用数据
			*tempbuff = SDIO->FIFO;		//循环读取数据
			tempbuff++;
		}
		INTX_ENABLE();					//开启总中断
		SDIO->ICR = 0X5FF;	 			//清除所有标记
	}else if(DeviceMode == SD_DMA_MODE){
 		TransferError = SD_OK;
		StopCondition = 0;				//单块读,不需要发送停止传输指令
		TransferEnd = 0;				//传输结束标置位，在中断服务置1
		SDIO->MASK |= (1<<1)|(1<<3)|(1<<8)|(1<<5)|(1<<9);	//配置需要的中断 
	 	SDIO->DCTRL |= 1<<3;		 	//SDIO DMA使能 
 	    SD_DMA_Config((unsigned int*)buf, blksize, 0); 
 		while(((DMA2->LISR&(1<<27))==RESET) && (TransferEnd==0) && (TransferError==SD_OK) && timeout){
			timeout--;					//等待传输完成
		}			
		if(timeout == 0){
			return SD_DATA_TIMEOUT;		//超时
		}
		if(TransferError != SD_OK){
			errorstatus = TransferError;  
		}
    }   
 	return errorstatus; 
}
//SD卡读取多个块 
//buf:读数据缓存区
//addr:读取地址
//blksize:块大小
//nblks:要读取的块数
//返回值:错误状态
__align(4) unsigned int *tempbuff;
SD_Error SD_ReadMultiBlocks(unsigned char *buf, long long addr, unsigned short blksize, unsigned int nblks)
{
  	SD_Error errorstatus = SD_OK;
	unsigned char power;
   	unsigned int count = 0;
	unsigned int timeout = SDIO_DATATIMEOUT;  
	
	tempbuff = (unsigned int*)buf;				//转换为unsigned int指针
    SDIO->DCTRL = 0x0;							//数据控制寄存器清零(关DMA)   
	if(CardType == SDIO_HIGH_CAPACITY_SD_CARD){	//大容量卡
		blksize = 512;
		addr >>= 9;
	}  
	
	SDIO_DataInitStruct.SDIO_DataBlockSize = 0;
	SDIO_DataInitStruct.SDIO_DataLength = 0 ;
	SDIO_DataInitStruct.SDIO_DataTimeOut = SD_DATATIMEOUT ;
	SDIO_DataInitStruct.SDIO_DPSM = SDIO_DPSM_Enable;
	SDIO_DataInitStruct.SDIO_TransferDir = SDIO_TransferDir_ToCard;
	SDIO_DataInitStruct.SDIO_TransferMode = SDIO_TransferMode_Block;
	SDIO_DataConfig(&SDIO_DataInitStruct);								//清除DPSM状态机配置
	if(SDIO->RESP1&SD_CARD_LOCKED){
		return SD_LOCK_UNLOCK_FAILED;									//卡锁了
	}
	
	if((blksize>0) && (blksize<=2048) && ((blksize&(blksize-1))==0)){
		power = convert_from_bytes_to_power_of_two(blksize);	    
		SDIO_CmdInitStruct.SDIO_Argument = blksize;
		SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
		SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStruct);	   			//发送CMD16+设置数据长度为blksize,短响应 
		
		errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);	//等待R1响应   
		if(errorstatus != SD_OK){
			return errorstatus;   							//响应错误	 
		}
	}else{
		return SD_INVALID_PARAMETER;	
	}		
	
	if(nblks > 1){											//多块读  							    
 	  	if(nblks*blksize > SD_MAX_DATA_LENGTH){
			return SD_INVALID_PARAMETER;					//判断是否超过最大接收长度
		}
		
		SDIO_DataInitStruct.SDIO_DataBlockSize = power<<4;
		SDIO_DataInitStruct.SDIO_DataLength = nblks*blksize;
		SDIO_DataInitStruct.SDIO_DataTimeOut = SD_DATATIMEOUT;
		SDIO_DataInitStruct.SDIO_DPSM = SDIO_DPSM_Enable;
		SDIO_DataInitStruct.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
		SDIO_DataInitStruct.SDIO_TransferMode = SDIO_TransferMode_Block;
		SDIO_DataConfig(&SDIO_DataInitStruct);				//nblks*blksize,512块大小,卡到控制器

		SDIO_CmdInitStruct.SDIO_Argument = addr;	
		SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_READ_MULT_BLOCK;
		SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStruct);					//发送CMD18+从addr地址出读取数据,短响应 
		errorstatus = CmdResp1Error(SD_CMD_READ_MULT_BLOCK);	//等待R1响应   
		if(errorstatus != SD_OK){
			return errorstatus;   								//响应错误	 
		}			
		
 		if(DeviceMode == SD_POLLING_MODE){
			INTX_DISABLE();												//关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)
			while(!(SDIO->STA&((1<<5)|(1<<1)|(1<<3)|(1<<8)|(1<<9)))){	//无上溢/CRC/超时/完成(标志)/起始位错误
				if(SDIO->STA & (1<<15)){								//接收区半满,表示至少存了8个字
					for(count=0; count<8; count++){						//循环读取数据
						*(tempbuff+count) = SDIO->FIFO;
					}
					tempbuff += 8;	 
					timeout = 0X7FFFFF; 	//读数据溢出时间
				}else{ 						//处理超时
					if(timeout==0){
						return SD_DATA_TIMEOUT;
					}
					timeout--;
				}
			}  
			if(SDIO->STA & (1<<3)){			//数据超时错误									   
		 		SDIO->ICR |= 1<<3; 			//清错误标志
				return SD_DATA_TIMEOUT;
		 	}else if(SDIO->STA & (1<<1)){	//数据块CRC错误
		 		SDIO->ICR |= 1<<1; 			//清错误标志
				return SD_DATA_CRC_FAIL;		   
			}else if(SDIO->STA & (1<<5)){ 	//接收fifo上溢错误
		 		SDIO->ICR |= 1<<5; 			//清错误标志
				return SD_RX_OVERRUN;		 
			}else if(SDIO->STA & (1<<9)){ 	//接收起始位错误
		 		SDIO->ICR |= 1<<9; 			//清错误标志
				return SD_START_BIT_ERR;		 
			}   
			while(SDIO->STA & (1<<21)){		//FIFO里面,还存在可用数据
				*tempbuff = SDIO->FIFO;		//循环读取数据
				tempbuff++;
			}
	 		if(SDIO->STA & (1<<8)){			//接收结束
				if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType) ||
				   (SDIO_HIGH_CAPACITY_SD_CARD==CardType)){
					SDIO_CmdInitStruct.SDIO_Argument = 0;
				    SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION;
					SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;
					SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;
					SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;
					SDIO_SendCommand(&SDIO_CmdInitStruct);					//发送CMD12+结束传输
					  
					errorstatus = CmdResp1Error(SD_CMD_STOP_TRANSMISSION);	//等待R1响应   
					if(errorstatus != SD_OK){
						return errorstatus;	 
					}
				}
 			}
			INTX_ENABLE();				//开启总中断
	 		SDIO->ICR = 0X5FF;	 		//清除所有标记 
 		}else if(DeviceMode == SD_DMA_MODE){
	   		TransferError = SD_OK;
			StopCondition = 1;			//多块读,需要发送停止传输指令 
			TransferEnd = 0;			//传输结束标置位，在中断服务置1
			SDIO->MASK |= (1<<1)|(1<<3)|(1<<8)|(1<<5)|(1<<9);	//配置需要的中断 
		 	SDIO->DCTRL|=1<<3;		 							//SDIO DMA使能 
	 	    SD_DMA_Config((unsigned int*)buf, nblks*blksize, 0); 
	 		while(((DMA2->LISR&(1<<27))==RESET) && timeout){
				timeout--;				//等待传输完成 
			}
			if(timeout == 0){
				return SD_DATA_TIMEOUT;	//超时
			}
			while((TransferEnd==0) && (TransferError==SD_OK)); 
			if(TransferError != SD_OK){
				errorstatus=TransferError;  
			}				
		}		 
  	}
	return errorstatus;
}			    																  
//SD卡写1个块 
//buf:数据缓存区
//addr:写地址
//blksize:块大小	  
//返回值:错误状态
SD_Error SD_WriteBlock(unsigned char *buf, long long addr,  unsigned short blksize)
{
	SD_Error errorstatus = SD_OK;
	unsigned char power = 0, cardstate = 0;
	unsigned int timeout = 0, bytestransferred = 0;
	unsigned int cardstatus = 0, count = 0, restwords = 0;
	unsigned int tlen = blksize;							//总长度(字节)
	unsigned int *tempbuff = (unsigned int*)buf;	
	
 	if(buf == NULL){
		return SD_INVALID_PARAMETER;			//参数错误   
	}
  	SDIO->DCTRL = 0x0;							//数据控制寄存器清零(关DMA)  
	
  	SDIO_DataInitStruct.SDIO_DataBlockSize = 0;
	SDIO_DataInitStruct.SDIO_DataLength = 0;
	SDIO_DataInitStruct.SDIO_DataTimeOut = SD_DATATIMEOUT;
	SDIO_DataInitStruct.SDIO_DPSM = SDIO_DPSM_Enable;
	SDIO_DataInitStruct.SDIO_TransferDir = SDIO_TransferDir_ToCard;
	SDIO_DataInitStruct.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStruct);							//清除DPSM状态机配置
	
	if(SDIO->RESP1 & SD_CARD_LOCKED){
		return SD_LOCK_UNLOCK_FAILED;				//卡锁了
	}
 	if(CardType == SDIO_HIGH_CAPACITY_SD_CARD){		//大容量卡
		blksize = 512;
		addr >>= 9;
	}    
	if((blksize>0) && (blksize<=2048) && ((blksize&(blksize-1))==0)){
		power=convert_from_bytes_to_power_of_two(blksize);	 
		
		SDIO_CmdInitStruct.SDIO_Argument = blksize;	
		SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
		SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStruct);				//发送CMD16+设置数据长度为blksize,短响应 
		
		errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);	//等待R1响应   
		if(errorstatus != SD_OK){
			return errorstatus;   							//响应错误	
		}			
	}else{
		return SD_INVALID_PARAMETER;
	}
	
	SDIO_CmdInitStruct.SDIO_Argument = (unsigned int)RCA<<16;
	SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
	SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;
	SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;
	SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStruct);					//发送CMD13,查询卡的状态,短响应 	
	
	errorstatus = CmdResp1Error(SD_CMD_SEND_STATUS);		//等待R1响应   		   
	if(errorstatus != SD_OK){
		return errorstatus;
	}
	
	cardstatus = SDIO->RESP1;													  
	timeout = SD_DATATIMEOUT;
	//判断卡是否准备好
   	while(((cardstatus&0x00000100)==0) && (timeout>0)){ 	//检查READY_FOR_DATA位是否置位
		timeout--;
		
	   	SDIO_CmdInitStruct.SDIO_Argument = (unsigned int)RCA<<16;
		SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
		SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStruct);				//发送CMD13,查询卡的状态,短响应 
		
		errorstatus = CmdResp1Error(SD_CMD_SEND_STATUS);		//等待R1响应   		   
		if(errorstatus != SD_OK){
			return errorstatus;	
		}			
		cardstatus = SDIO->RESP1;													  
	}
	if(timeout == 0){
		return SD_ERROR;
	}
	
   	SDIO_CmdInitStruct.SDIO_Argument = addr;
	SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_WRITE_SINGLE_BLOCK;
	SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;
	SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;
	SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStruct);					//发送CMD24，写单块指令，短响应 	
	
	errorstatus = CmdResp1Error(SD_CMD_WRITE_SINGLE_BLOCK);	//等待R1响应   		   
	if(errorstatus != SD_OK){
		return errorstatus; 
	}		
	
	StopCondition = 0;										//单块写，不需要发送停止传输指令
	
 	SDIO_DataInitStruct.SDIO_DataBlockSize = power<<4;
	SDIO_DataInitStruct.SDIO_DataLength = blksize;
	SDIO_DataInitStruct.SDIO_DataTimeOut = SD_DATATIMEOUT;
	SDIO_DataInitStruct.SDIO_DPSM = SDIO_DPSM_Enable;
	SDIO_DataInitStruct.SDIO_TransferDir = SDIO_TransferDir_ToCard;
	SDIO_DataInitStruct.SDIO_TransferMode=SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStruct); 					//blksize，控制器到卡	
	
	timeout = SDIO_DATATIMEOUT;
	if(DeviceMode == SD_POLLING_MODE){
		INTX_DISABLE();												//关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)
		while(!(SDIO->STA&((1<<10)|(1<<4)|(1<<1)|(1<<3)|(1<<9)))){	//数据块发送成功/下溢/CRC/超时/起始位错误
			if(SDIO->STA & (1<<14)){								//发送区半空，表示至少存了8个字
				if((tlen-bytestransferred) < SD_HALFFIFOBYTES){		//不够32字节了
					restwords = ((tlen-bytestransferred)%4==0)?((tlen-bytestransferred)/4):((tlen-bytestransferred)/4+1);
					for(count=0; count<restwords; count++,tempbuff++,bytestransferred+=4){
						SDIO->FIFO=*tempbuff;
					}
				}else{
					for(count=0; count<8; count++){
						SDIO->FIFO = *(tempbuff+count);
					}
					tempbuff += 8;
					bytestransferred += 32;
				}
				timeout = 0X3FFFFFFF;	//写数据溢出时间
			}else{
				if(timeout == 0){
					return SD_DATA_TIMEOUT;
				}
				timeout--;
			}
		} 
		if(SDIO->STA & (1<<3)){			//数据超时错误							   
	 		SDIO->ICR |= 1<<3; 			//清错误标志
			return SD_DATA_TIMEOUT;
	 	}else if(SDIO->STA & (1<<1)){	//数据块CRC错误
	 		SDIO->ICR |= 1<<1; 			//清错误标志
			return SD_DATA_CRC_FAIL;		   
		}else if(SDIO->STA & (1<<4)){ 	//接收fifo下溢错误
	 		SDIO->ICR |= 1<<4; 			//清错误标志
			return SD_TX_UNDERRUN;		 
		}else if(SDIO->STA & (1<<9)){ 	//接收起始位错误
	 		SDIO->ICR |= 1<<9; 			//清错误标志
			return SD_START_BIT_ERR;		 
		}   
		INTX_ENABLE();					//开启总中断
		SDIO->ICR = 0X5FF;	 			//清除所有标记	  
	}else if(DeviceMode==SD_DMA_MODE){
   		TransferError = SD_OK;
		StopCondition = 0;				//单块写,不需要发送停止传输指令 
		TransferEnd = 0;				//传输结束标置位，在中断服务置1
		SDIO->MASK |= (1<<1)|(1<<3)|(1<<8)|(1<<4)|(1<<9);	//配置产生数据接收完成中断
		SD_DMA_Config((unsigned int *)buf, blksize, 1);		//SDIO DMA配置
 	 	SDIO->DCTRL |= 1<<3;								//SDIO DMA使能.  
 		while(((DMA2->LISR&(1<<27))==RESET) && timeout){
			timeout--;					//等待传输完成 
		}
		if(timeout == 0){
  			SD_Init();	 				//重新初始化SD卡,可以解决写入死机的问题
			return SD_DATA_TIMEOUT;		//超时	 
 		}
		timeout = SDIO_DATATIMEOUT;
		while((TransferEnd==0) && (TransferError==SD_OK) && timeout){
			timeout--;
		}
 		if(timeout == 0){
			return SD_DATA_TIMEOUT;		//超时	 
		}
  		if(TransferError != SD_OK){
			return TransferError;
		}
 	}  
 	SDIO->ICR = 0X5FF;	 				//清除所有标记
 	errorstatus = IsCardProgramming(&cardstate);
 	while((errorstatus==SD_OK) && ((cardstate==SD_CARD_PROGRAMMING)||(cardstate==SD_CARD_RECEIVING))){
		errorstatus=IsCardProgramming(&cardstate);
	}
	
	return errorstatus;
}
//SD卡写多个块 
//buf:数据缓存区
//addr:写地址
//blksize:块大小
//nblks:要写入的块数
//返回值:错误状态												   
SD_Error SD_WriteMultiBlocks(unsigned char *buf,long long addr,unsigned short blksize,unsigned int nblks)
{
	SD_Error errorstatus = SD_OK;
	unsigned char  power = 0, cardstate = 0;
	unsigned int timeout = 0, bytestransferred = 0;
	unsigned int count = 0, restwords = 0;
	unsigned int tlen = nblks*blksize;				//总长度(字节)
	unsigned int *tempbuff = (unsigned int*)buf;  
	
  	if(buf == NULL){
		return SD_INVALID_PARAMETER; 				//参数错误  
	}
  	SDIO->DCTRL = 0x0;								//数据控制寄存器清零(关DMA) 
	
  	SDIO_DataInitStruct.SDIO_DataBlockSize = 0; 	
	SDIO_DataInitStruct.SDIO_DataLength = 0;
	SDIO_DataInitStruct.SDIO_DataTimeOut = SD_DATATIMEOUT;
	SDIO_DataInitStruct.SDIO_DPSM = SDIO_DPSM_Enable;
	SDIO_DataInitStruct.SDIO_TransferDir = SDIO_TransferDir_ToCard;
	SDIO_DataInitStruct.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStruct);			//清除DPSM状态机配置	
	
	if(SDIO->RESP1&SD_CARD_LOCKED){
		return SD_LOCK_UNLOCK_FAILED;				//卡锁了
	}
 	if(CardType == SDIO_HIGH_CAPACITY_SD_CARD){		//大容量卡
		blksize = 512;
		addr >>= 9;
	}    
	if((blksize>0) && (blksize<=2048) && ((blksize&(blksize-1))==0)){
		power=convert_from_bytes_to_power_of_two(blksize);	    
		SDIO_CmdInitStruct.SDIO_Argument = blksize;	
		SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
		SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStruct);	   			//发送CMD16+设置数据长度为blksize,短响应
		
		errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);	//等待R1响应   
		if(errorstatus != SD_OK){
			return errorstatus;   							//响应错误	
		}			
	}else{
		return SD_INVALID_PARAMETER;
	}		
	if(nblks > 1)
	{					  
		if(nblks*blksize > SD_MAX_DATA_LENGTH){
			return SD_INVALID_PARAMETER;   
		}
     	if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType) ||
		   (SDIO_HIGH_CAPACITY_SD_CARD==CardType)){
			//提高性能
	 	   	SDIO_CmdInitStruct.SDIO_Argument = (unsigned int)RCA<<16;		
			SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_APP_CMD;
			SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;
			SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;
			SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;
			SDIO_SendCommand(&SDIO_CmdInitStruct);			//发送ACMD55,短响应 
			
			errorstatus = CmdResp1Error(SD_CMD_APP_CMD);	//等待R1响应   		   
			if(errorstatus!=SD_OK){
				return errorstatus;		
			}
		    
	 	   	SDIO_CmdInitStruct.SDIO_Argument = nblks;		
			SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_SET_BLOCK_COUNT;
			SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;
			SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;
			SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;
			SDIO_SendCommand(&SDIO_CmdInitStruct);					//发送CMD23,设置块数量,短响应 	 
			
			errorstatus = CmdResp1Error(SD_CMD_SET_BLOCK_COUNT);	//等待R1响应   		   
			if(errorstatus != SD_OK){
				return errorstatus;	
			}			    
		} 
		SDIO_CmdInitStruct.SDIO_Argument = addr;	 
		SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_WRITE_MULT_BLOCK;
		SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStruct);					//发送CMD25,多块写指令,短响应 	 
		
		errorstatus = CmdResp1Error(SD_CMD_WRITE_MULT_BLOCK);	//等待R1响应   		   
		if(errorstatus != SD_OK){
			return errorstatus;
		}
		
 	 	SDIO_DataInitStruct.SDIO_DataBlockSize = power<<4;
		SDIO_DataInitStruct.SDIO_DataLength = nblks*blksize;
		SDIO_DataInitStruct.SDIO_DataTimeOut = SD_DATATIMEOUT;
		SDIO_DataInitStruct.SDIO_DPSM = SDIO_DPSM_Enable;
		SDIO_DataInitStruct.SDIO_TransferDir = SDIO_TransferDir_ToCard;
		SDIO_DataInitStruct.SDIO_TransferMode = SDIO_TransferMode_Block;
		SDIO_DataConfig(&SDIO_DataInitStruct);					//blksize, 控制器到卡	
		
		if(DeviceMode == SD_POLLING_MODE){
			timeout = SDIO_DATATIMEOUT;
			INTX_DISABLE();												//关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)
			while(!(SDIO->STA&((1<<4)|(1<<1)|(1<<8)|(1<<3)|(1<<9)))){	//下溢/CRC/数据结束/超时/起始位错误
				if(SDIO->STA&(1<<14)){									//发送区半空,表示至少存了8字(32字节)	  
					if((tlen-bytestransferred)<SD_HALFFIFOBYTES){		//不够32字节了
						restwords = ((tlen-bytestransferred)%4==0)?((tlen-bytestransferred)/4):((tlen-bytestransferred)/4+1);
						for(count=0; count<restwords; count++,tempbuff++,bytestransferred+=4){
							SDIO->FIFO = *tempbuff;
						}
					}else{ 												//发送区半空,可以发送至少8字(32字节)数据
						for(count=0; count<SD_HALFFIFO; count++){
							SDIO->FIFO = *(tempbuff+count);
						}
						tempbuff += SD_HALFFIFO;
						bytestransferred += SD_HALFFIFOBYTES;
					}
					timeout = 0X3FFFFFFF;								//写数据溢出时间
				}else{
					if(timeout == 0){
						return SD_DATA_TIMEOUT; 
					}
					timeout--;
				}
			} 
			if(SDIO->STA & (1<<3)){			//数据超时错误						   
		 		SDIO->ICR |= 1<<3; 			//清错误标志
				return SD_DATA_TIMEOUT;
		 	}else if(SDIO->STA & (1<<1)){	//数据块CRC错误
		 		SDIO->ICR |= 1<<1; 			//清错误标志
				return SD_DATA_CRC_FAIL;		   
			}else if(SDIO->STA & (1<<4)){ 	//接收fifo下溢错误
		 		SDIO->ICR |= 1<<4; 			//清错误标志
				return SD_TX_UNDERRUN;		 
			}else if(SDIO->STA & (1<<9)){ 	//接收起始位错误
		 		SDIO->ICR |= 1<<9; 			//清错误标志
				return SD_START_BIT_ERR;		 
			}   										   
			if(SDIO->STA & (1<<8)){			//发送结束											 
				if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType) ||
					(SDIO_HIGH_CAPACITY_SD_CARD==CardType)){
					SDIO_CmdInitStruct.SDIO_Argument =0;	  
					SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION;
					SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;
					SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;
					SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;
					SDIO_SendCommand(&SDIO_CmdInitStruct);					//发送CMD12+结束传输 
					
					errorstatus = CmdResp1Error(SD_CMD_STOP_TRANSMISSION);	//等待R1响应   
					if(errorstatus != SD_OK){
						return errorstatus;	 
					}
				}
			}
			INTX_ENABLE();				//开启总中断
	 		SDIO->ICR = 0X5FF;	 		//清除所有标记 
	    }else if(DeviceMode == SD_DMA_MODE){
	   		TransferError = SD_OK;
			StopCondition = 1;			//多块写,需要发送停止传输指令 
			TransferEnd = 0;			//传输结束标置位，在中断服务置1
			SDIO->MASK |= (1<<1)|(1<<3)|(1<<8)|(1<<4)|(1<<9);		//配置产生数据接收完成中断
			SD_DMA_Config((unsigned int*)buf, nblks*blksize, 1);	//SDIO DMA配置
	 	 	SDIO->DCTRL |= 1<<3;									//SDIO DMA使能. 
			timeout = SDIO_DATATIMEOUT;
	 		while(((DMA2->LISR&(1<<27))==RESET) && timeout){
				timeout--;						//等待传输完成 
			}
			if(timeout == 0){		 			//超时						  
  				SD_Init();	 					//重新初始化SD卡,可以解决写入死机的问题
	 			return SD_DATA_TIMEOUT;			//超时	 
	 		}
			timeout = SDIO_DATATIMEOUT;
			while((TransferEnd==0) && (TransferError==SD_OK) && timeout){
				timeout--;
			}
	 		if(timeout == 0){
				return SD_DATA_TIMEOUT;			//超时	 
			}
	 		if(TransferError != SD_OK){
				return TransferError;	 
			}
		}
  	}
 	SDIO->ICR = 0X5FF;	 						//清除所有标记
 	errorstatus = IsCardProgramming(&cardstate);
 	while((errorstatus==SD_OK) && ((cardstate==SD_CARD_PROGRAMMING)||(cardstate==SD_CARD_RECEIVING))){
		errorstatus = IsCardProgramming(&cardstate);
	}   
	
	return errorstatus;	   
}
//SDIO中断服务函数		  
void SDIO_IRQHandler(void) 
{											
 	SD_ProcessIRQSrc();	//处理所有SDIO相关中断
}	 																    
//SDIO中断处理函数
//处理SDIO传输过程中的各种中断事务
//返回值:错误代码
SD_Error SD_ProcessIRQSrc(void)
{
	if(SDIO->STA & (1<<8)){			//接收完成中断
		if (StopCondition == 1){
			SDIO_CmdInitStruct.SDIO_Argument = 0;
			SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION;
			SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;
			SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;
			SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;
			SDIO_SendCommand(&SDIO_CmdInitStruct);	//发送CMD12+结束传输 	  
			
			TransferError = CmdResp1Error(SD_CMD_STOP_TRANSMISSION);
		}else{
			TransferError = SD_OK;	
		}
 		SDIO_ClearFlag(SDIO_FLAG_DATAEND);			//清除完成中断标记
		SDIO->MASK &= ~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));	//关闭相关中断
 		TransferEnd = 1;
		return(TransferError);
	}
 	if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET){	//数据CRC错误
		SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  				//清错误标志
		SDIO->MASK &= ~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));	//关闭相关中断
	    TransferError = SD_DATA_CRC_FAIL;
	    return(SD_DATA_CRC_FAIL);
	}
 	if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET){	//数据超时错误
		SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);  				//清中断标志
		SDIO->MASK &= ~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));	//关闭相关中断
	    TransferError = SD_DATA_TIMEOUT;
	    return(SD_DATA_TIMEOUT);
	}
  	if(SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET){		//FIFO上溢错误
		SDIO_ClearFlag(SDIO_FLAG_RXOVERR);  				//清中断标志
		SDIO->MASK &= ~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));	//关闭相关中断
	    TransferError = SD_RX_OVERRUN;
	    return (SD_RX_OVERRUN);
	}
   	if(SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET){	//FIFO下溢错误
		SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);  				//清中断标志
		SDIO->MASK &= ~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));	//关闭相关中断
	    TransferError = SD_TX_UNDERRUN;
	    return(SD_TX_UNDERRUN);
	}
	if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET){	//起始位错误
		SDIO_ClearFlag(SDIO_FLAG_STBITERR);  				//清中断标志
		SDIO->MASK &= ~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));	//关闭相关中断
	    TransferError = SD_START_BIT_ERR;
	    return (SD_START_BIT_ERR);
	}
	return (SD_OK);
}
  
//检查CMD0的执行状态
//返回值:sd卡错误码
SD_Error CmdError(void)
{
	SD_Error errorstatus = SD_OK;
	unsigned int timeout = SDIO_CMD0TIMEOUT;	 
	
	while(timeout--){
		if(SDIO_GetFlagStatus(SDIO_FLAG_CMDSENT) != RESET)break;	//命令已发送(无需响应)	 
	}	    
	if(timeout == 0){
		return SD_CMD_RSP_TIMEOUT; 
	}		
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);								//清除所有标记
	
	return errorstatus;
}	 
//检查R7响应的错误状态
//返回值:sd卡错误码
SD_Error CmdResp7Error(void)
{
	SD_Error errorstatus = SD_OK;
	unsigned int status;
	unsigned int timeout = SDIO_CMD0TIMEOUT;
	
 	while(timeout--){
		status = SDIO->STA;
		if(status & ((1<<0)|(1<<2)|(1<<6))){
			break;							//CRC错误/命令响应超时/已经收到响应(CRC校验成功)	
		}
	}
 	if((timeout==0) || (status&(1<<2))){	//响应超时																    
		errorstatus = SD_CMD_RSP_TIMEOUT;	//当前卡不是2.0兼容卡,或者不支持设定的电压范围
		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT); //清除命令响应超时标志
		return errorstatus;
	}	 
	if(status & 1<<6){						//成功接收到响应						   
		errorstatus = SD_OK;
		SDIO_ClearFlag(SDIO_FLAG_CMDREND); 	//清除响应标志
 	}
	
	return errorstatus;
}	   
//检查R1响应的错误状态
//cmd:当前命令
//返回值:sd卡错误码
SD_Error CmdResp1Error(unsigned char cmd)
{	  
   	unsigned int status; 
	while(1){
		status = SDIO->STA;
		if(status & ((1<<0)|(1<<2)|(1<<6))){				//CRC错误/命令响应超时/已经收到响应(CRC校验成功)
			break;
		}
	} 
	if(SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET){	//响应超时																    
 		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT); 				//清除命令响应超时标志
		return SD_CMD_RSP_TIMEOUT;
	}	
 	if(SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET){	//CRC错误																		    
 		SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL); 				//清除标志
		return SD_CMD_CRC_FAIL;
	}		
	if(SDIO->RESPCMD != cmd){
		return SD_ILLEGAL_CMD;								//命令不匹配 
	}
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);						//清除所有标记
	return (SD_Error)(SDIO->RESP1&SD_OCR_ERRORBITS);		//返回卡响应
}
//检查R3响应的错误状态
//返回值:错误状态
SD_Error CmdResp3Error(void)
{
	unsigned int status;	
	
 	while(1){
		status = SDIO->STA;
		if(status & ((1<<0)|(1<<2)|(1<<6))){				//CRC错误/命令响应超时/已经收到响应(CRC校验成功)	
			break;
		}
	}
 	if(SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET){	//响应超时									 
		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);					//清除命令响应超时标志
		return SD_CMD_RSP_TIMEOUT;
	}	 
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);						//清除所有标记
	
 	return SD_OK;										  
}
//检查R2响应的错误状态
//返回值:错误状态
SD_Error CmdResp2Error(void)
{
	SD_Error errorstatus = SD_OK;
	unsigned int status;
	unsigned int timeout = SDIO_CMD0TIMEOUT;
	
 	while(timeout--){
		status = SDIO->STA;
		if(status & ((1<<0)|(1<<2)|(1<<6))){			//CRC错误/命令响应超时/已经收到响应(CRC校验成功)	
			break;
		}
	}
  	if((timeout==0) || (status&(1<<2))){				//响应超时																		    
		errorstatus = SD_CMD_RSP_TIMEOUT; 
		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT); 			//清除命令响应超时标志
		return errorstatus;
	}	 
	if(SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET){//CRC错误								
		errorstatus = SD_CMD_CRC_FAIL;
		SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);				//清除响应标志
 	}
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);					//清除所有标记
	
 	return errorstatus;									    		 
} 
//检查R6响应的错误状态
//cmd:之前发送的命令
//prca:卡返回的RCA地址
//返回值:错误状态
SD_Error CmdResp6Error(unsigned char cmd,unsigned short*prca)
{
	SD_Error errorstatus = SD_OK;
	unsigned int status;					    
	unsigned int rspr1;
	
 	while(1){
		status = SDIO->STA;
		if(status & ((1<<0)|(1<<2)|(1<<6))){				//CRC错误/命令响应超时/已经收到响应(CRC校验成功)	
			break;
		}
	}
	if(SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET){	//响应超时																	    
 		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);					//清除命令响应超时标志
		return SD_CMD_RSP_TIMEOUT;
	}	 	 
	if(SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET){	//CRC错误						   
		SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);					//清除响应标志
 		return SD_CMD_CRC_FAIL;
	}
	if(SDIO->RESPCMD != cmd){								//判断是否响应cmd命令
 		return SD_ILLEGAL_CMD; 		
	}	    
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);						//清除所有标记
	rspr1 = SDIO->RESP1;									//得到响应 	 
	if(SD_ALLZERO == (rspr1&(SD_R6_GENERAL_UNKNOWN_ERROR|SD_R6_ILLEGAL_CMD|SD_R6_COM_CRC_FAILED))){
		*prca = (unsigned short)(rspr1>>16);				//右移16位得到,rca
		return errorstatus;
	}
   	if(rspr1 & SD_R6_GENERAL_UNKNOWN_ERROR){
		return SD_GENERAL_UNKNOWN_ERROR;
	}
   	if(rspr1 & SD_R6_ILLEGAL_CMD){
		return SD_ILLEGAL_CMD;
	}
   	if(rspr1 & SD_R6_COM_CRC_FAILED){
		return SD_COM_CRC_FAILED;
	}
	return errorstatus;
}

//SDIO使能宽总线模式
//enx:0,不使能;1,使能;
//返回值:错误状态
SD_Error SDEnWideBus(unsigned char enx)
{
	SD_Error errorstatus = SD_OK;
 	unsigned int scr[2] = {0,0};
	unsigned char arg = 0X00;
	
	if(enx){
		arg = 0X02;
	}else{
		arg = 0X00;
	}
 	if(SDIO->RESP1 & SD_CARD_LOCKED){
		return SD_LOCK_UNLOCK_FAILED;					//SD卡处于LOCKED状态		   
	}		
 	errorstatus = FindSCR(RCA,scr);						//得到SCR寄存器数据
 	if(errorstatus != SD_OK){
		return errorstatus;
	}
	if((scr[1]&SD_WIDE_BUS_SUPPORT) != SD_ALLZERO){		//支持宽总线
		SDIO_CmdInitStruct.SDIO_Argument = (uint32_t) RCA << 16;
		SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_APP_CMD;
		SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStruct);			//发送CMD55+RCA,短响应	

		errorstatus = CmdResp1Error(SD_CMD_APP_CMD);
		if(errorstatus != SD_OK){
			return errorstatus;
		}			

		SDIO_CmdInitStruct.SDIO_Argument = arg;	
		SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_APP_SD_SET_BUSWIDTH;
		SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStruct);				//发送ACMD6,短响应,参数:10,4位;00,1位.	

		errorstatus = CmdResp1Error(SD_CMD_APP_SD_SET_BUSWIDTH);
		return errorstatus;
	}else{
		return SD_REQUEST_NOT_APPLICABLE;					//不支持宽总线设置  	
	}		
}												   
//检查卡是否正在执行写操作
//pstatus:当前状态.
//返回值:错误代码
SD_Error IsCardProgramming(unsigned char *pstatus)
{
	unsigned int respR1 = 0, status = 0;  

	SDIO_CmdInitStruct.SDIO_Argument = (uint32_t) RCA << 16; 	//卡相对地址参数
	SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_SEND_STATUS;		//发送CMD13 	
	SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;
	SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;
	SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStruct);	

	status=SDIO->STA;

	while(!(status&((1<<0)|(1<<6)|(1<<2)))){
		status = SDIO->STA;										//等待操作完成
	}
	if(SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET){		//CRC检测失败 
		SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);						//清除错误标记
		return SD_CMD_CRC_FAIL;
	}
	if(SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET){		//命令超时 
		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);						//清除错误标记
		return SD_CMD_RSP_TIMEOUT;
	}
	if(SDIO->RESPCMD != SD_CMD_SEND_STATUS){
		return SD_ILLEGAL_CMD;
	}
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);							//清除所有标记
	respR1 = SDIO->RESP1;
	*pstatus = (unsigned char)((respR1>>9)&0x0000000F);
	
	return SD_OK;
}
//读取当前卡状态
//pcardstatus:卡状态
//返回值:错误代码
SD_Error SD_SendStatus(uint32_t *pcardstatus)
{
	SD_Error errorstatus = SD_OK;
	
	if(pcardstatus == NULL){
		errorstatus = SD_INVALID_PARAMETER;
		return errorstatus;
	}

	SDIO_CmdInitStruct.SDIO_Argument = (uint32_t) RCA << 16;	//发送CMD13,短响应		 
	SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
	SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;
	SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;
	SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStruct);	

	errorstatus = CmdResp1Error(SD_CMD_SEND_STATUS);			//查询响应状态 
	if(errorstatus != SD_OK){
		return errorstatus;
	}
	*pcardstatus = SDIO->RESP1;									//读取响应值
	
	return errorstatus;
} 
//返回SD卡的状态
//返回值:SD卡状态
SDCardState SD_GetState(void)
{
	unsigned int resp1 = 0;
	
	if(SD_SendStatus(&resp1) != SD_OK){
		return SD_CARD_ERROR;
	}else{
		return (SDCardState)((resp1>>9)&0x0F);
	}
}
//查找SD卡的SCR寄存器值
//rca:卡相对地址
//pscr:数据缓存区(存储SCR内容)
//返回值:错误状态		   
SD_Error FindSCR(unsigned short rca,unsigned int *pscr)
{ 
	unsigned int index = 0; 
	SD_Error errorstatus = SD_OK;
	unsigned int tempscr[2] = {0,0};  

	SDIO_CmdInitStruct.SDIO_Argument = (unsigned int)8;	 
	SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;  //cmd16
	SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;  //r1
	SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;
	SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStruct);					 //发送CMD16,短响应,设置Block Size为8字节	

	errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

	if(errorstatus != SD_OK){
		return errorstatus;	 
	}

	SDIO_CmdInitStruct.SDIO_Argument = (unsigned int) RCA<<16; 
	SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_APP_CMD;		//发送CMD55,短响应 	
	SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;
	SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;
	SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStruct);

	errorstatus = CmdResp1Error(SD_CMD_APP_CMD);
	if(errorstatus != SD_OK){
		return errorstatus;
	}

	SDIO_DataInitStruct.SDIO_DataTimeOut = SD_DATATIMEOUT;
	SDIO_DataInitStruct.SDIO_DataLength = 8;  
	SDIO_DataInitStruct.SDIO_DataBlockSize = SDIO_DataBlockSize_8b;  	//块大小8byte 
	SDIO_DataInitStruct.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
	SDIO_DataInitStruct.SDIO_TransferMode = SDIO_TransferMode_Block;
	SDIO_DataInitStruct.SDIO_DPSM = SDIO_DPSM_Enable;
	SDIO_DataConfig(&SDIO_DataInitStruct);								//8个字节长度,block为8字节,SD卡到SDIO.

	SDIO_CmdInitStruct.SDIO_Argument = 0x0;
	SDIO_CmdInitStruct.SDIO_CmdIndex = SD_CMD_SD_APP_SEND_SCR;			//发送ACMD51,短响应,参数为0	
	SDIO_CmdInitStruct.SDIO_Response = SDIO_Response_Short;  			//r1
	SDIO_CmdInitStruct.SDIO_Wait = SDIO_Wait_No;	
	SDIO_CmdInitStruct.SDIO_CPSM = SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStruct);

	errorstatus = CmdResp1Error(SD_CMD_SD_APP_SEND_SCR);
	if(errorstatus!=SD_OK){
		return errorstatus;	
	}		
	while(!(SDIO->STA&(SDIO_FLAG_RXOVERR|SDIO_FLAG_DCRCFAIL|SDIO_FLAG_DTIMEOUT|SDIO_FLAG_DBCKEND|SDIO_FLAG_STBITERR))){ 
		if(SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET){		//接收FIFO数据可用
			*(tempscr+index) = SDIO->FIFO;						//读取FIFO内容
			index++;
			if(index >= 2){
				break;
			}
		}
	}
	if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET){		//数据超时错误								   
		SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 					//清错误标志
		return SD_DATA_TIMEOUT;
	}else if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET){	//数据块CRC错误
		SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  					//清错误标志
		return SD_DATA_CRC_FAIL;		   
	}else if(SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET){ 	//接收fifo上溢错误
		SDIO_ClearFlag(SDIO_FLAG_RXOVERR);						//清错误标志
		return SD_RX_OVERRUN;		 
	}else if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET){ 	//接收起始位错误
		SDIO_ClearFlag(SDIO_FLAG_STBITERR);						//清错误标志
		return SD_START_BIT_ERR;		 
	}  
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);							//清除所有标记
	//把数据顺序按8位为单位倒过来.  	
	*(pscr+1) = ((tempscr[0]&SD_0TO7BITS)<<24)|((tempscr[0]&SD_8TO15BITS)<<8)|((tempscr[0]&SD_16TO23BITS)>>8)|((tempscr[0]&SD_24TO31BITS)>>24);
	*(pscr) = ((tempscr[1]&SD_0TO7BITS)<<24)|((tempscr[1]&SD_8TO15BITS)<<8)|((tempscr[1]&SD_16TO23BITS)>>8)|((tempscr[1]&SD_24TO31BITS)>>24);
 	
	return errorstatus;
}
//得到NumberOfBytes以2为底的指数.
//NumberOfBytes:字节数.
//返回值:以2为底的指数值
unsigned char convert_from_bytes_to_power_of_two(unsigned short NumberOfBytes)
{
	unsigned char count = 0;
	
	while(NumberOfBytes != 1){
		NumberOfBytes >>= 1;
		count++;
	}
	return count;
} 	 
//配置SDIO DMA  
//mbuf:存储器地址
//bufsize:传输数据量
//dir:方向;1,存储器-->SDIO(写数据);0,SDIO-->存储器(读数据);
void SD_DMA_Config(unsigned int*mbuf, unsigned int bufsize, unsigned char dir)
{		 
	DMA_InitTypeDef  DMA_InitStructure;

	while (DMA_GetCmdStatus(DMA2_Stream3) != DISABLE);						//等待DMA可配置 
		
	DMA_DeInit(DMA2_Stream3);												//清空之前该stream3上的所有中断标志

	DMA_InitStructure.DMA_Channel = DMA_Channel_4;  						//通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = (unsigned int)&SDIO->FIFO;	//DMA外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (unsigned int)mbuf;				//DMA 存储器0地址
	DMA_InitStructure.DMA_DIR = dir;										//存储器到外设模式
	DMA_InitStructure.DMA_BufferSize = 0;									//数据传输量 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;	//外设数据长度:32位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;			//存储器数据长度:32位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							// 使用普通模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;					//最高优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;   				//FIFO使能      
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;			//全FIFO
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC4;				//外设突发4次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_INC4;		//存储器突发4次传输
	DMA_Init(DMA2_Stream3, &DMA_InitStructure);								//初始化DMA Stream

	DMA_FlowControllerConfig(DMA2_Stream3,DMA_FlowCtrl_Peripheral);			//外设流控制 
	 
	DMA_Cmd(DMA2_Stream3 ,ENABLE);											//开启DMA传输	 
}   
//读SD卡
//buf:读数据缓存区
//sector:扇区地址
//cnt:扇区个数	
//返回值:错误状态;0,正常;其他,错误代码;				  				 
unsigned char SD_ReadDisk(unsigned char*buf, unsigned int sector, unsigned char cnt)
{
	unsigned char sta = SD_OK;
	long long lsector = sector;
	unsigned char n;
	
	if(CardType != SDIO_STD_CAPACITY_SD_CARD_V1_1){
		lsector <<= 9;
	}
	if((unsigned int)buf%4 != 0){
	 	for(n=0; n<cnt; n++){
		 	sta = SD_ReadBlock(SDIO_DATA_BUFFER, lsector+512*n, 512);	//单个sector的读操作
			memcpy(buf, SDIO_DATA_BUFFER, 512);
			buf += 512;
		} 
	}else{
		if(cnt == 1){
			sta = SD_ReadBlock(buf, lsector, 512);    					//单个sector的读操作
		}else{
			sta = SD_ReadMultiBlocks(buf, lsector, 512, cnt);			//多个sector  
		}
	}
	
	return sta;
}
//写SD卡
//buf:写数据缓存区
//sector:扇区地址
//cnt:扇区个数	
//返回值:错误状态;0,正常;其他,错误代码;	
unsigned char SD_WriteDisk(unsigned char* buf, unsigned int sector, unsigned char cnt)
{
	unsigned char sta = SD_OK;
	unsigned char n;
	long long lsector = sector;
	
	if(CardType != SDIO_STD_CAPACITY_SD_CARD_V1_1){
		lsector <<= 9;
	}
	if((unsigned int)buf%4 != 0){		//判断是否是int 类型的数据
	 	for(n=0; n<cnt; n++){
			memcpy(SDIO_DATA_BUFFER, buf, 512);
		 	sta = SD_WriteBlock(SDIO_DATA_BUFFER, lsector+512*n, 512);	//单个sector的写操作
			buf+=512;
		} 
	}else{
		if(cnt == 1){
			sta = SD_WriteBlock(buf, lsector, 512);    					//单个sector的写操作
		}else{
			sta = SD_WriteMultiBlocks(buf, lsector, 512, cnt);			//多个sector  
		}
	}
	
	return sta;
}


//通过串口打印SD卡相关信息
void SD_ShowSdcard_Info(void)
{
	switch(SDCardInfo.CardType){
		case SDIO_STD_CAPACITY_SD_CARD_V1_1:printf("Card Type:SDSC V1.1\r\n");break;
		case SDIO_STD_CAPACITY_SD_CARD_V2_0:printf("Card Type:SDSC V2.0\r\n");break;
		case SDIO_HIGH_CAPACITY_SD_CARD:printf("Card Type:SDHC V2.0\r\n");break;
		case SDIO_MULTIMEDIA_CARD:printf("Card Type:MMC Card\r\n");break;
	}	
	printf("Card ManufacturerID:%d\r\n",SDCardInfo.SD_cid.ManufacturerID);			//制造商ID
 	printf("Card RCA:%d\r\n",SDCardInfo.RCA);										//卡相对地址
	printf("Card Capacity:%d MB\r\n",(unsigned int)(SDCardInfo.CardCapacity>>20));	//显示容量
 	printf("Card BlockSize:%d\r\n\r\n",SDCardInfo.CardBlockSize);					//显示块大小
}






