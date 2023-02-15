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

#define SD_CARD	 0  //SD��,���Ϊ0
#define EX_FLASH 1	//�ⲿflash,���Ϊ1

#define FLASH_SECTOR_SIZE 	512			  
//����W25Q128��ǰ12M�ֽڸ�fatfs��,12M�ֽں�,���ڴ���ֿ�,�ֿ�ռ��3.09M
//ʣ�ಿ��,���ͻ��Լ���	 			    
unsigned short	FLASH_SECTOR_COUNT = 2048 * 12;	//W25Q1218,ǰ12M�ֽڸ�FATFSռ��
#define FLASH_BLOCK_SIZE   	8     				//ÿ��BLOCK��8������
/*********************************************************************************************************
* �� �� �� : disk_initialize
* ����˵�� : ��ʼ������
* ��    �� : pdrv���������������
* �� �� ֵ : ״̬��
* ��    ע : ��
*********************************************************************************************************/ 
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	unsigned char res = 0;	
    
	switch(pdrv)
	{
		case SD_CARD:						//SD��
			res=SD_Initialize();			//SD����ʼ�� 
  			break;
		case EX_FLASH:						//�ⲿflash
			W25QXX_Init();
			FLASH_SECTOR_COUNT = 2048 * 12;	//W25Q1218,ǰ12M�ֽڸ�FATFSռ�� 
 			break;
		default:
			res=1; 
	}		 
	if(res)return  STA_NOINIT;
	else return 0; 							//��ʼ���ɹ�
}  
/*********************************************************************************************************
* �� �� �� : disk_status
* ����˵�� : ��ô���״̬
* ��    �� : pdrv���������������
* �� �� ֵ : ״̬��
* ��    ע : ��
*********************************************************************************************************/ 
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{ 
	return 0;
} 
/*********************************************************************************************************
* �� �� �� : disk_read
* ����˵�� : ����������
* ��    �� : pdrv��������������ţ�buff�����ݻ�������sector��������ַ��count����ȡ��������
* �� �� ֵ : ״̬��
* ��    ע : ��
*********************************************************************************************************/ 
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	unsigned char res = 0; 
	
    if (!count)	return RES_PARERR;	//count���ܵ���0�����򷵻ز�������		 	 
	switch(pdrv)
	{
		case SD_CARD:	//SD��
			res = SD_ReadDisk(buff, sector, count);	 
			while(res)	//������
			{
				SD_Initialize();	//���³�ʼ��SD��
				res=SD_ReadDisk(buff, sector, count);	
				//printf("sd rd error:%d\r\n", res);
			}
			break;
		case EX_FLASH:	//�ⲿflash
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
   //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
    if(res == 0x00)	return RES_OK;	 
    else 			return RES_ERROR;	   
}
/*********************************************************************************************************
* �� �� �� : disk_write
* ����˵�� : д���ݵ�����
* ��    �� : pdrv��������������ţ�buff�����ݻ�������sector��������ַ��count��д���������
* �� �� ֵ : ״̬��
* ��    ע : ��
*********************************************************************************************************/ 
//count:��Ҫд���������
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	unsigned char res = 0; 
	
    if (!count)	return RES_PARERR;	//count���ܵ���0�����򷵻ز�������		 	 
	switch(pdrv)
	{
		case SD_CARD:		//SD��
			res = SD_WriteDisk((unsigned char*)buff, sector, count);
			while(res)		//д����
			{
				SD_Initialize();	//���³�ʼ��SD��
				res = SD_WriteDisk((unsigned char*)buff, sector, count);	
				//printf("sd wr error:%d\r\n",res);
			}
			break;
		case EX_FLASH:		//�ⲿflash
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
    //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
    if(res == 0x00)	return RES_OK;	 
    else return 	RES_ERROR;	
}
#endif
/*********************************************************************************************************
* �� �� �� : disk_ioctl
* ����˵�� : ����������Ļ��
* ��    �� : pdrv��������������ţ�cmd���������buff������/���ջ�����
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = 0;						  			     
	if(pdrv == SD_CARD)				//SD��
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
		    case GET_BLOCK_SIZE:	//���С
				*(WORD*)buff = 4096;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:	//SD������
		        *(DWORD*)buff = 8192;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}else if(pdrv==EX_FLASH)		//�ⲿFLASH  
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
	}else res = RES_ERROR;//�����Ĳ�֧��
    return res;
}
#endif

/*********************************************************************************************************
* �� �� �� : get_fattime
* ����˵�� : ��ȡʱ�䣬�����ļ�������ļ�ʱ��Ҫ�õ�
* ��    �� : ��
* �� �� ֵ : ʱ������
* ��    ע : User defined function to give a current time to fatfs module      
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
* �� �� �� : ff_memalloc
* ����˵�� : ��̬�����ڴ�
* ��    �� : size��������ڴ��С
* �� �� ֵ : ���뵽���ڴ���׵�ַ
* ��    ע : ��
*********************************************************************************************************/ 
//void *ff_memalloc (UINT size)			
//{
//	return (void*)mymalloc(SRAMIN,size);
//}
/*********************************************************************************************************
* �� �� �� : ff_memfree
* ����˵�� : �ͷ��ڴ�
* ��    �� : mf���ͷŵ��ڴ����׵�ַ
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
//void ff_memfree (void* mf)		 
//{
//	myfree(SRAMIN,mf);
//}

















