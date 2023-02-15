#ifndef __AUDIOPLAY_H
#define __AUDIOPLAY_H

#include "io_bit.h"
#include "ff.h"
#include "wavplay.h"
#include "exfuns.h"						  

//���ֲ��ſ�����
typedef __packed struct
{  
	unsigned char *i2sbuf1;	//2��I2S�����BUF
	unsigned char *i2sbuf2; 
	unsigned char *tbuf;		//��ʱ����,����24bit�����ʱ����Ҫ�õ�
	FIL *file;					//��Ƶ�ļ�ָ��
	unsigned char status;		//bit0:0,��ͣ����;1,��������
								//bit1:0,��������;1,�������� 
}__audiodev; 
extern __audiodev audiodev;	//���ֲ��ſ�����

void Audio_Start(void);
void Audio_Stop(void);
unsigned short Audio_GetTnum(unsigned char *path);
void Audio_IndexShow(unsigned char *fn, unsigned short index, unsigned short total);
void Audio_MsgShow(unsigned int totsec,unsigned int cursec);
void Audio_MusicPlay(void);
unsigned char Audio_PlaySong(unsigned char* fname);

unsigned char IdentifyResults(unsigned char *buf);
void BatchedAction(unsigned char action);

#endif












