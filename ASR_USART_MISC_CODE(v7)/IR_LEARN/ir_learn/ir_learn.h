#ifndef __IR_LEARN_H
#define __IR_LEARN_H

typedef enum
{
	IR_FORMAT,	//格式化
	IR_RESET,		//复位
	
	IR_IN_ENTER,//进入内部编码学习模式
	IR_IN_EXIT,	//退出内部编码学习模式
	IR_IN_SEND,	//发送内存存储编码命令
	
	IR_EX_ENTRE,//进入外部编码学习模式
	IR_EX_EXIT,	//退出外部编码学习模式
	IR_EX_SEND,	//发送外部存储编码命令
}IR_CMD;	

void Infrared_SendCmd(unsigned char cmd, unsigned char index);

#endif
