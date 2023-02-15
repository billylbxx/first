#include "ir_learn.h"
#include "uart4.h"

static unsigned char format[] = {0x68, 0x07, 0x00, 0xFF, 0x08, 0x07, 0x16};
static unsigned char reset[] = {0x68, 0x07, 0x00, 0xFF, 0x07, 0x06, 0x16};
unsigned char enterInteriorLearn[] = {0x68, 0x08, 0x00, 0xFF, 0x10, 0x00, 0x0F, 0x16};
static unsigned char exitInteriorLearn[] = {0x68, 0x07, 0x00, 0xFF, 0x11, 0x10, 0x16};
unsigned char sendIrCoding[] = {0x68, 0x08, 0x00, 0xFF, 0x12, 0x00, 0x11, 0x16};
static unsigned char enterExternLearn[] = {0x68, 0x07, 0x00, 0xFF, 0x20, 0x1F, 0x16};
static unsigned char exitExternLearn[] = {0x68, 0x07, 0x00, 0xFF, 0x21, 0x20, 0x16};

static unsigned char Get_Check(unsigned char *data, unsigned short len) ;

void Infrared_SendCmd(unsigned char cmd, unsigned char index)
{
	switch(cmd)
	{
		case IR_FORMAT:	//发送格式化命令
			Uart4_SendPackage(format, sizeof(format));
		break;
		case IR_RESET:	//发送复位命令
			Uart4_SendPackage(reset, sizeof(reset));
		break;
		case IR_IN_ENTER:	//发送进入内部编码学习模式，该模式下学习的红外编码会存储在模块内部flash中，只能存7个，0~6
		{	
			if((index&0x7) <= 6)
			{
				enterInteriorLearn[5] = index;
				enterInteriorLearn[6] = Get_Check(&enterInteriorLearn[3], 3);
				Uart4_SendPackage(enterInteriorLearn, sizeof(enterInteriorLearn));
			}
			else printf("Warning: Out of memory index!\r\n");
		}
		break;
		case IR_IN_EXIT:	//发送退出内部编码学习模式
			Uart4_SendPackage(exitInteriorLearn, sizeof(exitInteriorLearn));
		break;
		case IR_IN_SEND:	//发送内存存储编码命令
		{	
			if((index&0x7) <= 6)
			{
				sendIrCoding[5] = index;
				sendIrCoding[6] = Get_Check(&sendIrCoding[3], 3);
				Uart4_SendPackage(sendIrCoding, sizeof(sendIrCoding));
			}
			else printf("Warning: Out of memory index!\r\n");
		}break;
		case IR_EX_ENTRE:	//发送进入外部编码学习模式，该模式下学习的红外编码会通过串口反馈，需要主控去存储
			Uart4_SendPackage(enterExternLearn, sizeof(enterExternLearn));
		break;
		case IR_EX_EXIT:	//发送退出外部编码学习模式
			Uart4_SendPackage(exitExternLearn, sizeof(exitExternLearn));
		break;
		case IR_EX_SEND:	//发送外部存储编码命令
			
		break;

	}
}

//求校验和
static unsigned char Get_Check(unsigned char *data, unsigned short len) 
{
	unsigned char sum = 0;
	unsigned short i;
	
	for (i=0; i<len; i++)
		sum += data[i];

	return sum;
}