#include "fattester.h"	 
#include "sd_app.h"
#include "usart1.h"
#include "exfuns.h"
#include "mymalloc.h"		  
#include "ff.h"
#include "string.h"
/*********************************************************************************************************
* 函 数 名 : mf_mount
* 功能说明 : 为磁盘注册工作区
* 形    参 : path：磁盘路径；比如"0:"、"1:"；mt：0，不立即注册（稍后注册）；1：立即注册
* 返 回 值 : 执行结果
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char mf_mount(unsigned char* path, unsigned char mt)
{		   
	return f_mount(fs[2], (const TCHAR*)path, mt); 
}
/*********************************************************************************************************
* 函 数 名 : mf_open
* 功能说明 : 打开路径下的文件
* 形    参 : path：路径+文件名；mode：打开模式
* 返 回 值 : 执行结果
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char mf_open(unsigned char*path, unsigned char mode)
{
	unsigned char res;	 
	res = f_open(file, (const TCHAR*)path, mode);	//打开文件夹
	return res;
} 
/*********************************************************************************************************
* 函 数 名 : mf_close
* 功能说明 : 关闭文件
* 形    参 : 无
* 返 回 值 : 执行结果
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char mf_close(void)
{
	f_close(file);
	return 0;
}
/*********************************************************************************************************
* 函 数 名 : mf_read
* 功能说明 : 读出数据
* 形    参 : len：读出的长度
* 返 回 值 : 执行结果
* 备    注 : 无
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
		if(res)		//读数据出错了
		{
			printf("\r\nRead Error:%d\r\n", res);   
		}else
		{
			tlen += br;
			for(t=0; t<br; t++)	printf("%c", fatbuf[t]); 
		}	 
	}
	if(tlen)	printf("\r\nReaded data len:%d\r\n", tlen);	//读到的数据长度
	printf("Read data over\r\n");	 
	return res;
}
/*********************************************************************************************************
* 函 数 名 : 无
* 功能说明 : 无
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
//写入数据
//dat:数据缓存区
//len:写入长度
//返回值:执行结果
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

//打开目录
 //path:路径
//返回值:执行结果
unsigned char mf_opendir(unsigned char* path)
{
	return f_opendir(&dir,(const TCHAR*)path);	
}
//关闭目录 
//返回值:执行结果
unsigned char mf_closedir(void)
{
	return f_closedir(&dir);	
}
//打读取文件夹
//返回值:执行结果
unsigned char mf_readdir(void)
{
	unsigned char res;
	char *fn;			 
#if _USE_LFN
 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(SRAMIN,fileinfo.lfsize);
#endif		  
	res=f_readdir(&dir,&fileinfo);//读取一个文件的信息
	if(res!=FR_OK||fileinfo.fname[0]==0)
	{
		myfree(SRAMIN,fileinfo.lfname);
		return res;//读完了.
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

 //遍历文件
 //path:路径
 //返回值:执行结果
unsigned char mf_scan_files(unsigned char * path)
{
	FRESULT res;	  
    char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(SRAMIN,fileinfo.lfsize);
#endif		  

    res = f_opendir(&dir,(const TCHAR*)path); //打开一个目录
    if (res == FR_OK) 
	{	
		printf("\r\n"); 
		while(1)
		{
	        res = f_readdir(&dir, &fileinfo);                   //读取目录下的一个文件
	        if (res != FR_OK || fileinfo.fname[0] == 0) break;  //错误了/到末尾了,退出
	        //if (fileinfo.fname[0] == '.') continue;             //忽略上级目录
#if _USE_LFN
        	fn = *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
#else							   
        	fn = fileinfo.fname;
#endif	                                              /* It is a file. */
			printf("%s/", path);//打印路径	
			printf("%s\r\n",  fn);//打印文件名	  
		} 
    }	  
	myfree(SRAMIN,fileinfo.lfname);
    return res;	  
}
//显示剩余容量
//drv:盘符
//返回值:剩余容量(字节)
unsigned int mf_showfree(unsigned char *drv)
{
	FATFS *fs1;
	unsigned char res;
    unsigned int fre_clust=0, fre_sect=0, tot_sect=0;
    //得到磁盘信息及空闲簇数量
    res = f_getfree((const TCHAR*)drv,(DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect = (fs1->n_fatent - 2) * fs1->csize;//得到总扇区数
	    fre_sect = fre_clust * fs1->csize;			//得到空闲扇区数	   
#if _MAX_SS!=512
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		if(tot_sect<20480)//总容量小于10M
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    printf("\r\n磁盘总容量:%d KB\r\n"
		           "可用空间:%d KB\r\n",
		           tot_sect>>1,fre_sect>>1);
		}else
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    printf("\r\n磁盘总容量:%d MB\r\n"
		           "可用空间:%d MB\r\n",
		           tot_sect>>11,fre_sect>>11);
		}
	}
	return fre_sect;
}		    
//文件读写指针偏移
//offset:相对首地址的偏移量
//返回值:执行结果.
unsigned char mf_lseek(unsigned int offset)
{
	return f_lseek(file,offset);
}
//读取文件当前读写指针的位置.
//返回值:位置
unsigned int mf_tell(void)
{
	return f_tell(file);
}
//读取文件大小
//返回值:文件大小
unsigned int mf_size(void)
{
	return f_size(file);
} 
//创建目录
//pname:目录路径+名字
//返回值:执行结果
unsigned char mf_mkdir(unsigned char*pname)
{
	return f_mkdir((const TCHAR *)pname);
}
//格式化
//path:磁盘路径，比如"0:"、"1:"
//mode:模式
//au:簇大小
//返回值:执行结果
unsigned char mf_fmkfs(unsigned char* path,unsigned char mode,unsigned short au)
{
	return f_mkfs((const TCHAR*)path,mode,au);//格式化,drv:盘符;mode:模式;au:簇大小
} 
//删除文件/目录
//pname:文件/目录路径+名字
//返回值:执行结果
unsigned char mf_unlink(unsigned char *pname)
{
	return  f_unlink((const TCHAR *)pname);
}

//修改文件/目录名字(如果目录不同,还可以移动文件哦!)
//oldname:之前的名字
//newname:新名字
//返回值:执行结果
unsigned char mf_rename(unsigned char *oldname,unsigned char* newname)
{
	return  f_rename((const TCHAR *)oldname,(const TCHAR *)newname);
}
//获取盘符（磁盘名字）
//path:磁盘路径，比如"0:"、"1:"  
void mf_getlabel(unsigned char *path)
{
	unsigned char buf[20];
	unsigned int sn=0;
	unsigned char res;
	res=f_getlabel ((const TCHAR *)path,(TCHAR *)buf,(DWORD*)&sn);
	if(res==FR_OK)
	{
		printf("\r\n磁盘%s 的盘符为:%s\r\n",path,buf);
		printf("磁盘%s 的序列号:%X\r\n\r\n",path,sn); 
	}else printf("\r\n获取失败，错误码:%X\r\n",res);
}
//设置盘符（磁盘名字），最长11个字符！！，支持数字和大写字母组合以及汉字等
//path:磁盘号+名字，比如"0:ALIENTEK"、"1:OPENEDV"  
void mf_setlabel(unsigned char *path)
{
	unsigned char res;
	res=f_setlabel ((const TCHAR *)path);
	if(res==FR_OK)
	{
		printf("\r\n磁盘盘符设置成功:%s\r\n",path);
	}else printf("\r\n磁盘盘符设置失败，错误码:%X\r\n",res);
} 

//从文件里面读取一段字符串
//size:要读取的长度
void mf_gets(unsigned short size)
{
 	TCHAR* rbuf;
	rbuf=f_gets((TCHAR*)fatbuf,size,file);
	if(*rbuf==0)return  ;//没有数据读到
	else
	{
		printf("\r\nThe String Readed Is:%s\r\n",rbuf);  	  
	}			    	
}
//需要_USE_STRFUNC>=1
//写一个字符到文件
//c:要写入的字符
//返回值:执行结果
unsigned char mf_putc(unsigned char c)
{
	return f_putc((TCHAR)c,file);
}
//写字符串到文件
//c:要写入的字符串
//返回值:写入的字符串长度
unsigned char mf_puts(unsigned char*c)
{
	return f_puts((TCHAR*)c,file);
}













