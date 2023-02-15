#ifndef __AUDIOPLAY_H
#define __AUDIOPLAY_H

#include "io_bit.h"
#include "ff.h"
#include "wavplay.h"
#include "exfuns.h"						  

//音乐播放控制器
typedef __packed struct
{  
	unsigned char *i2sbuf1;	//2个I2S解码的BUF
	unsigned char *i2sbuf2; 
	unsigned char *tbuf;		//零时数组,仅在24bit解码的时候需要用到
	FIL *file;					//音频文件指针
	unsigned char status;		//bit0:0,暂停播放;1,继续播放
								//bit1:0,结束播放;1,开启播放 
}__audiodev; 
extern __audiodev audiodev;	//音乐播放控制器

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












