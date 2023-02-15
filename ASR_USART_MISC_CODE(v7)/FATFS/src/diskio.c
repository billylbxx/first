/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "sd_driver.h"
#include "w25qxx.h"
#include "mymalloc.h"
#include "rtc.h"

#define SD_CARD	 0  //SD卡,卷标为0
#define EX_FLASH 1	//外部flash,卷标为1

#define FLASH_SECTOR_SIZE 	512			  
//对于W25Q128，前12M字节给fatfs用,12M字节后,用于存放字库,字库占用3.09M
//剩余部分,给客户自己用	 			    
unsigned short	FLASH_SECTOR_COUNT = 2048 * 12;	//W25Q1218,前12M字节给FATFS占用
#define FLASH_BLOCK_SIZE   	8     				//每个BLOCK有8个扇区
/*********************************************************************************************************
* 函 数 名 : disk_initialize
* 功能说明 : 初始化磁盘
* 形    参 : pdrv：物理驱动器编号
* 返 回 值 : 状态码
* 备    注 : 无
*********************************************************************************************************/ 
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	unsigned char res = 0;	
    
	switch(pdrv)
	{
		case SD_CARD:						//SD卡
			res=SD_Initialize();			//SD卡初始化 
  			break;
		case EX_FLASH:						//外部flash
			W25QXX_Init();
			FLASH_SECTOR_COUNT = 2048 * 12;	//W25Q1218,前12M字节给FATFS占用 
 			break;
		default:
			res=1; 
	}		 
	if(res)return  STA_NOINIT;
	else return 0; 							//初始化成功
}  
/*********************************************************************************************************
* 函 数 名 : disk_status
* 功能说明 : 获得磁盘状态
* 形    参 : pdrv：物理驱动器编号
* 返 回 值 : 状态码
* 备    注 : 无
*********************************************************************************************************/ 
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{ 
	return 0;
} 
/*********************************************************************************************************
* 函 数 名 : disk_read
* 功能说明 : 读磁盘数据
* 形    参 : pdrv：物理驱动器编号；buff：数据缓存区；sector：扇区地址；count：读取的扇区数
* 返 回 值 : 状态码
* 备    注 : 无
*********************************************************************************************************/ 
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	unsigned char res = 0; 
	
    if (!count)	return RES_PARERR;	//count不能等于0，否则返回参数错误		 	 
	switch(pdrv)
	{
		case SD_CARD:	//SD卡
			res = SD_ReadDisk(buff, sector, count);	 
			while(res)	//读出错
			{
				SD_Initialize();	//重新初始化SD卡
				res=SD_ReadDisk(buff, sector, count);	
				//printf("sd rd error:%d\r\n", res);
			}
			break;
		case EX_FLASH:	//外部flash
			for(; count>0; count--)
			{
				W25QXX_ReadDatas(buff, sector*FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE);
				sector++;
				buff += FLASH_SECTOR_SIZE;
			}
			res = 0;
			break;
		default:
			res = 1; 
	}
   //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res == 0x00)	return RES_OK;	 
    else 			return RES_ERROR;	   
}
/*********************************************************************************************************
* 函 数 名 : disk_write
* 功能说明 : 写数据到磁盘
* 形    参 : pdrv：物理驱动器编号；buff：数据缓存区；sector：扇区地址；count：写入的扇区数
* 返 回 值 : 状态码
* 备    注 : 无
*********************************************************************************************************/ 
//count:需要写入的扇区数
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	unsigned char res = 0; 
	
    if (!count)	return RES_PARERR;	//count不能等于0，否则返回参数错误		 	 
	switch(pdrv)
	{
		case SD_CARD:		//SD卡
			res = SD_WriteDisk((unsigned char*)buff, sector, count);
			while(res)		//写出错
			{
				SD_Initialize();	//重新初始化SD卡
				res = SD_WriteDisk((unsigned char*)buff, sector, count);	
				//printf("sd wr error:%d\r\n",res);
			}
			break;
		case EX_FLASH:		//外部flash
			for(; count>0; count--)
			{										    
				W25QXX_WritePage((unsigned char*)buff, sector*FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE);
				sector++;
				buff += FLASH_SECTOR_SIZE;
			}
			res = 0;
			break;
		default:
			res = 1; 
	}
    //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res == 0x00)	return RES_OK;	 
    else return 	RES_ERROR;	
}
#endif
/*********************************************************************************************************
* 函 数 名 : disk_ioctl
* 功能说明 : 其他表参数的获得
* 形    参 : pdrv：物理驱动器编号；cmd：控制命令；buff：发送/接收缓存区
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = 0;						  			     
	if(pdrv == SD_CARD)				//SD卡
	{
	    switch(cmd)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
				*(DWORD*)buff = 512; 
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:	//块大小
				*(WORD*)buff = 4096;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:	//SD卡容量
		        *(DWORD*)buff = 8192;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}else if(pdrv==EX_FLASH)		//外部FLASH  
	{
	    switch(cmd)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = FLASH_SECTOR_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = FLASH_BLOCK_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = FLASH_SECTOR_COUNT;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}else res = RES_ERROR;//其他的不支持
    return res;
}
#endif

/*********************************************************************************************************
* 函 数 名 : get_fattime
* 功能说明 : 获取时间，创建文件或更新文件时需要用到
* 形    参 : 无
* 返 回 值 : 时间数据
* 备    注 : User defined function to give a current time to fatfs module      
			 31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31)                                                                                                                                                                                                                                          
			 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2)   
*********************************************************************************************************/                                                                                                                                                                                                                                          
DWORD get_fattime (void)
{		
	DWORD date = 0;
	
	RTC_GetTimeDate();
	date =
	( (
	(RTC_DateStruct.RTC_Year+20) << 25) |
	(RTC_DateStruct.RTC_Month << 21 ) |
	(RTC_DateStruct.RTC_Date << 16 ) |
	(RTC_TimeStruct.RTC_Hours << 11 ) |
	(RTC_TimeStruct.RTC_Minutes << 5 ) |
	(RTC_TimeStruct.RTC_Seconds )
	);
	return date;
}		
/*********************************************************************************************************
* 函 数 名 : ff_memalloc
* 功能说明 : 动态分配内存
* 形    参 : size：申请的内存大小
* 返 回 值 : 申请到的内存块首地址
* 备    注 : 无
*********************************************************************************************************/ 
//void *ff_memalloc (UINT size)			
//{
//	return (void*)mymalloc(SRAMIN,size);
//}
/*********************************************************************************************************
* 函 数 名 : ff_memfree
* 功能说明 : 释放内存
* 形    参 : mf：释放的内存块的首地址
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
//void ff_memfree (void* mf)		 
//{
//	myfree(SRAMIN,mf);
//}

















