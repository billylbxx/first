#include "fattester.h"	 
#include "sd_app.h"
#include "usart1.h"
#include "exfuns.h"
#include "mymalloc.h"		  
#include "ff.h"
#include "string.h"
/*********************************************************************************************************
* �� �� �� : mf_mount
* ����˵�� : Ϊ����ע�Ṥ����
* ��    �� : path������·��������"0:"��"1:"��mt��0��������ע�ᣨ�Ժ�ע�ᣩ��1������ע��
* �� �� ֵ : ִ�н��
* ��    ע : ��
*********************************************************************************************************/ 
unsigned char mf_mount(unsigned char* path, unsigned char mt)
{		   
	return f_mount(fs[2], (const TCHAR*)path, mt); 
}
/*********************************************************************************************************
* �� �� �� : mf_open
* ����˵�� : ��·���µ��ļ�
* ��    �� : path��·��+�ļ�����mode����ģʽ
* �� �� ֵ : ִ�н��
* ��    ע : ��
*********************************************************************************************************/ 
unsigned char mf_open(unsigned char*path, unsigned char mode)
{
	unsigned char res;	 
	res = f_open(file, (const TCHAR*)path, mode);	//���ļ���
	return res;
} 
/*********************************************************************************************************
* �� �� �� : mf_close
* ����˵�� : �ر��ļ�
* ��    �� : ��
* �� �� ֵ : ִ�н��
* ��    ע : ��
*********************************************************************************************************/ 
unsigned char mf_close(void)
{
	f_close(file);
	return 0;
}
/*********************************************************************************************************
* �� �� �� : mf_read
* ����˵�� : ��������
* ��    �� : len�������ĳ���
* �� �� ֵ : ִ�н��
* ��    ע : ��
*********************************************************************************************************/ 
unsigned char mf_read(unsigned short len)
{
	unsigned short i,t;
	unsigned char res = 0;
	unsigned short tlen = 0;
	printf("\r\nRead file data is:\r\n");
	for(i=0; i<len/512; i++)
	{
		res = f_read(file,fatbuf, 512, &br);
		if(res)
		{
			printf("Read Error:%d\r\n", res);
			break;
		}else
		{
			tlen += br;
			for(t=0; t<br; t++)	printf("%c", fatbuf[t]); 
		}
	}
	if(len % 512)
	{
		res = f_read(file, fatbuf, len%512, &br);
		if(res)		//�����ݳ�����
		{
			printf("\r\nRead Error:%d\r\n", res);   
		}else
		{
			tlen += br;
			for(t=0; t<br; t++)	printf("%c", fatbuf[t]); 
		}	 
	}
	if(tlen)	printf("\r\nReaded data len:%d\r\n", tlen);	//���������ݳ���
	printf("Read data over\r\n");	 
	return res;
}
/*********************************************************************************************************
* �� �� �� : ��
* ����˵�� : ��
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
//д������
//dat:���ݻ�����
//len:д�볤��
//����ֵ:ִ�н��
unsigned char mf_write(unsigned char*dat,unsigned short len)
{			    
	unsigned char res;	   					   

	printf("\r\nBegin Write file...\r\n");
	printf("Write data len:%d\r\n",len);	 
	res=f_write(file,dat,len,&bw);
	if(res)
	{
		printf("Write Error:%d\r\n",res);   
	}else printf("Writed data len:%d\r\n",bw);
	printf("Write data over.\r\n");
	return res;
}

//��Ŀ¼
 //path:·��
//����ֵ:ִ�н��
unsigned char mf_opendir(unsigned char* path)
{
	return f_opendir(&dir,(const TCHAR*)path);	
}
//�ر�Ŀ¼ 
//����ֵ:ִ�н��
unsigned char mf_closedir(void)
{
	return f_closedir(&dir);	
}
//���ȡ�ļ���
//����ֵ:ִ�н��
unsigned char mf_readdir(void)
{
	unsigned char res;
	char *fn;			 
#if _USE_LFN
 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(SRAMIN,fileinfo.lfsize);
#endif		  
	res=f_readdir(&dir,&fileinfo);//��ȡһ���ļ�����Ϣ
	if(res!=FR_OK||fileinfo.fname[0]==0)
	{
		myfree(SRAMIN,fileinfo.lfname);
		return res;//������.
	}
#if _USE_LFN
	fn=*fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
#else
	fn=fileinfo.fname;;
#endif	
	printf("\r\n DIR info:\r\n");

	printf("dir.id:%d\r\n",dir.id);
	printf("dir.index:%d\r\n",dir.index);
	printf("dir.sclust:%d\r\n",dir.sclust);
	printf("dir.clust:%d\r\n",dir.clust);
	printf("dir.sect:%d\r\n",dir.sect);	  

	printf("\r\n");
	printf("File Name is:%s\r\n",fn);
	printf("File Size is:%d\r\n",fileinfo.fsize);
	printf("File data is:%d\r\n",fileinfo.fdate);
	printf("File time is:%d\r\n",fileinfo.ftime);
	printf("File Attr is:%d\r\n",fileinfo.fattrib);
	printf("\r\n");
	myfree(SRAMIN,fileinfo.lfname);
	return 0;
}			 

 //�����ļ�
 //path:·��
 //����ֵ:ִ�н��
unsigned char mf_scan_files(unsigned char * path)
{
	FRESULT res;	  
    char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(SRAMIN,fileinfo.lfsize);
#endif		  

    res = f_opendir(&dir,(const TCHAR*)path); //��һ��Ŀ¼
    if (res == FR_OK) 
	{	
		printf("\r\n"); 
		while(1)
		{
	        res = f_readdir(&dir, &fileinfo);                   //��ȡĿ¼�µ�һ���ļ�
	        if (res != FR_OK || fileinfo.fname[0] == 0) break;  //������/��ĩβ��,�˳�
	        //if (fileinfo.fname[0] == '.') continue;             //�����ϼ�Ŀ¼
#if _USE_LFN
        	fn = *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
#else							   
        	fn = fileinfo.fname;
#endif	                                              /* It is a file. */
			printf("%s/", path);//��ӡ·��	
			printf("%s\r\n",  fn);//��ӡ�ļ���	  
		} 
    }	  
	myfree(SRAMIN,fileinfo.lfname);
    return res;	  
}
//��ʾʣ������
//drv:�̷�
//����ֵ:ʣ������(�ֽ�)
unsigned int mf_showfree(unsigned char *drv)
{
	FATFS *fs1;
	unsigned char res;
    unsigned int fre_clust=0, fre_sect=0, tot_sect=0;
    //�õ�������Ϣ�����д�����
    res = f_getfree((const TCHAR*)drv,(DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect = (fs1->n_fatent - 2) * fs1->csize;//�õ���������
	    fre_sect = fre_clust * fs1->csize;			//�õ�����������	   
#if _MAX_SS!=512
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		if(tot_sect<20480)//������С��10M
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    printf("\r\n����������:%d KB\r\n"
		           "���ÿռ�:%d KB\r\n",
		           tot_sect>>1,fre_sect>>1);
		}else
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    printf("\r\n����������:%d MB\r\n"
		           "���ÿռ�:%d MB\r\n",
		           tot_sect>>11,fre_sect>>11);
		}
	}
	return fre_sect;
}		    
//�ļ���дָ��ƫ��
//offset:����׵�ַ��ƫ����
//����ֵ:ִ�н��.
unsigned char mf_lseek(unsigned int offset)
{
	return f_lseek(file,offset);
}
//��ȡ�ļ���ǰ��дָ���λ��.
//����ֵ:λ��
unsigned int mf_tell(void)
{
	return f_tell(file);
}
//��ȡ�ļ���С
//����ֵ:�ļ���С
unsigned int mf_size(void)
{
	return f_size(file);
} 
//����Ŀ¼
//pname:Ŀ¼·��+����
//����ֵ:ִ�н��
unsigned char mf_mkdir(unsigned char*pname)
{
	return f_mkdir((const TCHAR *)pname);
}
//��ʽ��
//path:����·��������"0:"��"1:"
//mode:ģʽ
//au:�ش�С
//����ֵ:ִ�н��
unsigned char mf_fmkfs(unsigned char* path,unsigned char mode,unsigned short au)
{
	return f_mkfs((const TCHAR*)path,mode,au);//��ʽ��,drv:�̷�;mode:ģʽ;au:�ش�С
} 
//ɾ���ļ�/Ŀ¼
//pname:�ļ�/Ŀ¼·��+����
//����ֵ:ִ�н��
unsigned char mf_unlink(unsigned char *pname)
{
	return  f_unlink((const TCHAR *)pname);
}

//�޸��ļ�/Ŀ¼����(���Ŀ¼��ͬ,�������ƶ��ļ�Ŷ!)
//oldname:֮ǰ������
//newname:������
//����ֵ:ִ�н��
unsigned char mf_rename(unsigned char *oldname,unsigned char* newname)
{
	return  f_rename((const TCHAR *)oldname,(const TCHAR *)newname);
}
//��ȡ�̷����������֣�
//path:����·��������"0:"��"1:"  
void mf_getlabel(unsigned char *path)
{
	unsigned char buf[20];
	unsigned int sn=0;
	unsigned char res;
	res=f_getlabel ((const TCHAR *)path,(TCHAR *)buf,(DWORD*)&sn);
	if(res==FR_OK)
	{
		printf("\r\n����%s ���̷�Ϊ:%s\r\n",path,buf);
		printf("����%s �����к�:%X\r\n\r\n",path,sn); 
	}else printf("\r\n��ȡʧ�ܣ�������:%X\r\n",res);
}
//�����̷����������֣����11���ַ�������֧�����ֺʹ�д��ĸ����Լ����ֵ�
//path:���̺�+���֣�����"0:ALIENTEK"��"1:OPENEDV"  
void mf_setlabel(unsigned char *path)
{
	unsigned char res;
	res=f_setlabel ((const TCHAR *)path);
	if(res==FR_OK)
	{
		printf("\r\n�����̷����óɹ�:%s\r\n",path);
	}else printf("\r\n�����̷�����ʧ�ܣ�������:%X\r\n",res);
} 

//���ļ������ȡһ���ַ���
//size:Ҫ��ȡ�ĳ���
void mf_gets(unsigned short size)
{
 	TCHAR* rbuf;
	rbuf=f_gets((TCHAR*)fatbuf,size,file);
	if(*rbuf==0)return  ;//û�����ݶ���
	else
	{
		printf("\r\nThe String Readed Is:%s\r\n",rbuf);  	  
	}			    	
}
//��Ҫ_USE_STRFUNC>=1
//дһ���ַ����ļ�
//c:Ҫд����ַ�
//����ֵ:ִ�н��
unsigned char mf_putc(unsigned char c)
{
	return f_putc((TCHAR)c,file);
}
//д�ַ������ļ�
//c:Ҫд����ַ���
//����ֵ:д����ַ�������
unsigned char mf_puts(unsigned char*c)
{
	return f_puts((TCHAR*)c,file);
}













