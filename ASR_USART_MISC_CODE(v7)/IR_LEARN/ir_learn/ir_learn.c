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
		case IR_FORMAT:	//���͸�ʽ������
			Uart4_SendPackage(format, sizeof(format));
		break;
		case IR_RESET:	//���͸�λ����
			Uart4_SendPackage(reset, sizeof(reset));
		break;
		case IR_IN_ENTER:	//���ͽ����ڲ�����ѧϰģʽ����ģʽ��ѧϰ�ĺ�������洢��ģ���ڲ�flash�У�ֻ�ܴ�7����0~6
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
		case IR_IN_EXIT:	//�����˳��ڲ�����ѧϰģʽ
			Uart4_SendPackage(exitInteriorLearn, sizeof(exitInteriorLearn));
		break;
		case IR_IN_SEND:	//�����ڴ�洢��������
		{	
			if((index&0x7) <= 6)
			{
				sendIrCoding[5] = index;
				sendIrCoding[6] = Get_Check(&sendIrCoding[3], 3);
				Uart4_SendPackage(sendIrCoding, sizeof(sendIrCoding));
			}
			else printf("Warning: Out of memory index!\r\n");
		}break;
		case IR_EX_ENTRE:	//���ͽ����ⲿ����ѧϰģʽ����ģʽ��ѧϰ�ĺ�������ͨ�����ڷ�������Ҫ����ȥ�洢
			Uart4_SendPackage(enterExternLearn, sizeof(enterExternLearn));
		break;
		case IR_EX_EXIT:	//�����˳��ⲿ����ѧϰģʽ
			Uart4_SendPackage(exitExternLearn, sizeof(exitExternLearn));
		break;
		case IR_EX_SEND:	//�����ⲿ�洢��������
			
		break;

	}
}

//��У���
static unsigned char Get_Check(unsigned char *data, unsigned short len) 
{
	unsigned char sum = 0;
	unsigned short i;
	
	for (i=0; i<len; i++)
		sum += data[i];

	return sum;
}