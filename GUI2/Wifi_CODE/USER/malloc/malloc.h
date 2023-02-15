#ifndef __MALLOC__H
#define __MALLOC__H
#include "stm32f4xx.h"

/*定义NULL宏*/
#ifndef NULL
#define NULL 	0
#endif	//NULL

/*控制是否对SRAM内存管理*/
#define	USE_MEM_IN		1
#define	USE_MEM_EX		1

/*内部SRAM内存参数设置*/
#define MEMIN_BLOCK_SIZE		16							 		//内存块的大小(字节数)
#define MEMIN_BLOCK_NUM			3200								//内存块个数
#define MEMIN_TABLE_LEN			MEMIN_BLOCK_NUM						//内存管理项表长度
#define	MEMIN_SIZE				(MEMIN_BLOCK_NUM*MEMIN_BLOCK_SIZE)	//内存池大小(100KB)

/*外部SRAM内存参数设置*/
#define MEMEX_BASE          0x68000000							//外部内存基地址
#define MEMEX_BLOCK_SIZE		32							 		//内存块的大小(字节数)
#define MEMEX_BLOCK_NUM			30000								//内存块个数
#define MEMEX_TABLE_LEN			MEMEX_BLOCK_NUM						//内存管理项表长度
#define	MEMEX_SIZE				(MEMEX_BLOCK_NUM*MEMEX_BLOCK_SIZE)	//内存池大小(937.5KB)

/*函数声明*/
void MemEx_Init(void);
void MemIn_Init(void);
u8 MemEx_GetRatio(void);
u8 MemIn_GetRatio(void);
void* MemEx_Malloc(u32 NumByte);
void* MemIn_Malloc(u32 NumByte);
void MemEx_Free(void *p);
void MemIn_Free(void *p);


#endif



