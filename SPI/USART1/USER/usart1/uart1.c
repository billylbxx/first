#include "uart1.h"


//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;           
//定义_sys_exit()以避免使用半主机模式    
int _sys_exit(int x) 
{ 
	x = x; 
} 
int _ttywrch(int ch)
{
	ch = ch;
}


//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	    
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif






void uart1_init(u32 Baud)
{
	GPIO_InitTypeDef  GPIO_InitStruct;  //GPIO初始化结构体
	USART_InitTypeDef USART_InitStruct; //串口初始化结构体
	NVIC_InitTypeDef  NVIC_InitStruct;  //中断结构体
	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);    
	//1.配置GPIO
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);  //把串口1TXD复用到PA9  
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //把串口1RXD复用到PA10 
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_9; //选择引脚
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; //复用模式
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; //推挽
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz; //速度2MHZ
	GPIO_Init(GPIOA,&GPIO_InitStruct); //初始化
	
	//配置串口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); //打开UART1时钟

	USART_InitStruct.USART_BaudRate = Baud; //波特率
	USART_InitStruct.USART_WordLength = USART_WordLength_8b; //数据位8位
	USART_InitStruct.USART_StopBits = USART_StopBits_1; //1位停止位
	USART_InitStruct.USART_Parity = USART_Parity_No; //无奇偶检验
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //使能发送与接收模式
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件流控
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//使能接收中断
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);//使能空闲中断
	USART_Cmd(USART1,ENABLE);            //使能串口
	USART_Init(USART1,&USART_InitStruct); //初始化串口配置
	
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;  //选择串口1总中断
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;    //使能串口1总中断
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;  //抢占优先级，先不管
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;         //响应优先级，先不管
	NVIC_Init(&NVIC_InitStruct);
	
	
}



/*********************************************
函数名：void USART1_SendByte(u8 data)
函数功能：发送一个字节
函数形参：u8 data
函数返回值：无
函数备注：
********************************************/
void USART1_SendByte(u8 data)
{
//	while(!(USART1->SR &1<<7)){;}  //等待发送完成  阻塞
//	USART1->DR = data;
	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)!=SET){;}
	USART1->DR = data;
}

/*********************************************
函数名：void USART1_SendStr(u8 *str)
函数功能：发送一串字符串
函数形参：u8 *str  
函数返回值：无
函数备注：USART1_SendStr("abcd");
********************************************/
void USART1_SendStr(u8 *str)
{
	while(*str!='\0') 
	{
		USART1_SendByte(*str);
		str++;
	}
}

/*********************************************
函数名：u8 USART1_RecByte(void)
函数功能：接收一个字节
函数形参：无 
函数返回值：u8 
函数备注：
********************************************/
u8 USART1_RecByte(void)
{
	while(!(USART1->SR &1<<5)){;}  //等待接收完成 阻塞
		return USART1->DR;
}


/*********************************************
函数名：u8 USART1_RecStr(void)
函数功能：接收一个字符串
函数形参：无 
函数返回值：void 
函数备注：
********************************************/
void USART1_RecStr(u8 *buf)
{
	u8 ch;
	while(1)
	{
		ch=USART1_RecByte();
		if(ch=='\n')
		{
			break;
		}
		*buf = ch;  
		buf++;
	}
	*(buf-1) = '\0'; //覆盖 ‘\r’
}

/*********************************************
函数名：SendUsart1Package(u8 *data,u16 len)
函数功能：串口1按长度发送数据
函数形参：data：指向要发的数据 len：要发送的数据长度 
函数返回值：void 
函数备注：SendUsart1Package(Usart2.RxBuff,Usart2.FlagLen&0x7fff);
********************************************/
void SendUsart1Package(u8 *data,u16 len)
{
	while(len--)
	{
		while(!USART_GetFlagStatus(USART1,USART_FLAG_TC));
		USART_SendData(USART1,*data++);
	}
}




u8 uart_buf[128],i=0,uart1_flag=0,data=0;
//串口中断服务函数
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==1)//接收中断
	{
		uart_buf[i++]= USART1->DR; // 接收数据存放  读请标志
	}
	if(USART_GetITStatus(USART1,USART_IT_IDLE)==1)//空闲中断
	{
		uart1_flag=1;
		uart_buf[i] = '\0';	  //补全为完整的字符串	
		i=0;
		USART1->SR;  //请标志
		USART1->DR;
	}	
}





