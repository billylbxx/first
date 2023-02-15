#include "uart1.h"


//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;           
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
int _sys_exit(int x) 
{ 
	x = x; 
} 
int _ttywrch(int ch)
{
	ch = ch;
}


//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	    
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif






void uart1_init(u32 Baud)
{
	GPIO_InitTypeDef  GPIO_InitStruct;  //GPIO��ʼ���ṹ��
	USART_InitTypeDef USART_InitStruct; //���ڳ�ʼ���ṹ��
	NVIC_InitTypeDef  NVIC_InitStruct;  //�жϽṹ��
	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);    
	//1.����GPIO
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);  //�Ѵ���1TXD���õ�PA9  
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //�Ѵ���1RXD���õ�PA10 
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_9; //ѡ������
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; //����ģʽ
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; //����
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz; //�ٶ�2MHZ
	GPIO_Init(GPIOA,&GPIO_InitStruct); //��ʼ��
	
	//���ô���
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); //��UART1ʱ��

	USART_InitStruct.USART_BaudRate = Baud; //������
	USART_InitStruct.USART_WordLength = USART_WordLength_8b; //����λ8λ
	USART_InitStruct.USART_StopBits = USART_StopBits_1; //1λֹͣλ
	USART_InitStruct.USART_Parity = USART_Parity_No; //����ż����
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //ʹ�ܷ��������ģʽ
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//ʹ�ܽ����ж�
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);//ʹ�ܿ����ж�
	USART_Cmd(USART1,ENABLE);            //ʹ�ܴ���
	USART_Init(USART1,&USART_InitStruct); //��ʼ����������
	
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;  //ѡ�񴮿�1���ж�
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;    //ʹ�ܴ���1���ж�
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ����Ȳ���
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;         //��Ӧ���ȼ����Ȳ���
	NVIC_Init(&NVIC_InitStruct);
	
	
}



/*********************************************
��������void USART1_SendByte(u8 data)
�������ܣ�����һ���ֽ�
�����βΣ�u8 data
��������ֵ����
������ע��
********************************************/
void USART1_SendByte(u8 data)
{
//	while(!(USART1->SR &1<<7)){;}  //�ȴ��������  ����
//	USART1->DR = data;
	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)!=SET){;}
	USART1->DR = data;
}

/*********************************************
��������void USART1_SendStr(u8 *str)
�������ܣ�����һ���ַ���
�����βΣ�u8 *str  
��������ֵ����
������ע��USART1_SendStr("abcd");
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
��������u8 USART1_RecByte(void)
�������ܣ�����һ���ֽ�
�����βΣ��� 
��������ֵ��u8 
������ע��
********************************************/
u8 USART1_RecByte(void)
{
	while(!(USART1->SR &1<<5)){;}  //�ȴ�������� ����
		return USART1->DR;
}


/*********************************************
��������u8 USART1_RecStr(void)
�������ܣ�����һ���ַ���
�����βΣ��� 
��������ֵ��void 
������ע��
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
	*(buf-1) = '\0'; //���� ��\r��
}

/*********************************************
��������SendUsart1Package(u8 *data,u16 len)
�������ܣ�����1�����ȷ�������
�����βΣ�data��ָ��Ҫ�������� len��Ҫ���͵����ݳ��� 
��������ֵ��void 
������ע��SendUsart1Package(Usart2.RxBuff,Usart2.FlagLen&0x7fff);
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
//�����жϷ�����
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==1)//�����ж�
	{
		uart_buf[i++]= USART1->DR; // �������ݴ��  �����־
	}
	if(USART_GetITStatus(USART1,USART_IT_IDLE)==1)//�����ж�
	{
		uart1_flag=1;
		uart_buf[i] = '\0';	  //��ȫΪ�������ַ���	
		i=0;
		USART1->SR;  //���־
		USART1->DR;
	}	
}





