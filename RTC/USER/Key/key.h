#ifndef __KEY_H
#define __KEY_H
#include "io_bit.h" 
#define KEY1 PAin(0)
#define KEY2 PCin(13)
#define KEY3 PBin(1)

void key_Init(void);
char Scanf_Key(void);
typedef enum
{
	KEY_SHORT_PRESS = 0X00,	//�̰�
	KEY_LONG_PRESS  = 0X80,	//����
	
	KEY1_SHORT = 0x01,
	KEY2_SHORT = 0x02,
	KEY3_SHORT = 0x04,
	
	KEY1_LONG  = 0x81,
	KEY2_LONG  = 0x82,
	KEY3_LONG  = 0x84,
	
	KEY_FREE   = 10,			//����
	KEY_SHAKE  = 11,			//����
	KEY_PRESS  = 12,			//����
	KEY_LOOSEN = 13,			//�ɿ�

}KEY_STATE;
#endif
