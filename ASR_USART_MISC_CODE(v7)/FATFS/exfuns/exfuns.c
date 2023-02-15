#include "string.h"
#include "exfuns.h"
#include "fattester.h"	
#include "mymalloc.h"
#include "usart1.h"

#define FILE_MAX_TYPE_NUM		7	//���FILE_MAX_TYPE_NUM������
#define FILE_MAX_SUBT_NUM		4	//���FILE_MAX_SUBT_NUM��С��

//�ļ������б�
unsigned char *const FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM]=
{
	{"BIN"},						//BIN�ļ�
	{"LRC"},						//LRC�ļ�
	{"NES"},						//NES�ļ�
	{"TXT", "C", "H"},				//�ı��ļ�
	{"WAV", "MP3", "APE", "FLAC"},	//֧�ֵ������ļ�
	{"BMP", "JPG", "JPEG", "GIF"},	//ͼƬ�ļ�
	{"AVI"},						//��Ƶ�ļ�
};

/**********************************�����ļ���,ʹ��malloc��ʱ��*******************************************/
FATFS *fs[FF_VOLUMES];	//�߼����̹�����.	 
FIL *file;	  			//�ļ�1
FIL *ftemp;	  			//�ļ�2.
UINT br,bw;				//��д����
FILINFO fileinfo;		//�ļ���Ϣ
DIR dir;  				//Ŀ¼
unsigned char *fatbuf;	//SD�����ݻ�����
/**********************************************END*******************************************************/
/*********************************************************************************************************
* �� �� �� : exfuns_init
* ����˵�� : Ϊexfuns�����ڴ�
* ��    �� : ��
* �� �� ֵ : 0���ɹ���1��ʧ��
* ��    ע : ��
*********************************************************************************************************/ 
unsigned char exfuns_init(void)
{
	unsigned char i = 0;
	
	for(i=0; i<FF_VOLUMES; i++)
	{
		fs[i] = (FATFS*)mymalloc(SRAMIN, sizeof(FATFS));	//Ϊ����i�����������ڴ�	
		if(!fs[i])break;
	}
	file = (FIL*)mymalloc(SRAMIN, sizeof(FIL));				//Ϊfile�����ڴ�
	ftemp = (FIL*)mymalloc(SRAMIN, sizeof(FIL));			//Ϊftemp�����ڴ�
	fatbuf = (unsigned char*)mymalloc(SRAMIN, 512);			//Ϊfatbuf�����ڴ�
	if(i == FF_VOLUMES&&file&&ftemp&&fatbuf)	return 0;  	//������һ��ʧ��,��ʧ��.
	else 										return 1;	
}
/*********************************************************************************************************
* �� �� �� : char_upper
* ����˵�� : ��Сд��ĸתΪ��д��ĸ,���������,�򱣳ֲ���.
* ��    �� : c��Ҫת������ĸ
* �� �� ֵ : ת�������ĸ
* ��    ע : ��
*********************************************************************************************************/ 
unsigned char char_upper(unsigned char c)
{
	if(c < 'A')		return c;			//����,���ֲ���.
	if(c >= 'a')	return c-0x20;		//��Ϊ��д.
	else 			return c;			//��д,���ֲ���
}	   
/*********************************************************************************************************
* �� �� �� : f_typetell
* ����˵�� : �����ļ�������
* ��    �� : fname���ļ���
* �� �� ֵ : 0XFF����ʾ�޷�ʶ����ļ����ͱ�ţ�����������λ��ʾ�������࣬����λ��ʾ����С��
* ��    ע : ��
*********************************************************************************************************/ 
unsigned char f_typetell(unsigned char *fname)
{
	unsigned char tbuf[5] = {0};
	unsigned char *attr = '\0';			//��׺��
	unsigned char i = 0, j = 0;
	
	while(i < 250)
	{
		i++;
		if(*fname == '\0')	break;		//ƫ�Ƶ��������.
		fname++;
	}
	if(i == 250)	return 0XFF;		//������ַ���.
 	for(i=0; i<5; i++)					//�õ���׺��
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
 	for(i=0; i<4; i++)	tbuf[i] = char_upper(tbuf[i]);	//ȫ����Ϊ��д 
	for(i=0; i<FILE_MAX_TYPE_NUM; i++)					//����Ա�
	{
		for(j=0; j<FILE_MAX_SUBT_NUM; j++)				//����Ա�
		{
			if(*FILE_TYPE_TBL[i][j] == 0)	break;		//�����Ѿ�û�пɶԱȵĳ�Ա��.
			if(strcmp((const char *)FILE_TYPE_TBL[i][j], (const char *)tbuf) == 0)	//�ҵ���
			{
				return (i<<4) | j;
			}
		}
	}
	return 0XFF;	//û�ҵ�		 			   
}	 
/*********************************************************************************************************
* �� �� �� : exf_getfree
* ����˵�� : �õ�����ʣ������
* ��    �� : drv�����̱��("0:"/"1:")��total:����������λKB����free:ʣ����������λKB��
* �� �� ֵ : 0���������������������
* ��    ע : ��
*********************************************************************************************************/ 
unsigned char exf_getfree(unsigned char *drv,unsigned int *total,unsigned int *free)
{
	FATFS *fs1;
	unsigned char res;
    unsigned int fre_clust = 0, fre_sect = 0, tot_sect = 0;
    //�õ�������Ϣ�����д�����
    res = (unsigned int)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res == 0)
	{											   
	    tot_sect = (fs1->n_fatent-2)*fs1->csize;	//�õ���������
	    fre_sect = fre_clust*fs1->csize;			//�õ�����������	   
#if FF_MAX_SS != 512				  				//������С����512�ֽ�,��ת��Ϊ512�ֽ�
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total = tot_sect>>1;	//��λΪKB
		*free = fre_sect>>1;	//��λΪKB 
 	}
	return res;
}	











