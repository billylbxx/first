#ifndef __IR_LEARN_H
#define __IR_LEARN_H

typedef enum
{
	IR_FORMAT,	//��ʽ��
	IR_RESET,		//��λ
	
	IR_IN_ENTER,//�����ڲ�����ѧϰģʽ
	IR_IN_EXIT,	//�˳��ڲ�����ѧϰģʽ
	IR_IN_SEND,	//�����ڴ�洢��������
	
	IR_EX_ENTRE,//�����ⲿ����ѧϰģʽ
	IR_EX_EXIT,	//�˳��ⲿ����ѧϰģʽ
	IR_EX_SEND,	//�����ⲿ�洢��������
}IR_CMD;	

void Infrared_SendCmd(unsigned char cmd, unsigned char index);

#endif
