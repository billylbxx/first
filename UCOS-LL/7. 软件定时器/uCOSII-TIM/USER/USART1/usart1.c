#include "usart1.h"


#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
    int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
    x = x; 
} 
void _ttywrch(int ch)
{
    ch=ch;
}
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 

//串口初始化
void USART1_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStruct;							//定义一个GPIO结构体变量
	USART_InitTypeDef USART_InitStruct;							//定义一个USART结构体变量
	//NVIC_InitTypeDef NVIC_InitStruct;							//定义一个NVIC结构体变量
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);		//打开GPIOA的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);		//打开USART1的时钟
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);		//把GPIOA_Pin9复用为USART_Rx
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);	//把GPIOA_Pin10复用为USART_Tx
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;			//选择引脚9、10
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;					//选择复用模式
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;					//选择推挽模式
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;				//选择无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;				//选择50MHz
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);							//GPIO结构体初始化
	
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;	//选择接收、发送模式
	USART_InitStruct.USART_BaudRate = bound;						//设置波特率
	USART_InitStruct.USART_Parity = USART_Parity_No;			//无校验
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;	//数据位为8位
	USART_InitStruct.USART_StopBits = USART_StopBits_1;			//停止位为1位
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控
	
	USART_Init(USART1,&USART_InitStruct);						//USART结构体初始化
	
	USART_Cmd(USART1,ENABLE);									//使能USART1
	
}

 u8 res_flag;
 char usart_buffer[200];
//中断服务函数
void USART1_IRQHandler (void)
{
	if(USART_GetITStatus(USART1,USART_IT_IDLE))
	{

	}
}


