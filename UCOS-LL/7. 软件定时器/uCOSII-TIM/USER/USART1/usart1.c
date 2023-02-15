#include "usart1.h"


#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
    int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
    x = x; 
} 
void _ttywrch(int ch)
{
    ch=ch;
}
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 

//���ڳ�ʼ��
void USART1_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStruct;							//����һ��GPIO�ṹ�����
	USART_InitTypeDef USART_InitStruct;							//����һ��USART�ṹ�����
	//NVIC_InitTypeDef NVIC_InitStruct;							//����һ��NVIC�ṹ�����
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);		//��GPIOA��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);		//��USART1��ʱ��
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);		//��GPIOA_Pin9����ΪUSART_Rx
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);	//��GPIOA_Pin10����ΪUSART_Tx
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;			//ѡ������9��10
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;					//ѡ����ģʽ
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;					//ѡ������ģʽ
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;				//ѡ����������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;				//ѡ��50MHz
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);							//GPIO�ṹ���ʼ��
	
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;	//ѡ����ա�����ģʽ
	USART_InitStruct.USART_BaudRate = bound;						//���ò�����
	USART_InitStruct.USART_Parity = USART_Parity_No;			//��У��
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;	//����λΪ8λ
	USART_InitStruct.USART_StopBits = USART_StopBits_1;			//ֹͣλΪ1λ
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������
	
	USART_Init(USART1,&USART_InitStruct);						//USART�ṹ���ʼ��
	
	USART_Cmd(USART1,ENABLE);									//ʹ��USART1
	
}

 u8 res_flag;
 char usart_buffer[200];
//�жϷ�����
void USART1_IRQHandler (void)
{
	if(USART_GetITStatus(USART1,USART_IT_IDLE))
	{

	}
}


