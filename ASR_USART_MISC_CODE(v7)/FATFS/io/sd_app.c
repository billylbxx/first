#include "sd_app.h"
#include "sd_driver.h"
#include "ff.h"			/* Declarations of FatFs API */
#include "diskio.h"		/* Declarations of device I/O functions */

//文件类型列表
const char *File_type_table[6][13]=
{
    {"BIN","HEX"},		 
    {"LRC"},			     
    {"NES"},			     
    {"TXT","C","H"},//文本	 
    {"MP1","MP2","MP3","MP4","M4A","3GP","3G2","OGG","ACC","WMA","WAV","MID","FLAC"},//音像
    {"BMP","JPG","JPEG","GIF"},//图片
};

FATFS myfs;
void SD_Init()
{
	 u32 total,free;
	 u8 filenum = 0;
	char dispbuf[100];
	if(FR_OK !=SD_Initialize()) 
	{
		printf("SD初始化失败");
		return ;
	}
	printf("SD初始化成功\r\n");
	
	if(FR_OK != f_mount(&myfs, "0:", 1)) //注册驱动器
	{
		printf("SD注册失败\r\n");
		return ;
	}
	mf_getfree("/",&total,&free);
	filenum = f_getflietnum("/",0x42);
	memset(dispbuf,0,sizeof(dispbuf));
	sprintf(dispbuf,"SD注册成功，存储总容量：%0.2fM,剩余容量：%0.2fM；MP3文件：%d份\r\n",total / 1024.,free / 1024.,filenum); 
	printf("%s\r\n",dispbuf); 
}


u8 char_upper(u8 c)
{
    if(c<'A')return c;
    if(c>='a')return c-0x20;
    else return c;
}


u8 mf_getfree(u8 *drv,u32 *total,u32 *free)
{
    FATFS *fs1;
    u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    res = f_getfree((const TCHAR*)drv, (DWORD *)&fre_clust, &fs1);
    if(res==0)
    {
        tot_sect=(fs1->n_fatent-2)*fs1->csize;	
        fre_sect=fre_clust*fs1->csize;			
#if FF_MAX_SS != FF_MIN_SS				  				
        tot_sect*=fs1->ssize/512;
        fre_sect*=fs1->ssize/512;
#endif
        *total=tot_sect>>1;	
        *free=fre_sect>>1;	
    }
    return res;
}


u8 f_typetell(u8 *fname)
{
	u8 tbuf[6];
	u8 i=0,j;

	while((*fname++ != '\0') && i<250)	i++;
	if(i==250)	return 0XFF;    

	i=0;
	while(*fname !='.')
	{
		fname--;
		if(++i>6)	return 0XFF;
	}

	strcpy((char *)tbuf,(const char*)(fname+1));
	for(i=0; i<strlen((const char*)tbuf); i++)
		tbuf[i]=char_upper(tbuf[i]); 

	for(i=0; i<6; i++)
	{
		for(j=0; j<13; j++)
		{
			if(*File_type_table[i][j]==0)	break;
			if(strcmp((const char *)File_type_table[i][j],(const char *)tbuf)==0)
			{
				return (i<<4)|j;
			}
		}
	}
	return 0XFF;
}


/*
path:路径
ftpye：类型变化
*/
u16 f_getflietnum(char *path,u8 ftpye)
{
    u8 res;
    u16 rval=0;
    DIR tdir;	 		
    FILINFO tfileinfo;	
    res=f_opendir(&tdir,(const TCHAR*)path); 					
    if(res==FR_OK)
    {
        while(1)
        {
            res=f_readdir(&tdir,&tfileinfo);       		
            if(res != FR_OK || tfileinfo.fname[0] == 0)break;	
            res=f_typetell((u8*)tfileinfo.fname);
            if(res == ftpye)
            {
                rval++;
            }
        }
    }
    return rval;
}




