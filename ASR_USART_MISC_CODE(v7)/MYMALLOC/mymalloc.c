#include "mymalloc.h"	   
#include "FreeRTOS.h"
#include "task.h"
#include "stdlib.h"

//内存池(32字节对齐)
__align(32) unsigned char mem1base[MEM1_MAX_SIZE];													//内部SRAM内存池
__align(32) unsigned char mem2base[MEM2_MAX_SIZE] __attribute__((at(0X68000000)));					//外部SRAM内存池
__align(32) unsigned char mem3base[MEM3_MAX_SIZE];													//内部CCM内存池
//内存管理表
unsigned short mem1mapbase[MEM1_ALLOC_TABLE_SIZE];													//内部SRAM内存池MAP
unsigned short mem2mapbase[MEM2_ALLOC_TABLE_SIZE] __attribute__((at(0X68000000+MEM2_MAX_SIZE)));	//外部SRAM内存池MAP
unsigned short mem3mapbase[MEM3_ALLOC_TABLE_SIZE];													//内部CCM内存池MAP
//内存管理参数	   
const unsigned int memtblsize[SRAMBANK]={MEM1_ALLOC_TABLE_SIZE,MEM2_ALLOC_TABLE_SIZE,MEM3_ALLOC_TABLE_SIZE};	//内存表大小
const unsigned int memblksize[SRAMBANK]={MEM1_BLOCK_SIZE,MEM2_BLOCK_SIZE,MEM3_BLOCK_SIZE};					//内存分块大小
const unsigned int memsize[SRAMBANK]={MEM1_MAX_SIZE,MEM2_MAX_SIZE,MEM3_MAX_SIZE};							//内存总大小

//内存管理控制器
struct _m_mallco_dev mallco_dev=
{
	my_mem_init,							//内存初始化
	my_mem_perused,							//内存使用率
	mem1base, mem2base, mem3base,			//内存池
	mem1mapbase, mem2mapbase, mem3mapbase,	//内存管理状态表
	0, 0, 0,  		 						//内存管理未就绪
};
/*********************************************************************************************************
* 函 数 名 : mymemcpy
* 功能说明 : 复制内存
* 形    参 : des：目的地址；src：源地址；n：需要复制的内存长度(字节为单位)
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void mymemcpy(void *des, void *src, unsigned int n)  
{  
    unsigned char *xdes = des;
	unsigned char *xsrc = src; 
	
    while(n--)	*xdes++ = *xsrc++;  
}  
/*********************************************************************************************************
* 函 数 名 : mymemset
* 功能说明 : 设置内存
* 形    参 : s：内存首地址；c：要设置的值；count：需要设置的内存大小(字节为单位)
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void mymemset(void *s, unsigned char c, unsigned int count)  
{  
    unsigned char *xs = s;  
	
    while(count--)	*xs++ = c;  
}
/*********************************************************************************************************
* 函 数 名 : my_mem_init
* 功能说明 : 内存管理初始化  
* 形    参 : memx：所属内存块
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void my_mem_init(unsigned char memx)  
{  
    mymemset(mallco_dev.memmap[memx], 0, memtblsize[memx] * 2);		//内存状态表数据清零  
	mymemset(mallco_dev.membase[memx], 0, memsize[memx]);			//内存池所有数据清零  
	mallco_dev.memrdy[memx] = 1;									//内存管理初始化OK  
}  
/*********************************************************************************************************
* 函 数 名 : my_mem_perused
* 功能说明 : 获取内存使用率
* 形    参 : memx：所属内存块
* 返 回 值 : 使用率(0~100)
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char my_mem_perused(unsigned char memx)  
{  
    unsigned int used = 0;  
    unsigned int i;  
	
    for(i=0; i<memtblsize[memx]; i++)  
    {  
        if(mallco_dev.memmap[memx][i])	used++; 
    } 
    return (used * 100) / (memtblsize[memx]);  
}  
/*********************************************************************************************************
* 函 数 名 : my_mem_malloc
* 功能说明 : 内存分配（内部调用）
* 形    参 : memx：所属内存块；size：要分配的内存大小(字节)
* 返 回 值 : 0XFFFFFFFF：代表错误；其他：内存偏移地址 
* 备    注 : 无
*********************************************************************************************************/ 
unsigned int my_mem_malloc(unsigned char memx, unsigned int size)  
{  
    signed long offset = 0;  
    unsigned int nmemb = 0;		//需要的内存块数  
	unsigned int cmemb = 0;		//连续空内存块数
    unsigned int i;  
	
    if(!mallco_dev.memrdy[memx])	mallco_dev.init(memx);	//未初始化,先执行初始化 
    if(size == 0)	return 0XFFFFFFFF;						//不需要分配
    nmemb = size / memblksize[memx];  						//获取需要分配的连续内存块数
    if(size % memblksize[memx])	nmemb++;  
    for(offset=memtblsize[memx]-1; offset>=0; offset--)		//搜索整个内存控制区  
    {     
		if(!mallco_dev.memmap[memx][offset])	cmemb++;	//连续空内存块数增加
		else 									cmemb = 0;	//连续内存块清零
		if(cmemb == nmemb)									//找到了连续nmemb个空内存块
		{
            for(i=0; i<nmemb; i++)  						//标注内存块非空 
            {  
                mallco_dev.memmap[memx][offset+i] = nmemb;  
            }  
            return (offset * memblksize[memx]);				//返回偏移地址  
		}
    }  
    return 0XFFFFFFFF;										//未找到符合分配条件的内存块  
}  
/*********************************************************************************************************
* 函 数 名 : my_mem_free
* 功能说明 : 释放内存（内部调用）
* 形    参 : memx：所属内存块；offset：内存地址偏移
* 返 回 值 : 0：释放成功；1：释放失败;  
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char my_mem_free(unsigned char memx, unsigned int offset)  
{  
    int i;  
	
    if(!mallco_dev.memrdy[memx])	//未初始化,先执行初始化
	{
		mallco_dev.init(memx);    
        return 1;					//未初始化  
    }  
    if(offset<memsize[memx])		//偏移在内存池内. 
    {  
        int index = offset/memblksize[memx];			//偏移所在内存块号码  
        int nmemb = mallco_dev.memmap[memx][index];		//内存块数量
        for(i=0; i<nmemb; i++)  						//内存块清零
        {  
            mallco_dev.memmap[memx][index+i] = 0;  
        }  
        return 0;  
    }else return 2;	//偏移超区了.  
}  
/*********************************************************************************************************
* 函 数 名 : myfree
* 功能说明 : 释放内存（外部调用） 
* 形    参 : memx：所属内存块；ptr：内存首地址 
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void myfree(unsigned char memx,void *ptr)  
{  
	unsigned int offset = 0;   	
	
	if(ptr == NULL)	return;
	
	if(memx == SRAMCCM)
	{
		vPortFree(ptr);
	}
	else
	{
		offset = (unsigned int)ptr - (unsigned int)mallco_dev.membase[memx];   
		my_mem_free(memx, offset);
	}
}  
/*********************************************************************************************************
* 函 数 名 : mymalloc
* 功能说明 : 分配内存（外部调用）
* 形    参 : memx：所属内存块；size：内存大小(字节)
* 返 回 值 : 分配到的内存首地址
* 备    注 : 无
*********************************************************************************************************/ 
void *mymalloc(unsigned char memx,unsigned int size)  
{  
	unsigned int offset = 0; 
	
	if(memx == SRAMCCM)
	{
		return pvPortMalloc(size);
	}
	else 
	{
		offset = my_mem_malloc(memx,size); 
		if(offset == 0XFFFFFFFF)	return NULL; 
		else 						return (void *)((unsigned int)mallco_dev.membase[memx] + offset); 
	}
}  
/*********************************************************************************************************
* 函 数 名 : myrealloc
* 功能说明 : 重新分配内存（外部调用）
* 形    参 : memx：所属内存块；ptr：:旧内存首地址；size：要分配的内存大小(字节)
* 返 回 值 : 新分配到的内存首地址
* 备    注 : 无
*********************************************************************************************************/ 
void *myrealloc(unsigned char memx, void *ptr, unsigned int size)  
{  
    unsigned int offset;   
	
    offset = my_mem_malloc(memx, size);   	
    if(offset==0XFFFFFFFF)	return NULL;     
    else  
    {  									   
	    mymemcpy((void*)((unsigned int)mallco_dev.membase[memx] + offset), ptr, size);	//拷贝旧内存内容到新内存   
        myfree(memx, ptr);  											  				//释放旧内存
        return (void*)((unsigned int)mallco_dev.membase[memx] + offset);  				//返回新内存首地址
    }  
}












