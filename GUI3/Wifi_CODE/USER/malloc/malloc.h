#ifndef __MALLOC__H
#define __MALLOC__H
#include "stm32f4xx.h"

/*����NULL��*/
#ifndef NULL
#define NULL 	0
#endif	//NULL

/*�����Ƿ��SRAM�ڴ����*/
#define	USE_MEM_IN		1
#define	USE_MEM_EX		1

/*�ڲ�SRAM�ڴ��������*/
#define MEMIN_BLOCK_SIZE		16							 		//�ڴ��Ĵ�С(�ֽ���)
#define MEMIN_BLOCK_NUM			3200								//�ڴ�����
#define MEMIN_TABLE_LEN			MEMIN_BLOCK_NUM						//�ڴ���������
#define	MEMIN_SIZE				(MEMIN_BLOCK_NUM*MEMIN_BLOCK_SIZE)	//�ڴ�ش�С(100KB)

/*�ⲿSRAM�ڴ��������*/
#define MEMEX_BASE          0x68000000							//�ⲿ�ڴ����ַ
#define MEMEX_BLOCK_SIZE		32							 		//�ڴ��Ĵ�С(�ֽ���)
#define MEMEX_BLOCK_NUM			30000								//�ڴ�����
#define MEMEX_TABLE_LEN			MEMEX_BLOCK_NUM						//�ڴ���������
#define	MEMEX_SIZE				(MEMEX_BLOCK_NUM*MEMEX_BLOCK_SIZE)	//�ڴ�ش�С(937.5KB)

/*��������*/
void MemEx_Init(void);
void MemIn_Init(void);
u8 MemEx_GetRatio(void);
u8 MemIn_GetRatio(void);
void* MemEx_Malloc(u32 NumByte);
void* MemIn_Malloc(u32 NumByte);
void MemEx_Free(void *p);
void MemIn_Free(void *p);


#endif



