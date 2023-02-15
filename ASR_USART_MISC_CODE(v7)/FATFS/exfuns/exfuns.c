#include "string.h"
#include "exfuns.h"
#include "fattester.h"	
#include "mymalloc.h"
#include "usart1.h"

#define FILE_MAX_TYPE_NUM		7	//最多FILE_MAX_TYPE_NUM个大类
#define FILE_MAX_SUBT_NUM		4	//最多FILE_MAX_SUBT_NUM个小类

//文件类型列表
unsigned char *const FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM]=
{
	{"BIN"},						//BIN文件
	{"LRC"},						//LRC文件
	{"NES"},						//NES文件
	{"TXT", "C", "H"},				//文本文件
	{"WAV", "MP3", "APE", "FLAC"},	//支持的音乐文件
	{"BMP", "JPG", "JPEG", "GIF"},	//图片文件
	{"AVI"},						//视频文件
};

/**********************************公共文件区,使用malloc的时候*******************************************/
FATFS *fs[FF_VOLUMES];	//逻辑磁盘工作区.	 
FIL *file;	  			//文件1
FIL *ftemp;	  			//文件2.
UINT br,bw;				//读写变量
FILINFO fileinfo;		//文件信息
DIR dir;  				//目录
unsigned char *fatbuf;	//SD卡数据缓存区
/**********************************************END*******************************************************/
/*********************************************************************************************************
* 函 数 名 : exfuns_init
* 功能说明 : 为exfuns申请内存
* 形    参 : 无
* 返 回 值 : 0：成功；1：失败
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char exfuns_init(void)
{
	unsigned char i = 0;
	
	for(i=0; i<FF_VOLUMES; i++)
	{
		fs[i] = (FATFS*)mymalloc(SRAMIN, sizeof(FATFS));	//为磁盘i工作区申请内存	
		if(!fs[i])break;
	}
	file = (FIL*)mymalloc(SRAMIN, sizeof(FIL));				//为file申请内存
	ftemp = (FIL*)mymalloc(SRAMIN, sizeof(FIL));			//为ftemp申请内存
	fatbuf = (unsigned char*)mymalloc(SRAMIN, 512);			//为fatbuf申请内存
	if(i == FF_VOLUMES&&file&&ftemp&&fatbuf)	return 0;  	//申请有一个失败,即失败.
	else 										return 1;	
}
/*********************************************************************************************************
* 函 数 名 : char_upper
* 功能说明 : 将小写字母转为大写字母,如果是数字,则保持不变.
* 形    参 : c：要转换的字母
* 返 回 值 : 转换后的字母
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char char_upper(unsigned char c)
{
	if(c < 'A')		return c;			//数字,保持不变.
	if(c >= 'a')	return c-0x20;		//变为大写.
	else 			return c;			//大写,保持不变
}	   
/*********************************************************************************************************
* 函 数 名 : f_typetell
* 功能说明 : 报告文件的类型
* 形    参 : fname：文件名
* 返 回 值 : 0XFF，表示无法识别的文件类型编号；其他，高四位表示所属大类，低四位表示所属小类
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char f_typetell(unsigned char *fname)
{
	unsigned char tbuf[5] = {0};
	unsigned char *attr = '\0';			//后缀名
	unsigned char i = 0, j = 0;
	
	while(i < 250)
	{
		i++;
		if(*fname == '\0')	break;		//偏移到了最后了.
		fname++;
	}
	if(i == 250)	return 0XFF;		//错误的字符串.
 	for(i=0; i<5; i++)					//得到后缀名
	{
		fname--;
		if(*fname == '.')
		{
			fname++;
			attr = fname;
			break;
		}
  	}
	strcpy((char *)tbuf, (const char*)attr);			//copy
 	for(i=0; i<4; i++)	tbuf[i] = char_upper(tbuf[i]);	//全部变为大写 
	for(i=0; i<FILE_MAX_TYPE_NUM; i++)					//大类对比
	{
		for(j=0; j<FILE_MAX_SUBT_NUM; j++)				//子类对比
		{
			if(*FILE_TYPE_TBL[i][j] == 0)	break;		//此组已经没有可对比的成员了.
			if(strcmp((const char *)FILE_TYPE_TBL[i][j], (const char *)tbuf) == 0)	//找到了
			{
				return (i<<4) | j;
			}
		}
	}
	return 0XFF;	//没找到		 			   
}	 
/*********************************************************************************************************
* 函 数 名 : exf_getfree
* 功能说明 : 得到磁盘剩余容量
* 形    参 : drv：磁盘编号("0:"/"1:")；total:总容量（单位KB）；free:剩余容量（单位KB）
* 返 回 值 : 0，正常；其他，错误代码
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char exf_getfree(unsigned char *drv,unsigned int *total,unsigned int *free)
{
	FATFS *fs1;
	unsigned char res;
    unsigned int fre_clust = 0, fre_sect = 0, tot_sect = 0;
    //得到磁盘信息及空闲簇数量
    res = (unsigned int)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res == 0)
	{											   
	    tot_sect = (fs1->n_fatent-2)*fs1->csize;	//得到总扇区数
	    fre_sect = fre_clust*fs1->csize;			//得到空闲扇区数	   
#if FF_MAX_SS != 512				  				//扇区大小不是512字节,则转换为512字节
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total = tot_sect>>1;	//单位为KB
		*free = fre_sect>>1;	//单位为KB 
 	}
	return res;
}	











