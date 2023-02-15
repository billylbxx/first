#include "malloc.h"


/********************* 外部内存初始化*****************************/




#if USE_MEM_EX 

/*外部SRAM内存池*/					
#define MemExPool 	((u8 *)MEMEX_BASE)

/*外部SRAM内存管理项表*/
#define MemExTable	((u16*)(MEMEX_BASE+MEMEX_SIZE))

/*外部内存就绪标志*/
u8 MemExReady = 0;

/*************************************************************************************
* 函数名称：MemEx_Copy
* 功能描述：复制内存
* 入口参数：-src:内存源地址
*			-des:内存目的地址
*			-num:复制的个数
* 出口参数：无
* 返 回 值：无
* 说    明：无
*************************************************************************************/
void MemEx_Copy(void *src, void *des, u32 num)
{	
	u8 *psrc = src;
	u8 *pdes = des;
	while(num--)
	{
		*pdes++ = *psrc++;	
	}		
}


/*************************************************************************************
* 函数名称：MemEx_SetValue
* 功能描述：设置内存的值
* 入口参数：-start:需要设置的内存起始地址
*			-value:需要设置的值
*			-num:设置的字节个数
* 出口参数：无
* 返 回 值：无
* 说    明：无
*************************************************************************************/
void MemEx_SetValue(void *start, u8 value, u32 num)
{
	u8 *pstart = start;
	while(num--)
	{
		*pstart++ = value;
	}			
}


/*************************************************************************************
* 函数名称：MemEx_Init
* 功能描述：内存初始化
* 入口参数：无
* 出口参数：无
* 返 回 值：无
* 说    明：无
*************************************************************************************/
void MemEx_Init(void)
{ 	
	MemEx_SetValue(MemExTable, 0, MEMEX_TABLE_LEN * 2);	//清除内存管理项表
	MemEx_SetValue(MemExPool, 0, MEMEX_SIZE);			//清除内存池
	MemExReady = 1;										//内存就绪
}


/*************************************************************************************
* 函数名称：MemEx_GetRatio
* 功能描述：获取内存使用率
* 入口参数：无
* 出口参数：无
* 返 回 值：-ratio:内存使用率(单位为%)
* 说    明：无
*************************************************************************************/
u8 MemEx_GetRatio(void)
{
	u8 ratio;
	u32 i, used=0;
	for(i=0; i<MEMEX_TABLE_LEN; i++)
	{
		if(MemExTable[i] != 0)
			used++;
	}
	ratio = used * 100 / MEMEX_TABLE_LEN;				
	return ratio; 		
}


/*************************************************************************************
* 函数名称：MemEx_Malloc
* 功能描述：内存申请
* 入口参数：-NumByte:分配长度(字节数)
* 出口参数：无
* 返 回 值：-void*: 分配成功:内存块的首地址; 分配失败:NULL;
* 说    明：无
*************************************************************************************/
void* MemEx_Malloc(u32 NumByte)
{
	u32 i;
	s32 OffSet;											//内存块数偏移量	
	u32 NumBlock;										//需要的内存块数
	u32 CountBlock = 0;									//连续空内存块数计数
	void *MallocAddr;									//分配内存指针	

	if(!MemExReady)										//判断内存是否就绪
		MemEx_Init();
	if(NumByte <= 0)									//不需要分配
		return NULL;	
	NumBlock = NumByte / MEMEX_BLOCK_SIZE;				//求需要的内存块数
	if(NumByte % MEMEX_BLOCK_SIZE)
		NumBlock++;
		
	for(OffSet=MEMEX_TABLE_LEN-1; OffSet>=0; OffSet--)	//搜索整个内存块
	{
		if(!MemExTable[OffSet])							//找到空内存块则计数
			CountBlock++;
		else											//找到空内存块即非连续,清0重新计数
			CountBlock = 0;								
		if(CountBlock == NumBlock)						//找到NumBlock块连续的空内存块
		{
			for(i=0; i<NumBlock; i++)	 				//标注所分配的内存块
			{
				MemExTable[OffSet + i] = NumBlock;
			}
			MallocAddr = (void *)(OffSet * MEMEX_BLOCK_SIZE + MemExPool);	//求出分配内存的起始地址			
			return MallocAddr;												//返回地址
		}					
	}			 
	return NULL;
}


/*************************************************************************************
* 函数名称：MemEx_Free
* 功能描述：释放内存
* 入口参数：-p:释放p所指向的内存空间
* 出口参数：无
* 返 回 值：无
* 说    明：无
*************************************************************************************/
void MemEx_Free(void *p)
{
	if(!MemExReady)													//判断内存是否就绪
	{
		MemEx_Init();
		return; 
	}
			
	if((p >= MemExPool) &&	(p < MemExPool + MEMEX_SIZE))
	{
		u32 i;
		u32 Index = ((u32)p - (u32)MemExPool) / MEMEX_BLOCK_SIZE;	//p所在内存块号
		u32 NumBlock = 	MemExTable[Index];							//获取需要释放的内存块数
		for(i=0; i<NumBlock; i++)
		{
			MemExTable[Index + i] = 0;								//清除内存管理项表	
		}
		return;		  	
	}							
}

#endif //USE_MEM_EX 






/********************* 外部内存初始化*****************************/




#if USE_MEM_IN

/*内部SRAM内存池*/					
u8 MemInPool[MEMIN_SIZE];

/*内部SRAM内存管理项表*/
u16 MemInTable[MEMIN_TABLE_LEN];

/*内部内存就绪标志*/
u8 MemInReady = 0;



/*************************************************************************************
* 函数名称：MemIn_Copy
* 功能描述：复制内存
* 入口参数：-src:内存源地址
*			-des:内存目的地址
*			-num:复制的个数
* 出口参数：无
* 返 回 值：无
* 说    明：无
*************************************************************************************/
void MemIn_Copy(void *src, void *des, u32 num)
{	
	u8 *psrc = src;
	u8 *pdes = des;
	while(num--)
	{
		*pdes++ = *psrc++;	
	}		
}


/*************************************************************************************
* 函数名称：MemIn_SetValue
* 功能描述：设置内存的值
* 入口参数：-start:需要设置的内存起始地址
*			-value:需要设置的值
*			-num:设置的字节个数
* 出口参数：无
* 返 回 值：无
* 说    明：无
*************************************************************************************/
void MemIn_SetValue(void *start, u8 value, u32 num)
{
	u8 *pstart = start;
	while(num--)
	{
		*pstart++ = value;
	}			
}


/*************************************************************************************
* 函数名称：MemIn_Init
* 功能描述：内存初始化
* 入口参数：无
* 出口参数：无
* 返 回 值：无
* 说    明：无
*************************************************************************************/
void MemIn_Init(void)
{ 	
	MemIn_SetValue(MemInTable, 0, MEMIN_TABLE_LEN * 2);	//清除内存管理项表
	MemIn_SetValue(MemInPool, 0, MEMIN_SIZE);			//清除内存池
	MemInReady = 1;										//内存就绪
}


/*************************************************************************************
* 函数名称：MemIn_GetRatio
* 功能描述：获取内存使用率
* 入口参数：无
* 出口参数：无
* 返 回 值：-ratio:内存使用率(单位为%)
* 说    明：无
*************************************************************************************/
u8 MemIn_GetRatio(void)
{
	u8 ratio;
	u32 i, used=0;
	for(i=0; i<MEMIN_TABLE_LEN; i++)
	{
		if(MemInTable[i] != 0)
			used++;
	}
	ratio = used * 100 / MEMIN_TABLE_LEN;				
	return ratio; 		
}


/*************************************************************************************
* 函数名称：MemIn_Malloc
* 功能描述：内存申请
* 入口参数：-NumByte:分配长度(字节数)
* 出口参数：无
* 返 回 值：-void*: 分配成功:内存块的首地址; 分配失败:NULL;
* 说    明：无
*************************************************************************************/
void* MemIn_Malloc(u32 NumByte)
{
	u32 i;
	s32 OffSet;											//内存块数偏移量	
	u32 NumBlock;										//需要的内存块数
	u32 CountBlock = 0;									//连续空内存块数计数
	void *MallocAddr;									//分配内存指针	

	if(!MemInReady)										//判断内存是否就绪
		MemIn_Init();
	if(NumByte <= 0)									//不需要分配
		return NULL;	
	NumBlock = NumByte / MEMIN_BLOCK_SIZE;				//求需要的内存块数
	if(NumByte % MEMIN_BLOCK_SIZE)
		NumBlock++;
		
	for(OffSet=MEMIN_TABLE_LEN-1; OffSet>=0; OffSet--)	//搜索整个内存块
	{
		if(!MemInTable[OffSet])							//找到空内存块则计数
			CountBlock++;
		else											//找到空内存块即非连续,清0重新计数
			CountBlock = 0;								
		if(CountBlock == NumBlock)						//找到NumBlock块连续的空内存块
		{
			for(i=0; i<NumBlock; i++)	 				//标注所分配的内存块
			{
				MemInTable[OffSet + i] = NumBlock;
			}
			MallocAddr = (void *)(OffSet * MEMIN_BLOCK_SIZE + MemInPool);	//求出分配内存的起始地址			
			return MallocAddr;												//返回地址
		}					
	}			 
	return NULL;
}


/*************************************************************************************
* 函数名称：MemIn_Free
* 功能描述：释放内存
* 入口参数：-p:释放p所指向的内存空间
* 出口参数：无
* 返 回 值：无
* 说    明：无
*************************************************************************************/
void MemIn_Free(void *p)
{
	if(!MemInReady)													//判断内存是否就绪
	{
		MemIn_Init();
		return; 
	}
			
	if((p >= MemInPool) &&	(p < MemInPool + MEMIN_SIZE))
	{
		u32 i;
		u32 Index = ((u32)p - (u32)MemInPool) / MEMIN_BLOCK_SIZE;	//p所在内存块号
		u32 NumBlock = 	MemInTable[Index];							//获取需要释放的内存块数
		for(i=0; i<NumBlock; i++)
		{
			MemInTable[Index + i] = 0;								//清除内存管理项表	
		}
		return;		  	
	}					
}
	

#endif	//USE_MEM_IN









