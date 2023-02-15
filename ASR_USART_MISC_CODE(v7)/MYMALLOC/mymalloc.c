#include "mymalloc.h"	   
#include "FreeRTOS.h"
#include "task.h"
#include "stdlib.h"

//�ڴ��(32�ֽڶ���)
__align(32) unsigned char mem1base[MEM1_MAX_SIZE];													//�ڲ�SRAM�ڴ��
__align(32) unsigned char mem2base[MEM2_MAX_SIZE] __attribute__((at(0X68000000)));					//�ⲿSRAM�ڴ��
__align(32) unsigned char mem3base[MEM3_MAX_SIZE];													//�ڲ�CCM�ڴ��
//�ڴ�����
unsigned short mem1mapbase[MEM1_ALLOC_TABLE_SIZE];													//�ڲ�SRAM�ڴ��MAP
unsigned short mem2mapbase[MEM2_ALLOC_TABLE_SIZE] __attribute__((at(0X68000000+MEM2_MAX_SIZE)));	//�ⲿSRAM�ڴ��MAP
unsigned short mem3mapbase[MEM3_ALLOC_TABLE_SIZE];													//�ڲ�CCM�ڴ��MAP
//�ڴ�������	   
const unsigned int memtblsize[SRAMBANK]={MEM1_ALLOC_TABLE_SIZE,MEM2_ALLOC_TABLE_SIZE,MEM3_ALLOC_TABLE_SIZE};	//�ڴ���С
const unsigned int memblksize[SRAMBANK]={MEM1_BLOCK_SIZE,MEM2_BLOCK_SIZE,MEM3_BLOCK_SIZE};					//�ڴ�ֿ��С
const unsigned int memsize[SRAMBANK]={MEM1_MAX_SIZE,MEM2_MAX_SIZE,MEM3_MAX_SIZE};							//�ڴ��ܴ�С

//�ڴ���������
struct _m_mallco_dev mallco_dev=
{
	my_mem_init,							//�ڴ��ʼ��
	my_mem_perused,							//�ڴ�ʹ����
	mem1base, mem2base, mem3base,			//�ڴ��
	mem1mapbase, mem2mapbase, mem3mapbase,	//�ڴ����״̬��
	0, 0, 0,  		 						//�ڴ����δ����
};
/*********************************************************************************************************
* �� �� �� : mymemcpy
* ����˵�� : �����ڴ�
* ��    �� : des��Ŀ�ĵ�ַ��src��Դ��ַ��n����Ҫ���Ƶ��ڴ泤��(�ֽ�Ϊ��λ)
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void mymemcpy(void *des, void *src, unsigned int n)  
{  
    unsigned char *xdes = des;
	unsigned char *xsrc = src; 
	
    while(n--)	*xdes++ = *xsrc++;  
}  
/*********************************************************************************************************
* �� �� �� : mymemset
* ����˵�� : �����ڴ�
* ��    �� : s���ڴ��׵�ַ��c��Ҫ���õ�ֵ��count����Ҫ���õ��ڴ��С(�ֽ�Ϊ��λ)
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void mymemset(void *s, unsigned char c, unsigned int count)  
{  
    unsigned char *xs = s;  
	
    while(count--)	*xs++ = c;  
}
/*********************************************************************************************************
* �� �� �� : my_mem_init
* ����˵�� : �ڴ�����ʼ��  
* ��    �� : memx�������ڴ��
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void my_mem_init(unsigned char memx)  
{  
    mymemset(mallco_dev.memmap[memx], 0, memtblsize[memx] * 2);		//�ڴ�״̬����������  
	mymemset(mallco_dev.membase[memx], 0, memsize[memx]);			//�ڴ��������������  
	mallco_dev.memrdy[memx] = 1;									//�ڴ�����ʼ��OK  
}  
/*********************************************************************************************************
* �� �� �� : my_mem_perused
* ����˵�� : ��ȡ�ڴ�ʹ����
* ��    �� : memx�������ڴ��
* �� �� ֵ : ʹ����(0~100)
* ��    ע : ��
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
* �� �� �� : my_mem_malloc
* ����˵�� : �ڴ���䣨�ڲ����ã�
* ��    �� : memx�������ڴ�飻size��Ҫ������ڴ��С(�ֽ�)
* �� �� ֵ : 0XFFFFFFFF����������������ڴ�ƫ�Ƶ�ַ 
* ��    ע : ��
*********************************************************************************************************/ 
unsigned int my_mem_malloc(unsigned char memx, unsigned int size)  
{  
    signed long offset = 0;  
    unsigned int nmemb = 0;		//��Ҫ���ڴ����  
	unsigned int cmemb = 0;		//�������ڴ����
    unsigned int i;  
	
    if(!mallco_dev.memrdy[memx])	mallco_dev.init(memx);	//δ��ʼ��,��ִ�г�ʼ�� 
    if(size == 0)	return 0XFFFFFFFF;						//����Ҫ����
    nmemb = size / memblksize[memx];  						//��ȡ��Ҫ����������ڴ����
    if(size % memblksize[memx])	nmemb++;  
    for(offset=memtblsize[memx]-1; offset>=0; offset--)		//���������ڴ������  
    {     
		if(!mallco_dev.memmap[memx][offset])	cmemb++;	//�������ڴ��������
		else 									cmemb = 0;	//�����ڴ������
		if(cmemb == nmemb)									//�ҵ�������nmemb�����ڴ��
		{
            for(i=0; i<nmemb; i++)  						//��ע�ڴ��ǿ� 
            {  
                mallco_dev.memmap[memx][offset+i] = nmemb;  
            }  
            return (offset * memblksize[memx]);				//����ƫ�Ƶ�ַ  
		}
    }  
    return 0XFFFFFFFF;										//δ�ҵ����Ϸ����������ڴ��  
}  
/*********************************************************************************************************
* �� �� �� : my_mem_free
* ����˵�� : �ͷ��ڴ棨�ڲ����ã�
* ��    �� : memx�������ڴ�飻offset���ڴ��ַƫ��
* �� �� ֵ : 0���ͷųɹ���1���ͷ�ʧ��;  
* ��    ע : ��
*********************************************************************************************************/ 
unsigned char my_mem_free(unsigned char memx, unsigned int offset)  
{  
    int i;  
	
    if(!mallco_dev.memrdy[memx])	//δ��ʼ��,��ִ�г�ʼ��
	{
		mallco_dev.init(memx);    
        return 1;					//δ��ʼ��  
    }  
    if(offset<memsize[memx])		//ƫ�����ڴ����. 
    {  
        int index = offset/memblksize[memx];			//ƫ�������ڴ�����  
        int nmemb = mallco_dev.memmap[memx][index];		//�ڴ������
        for(i=0; i<nmemb; i++)  						//�ڴ������
        {  
            mallco_dev.memmap[memx][index+i] = 0;  
        }  
        return 0;  
    }else return 2;	//ƫ�Ƴ�����.  
}  
/*********************************************************************************************************
* �� �� �� : myfree
* ����˵�� : �ͷ��ڴ棨�ⲿ���ã� 
* ��    �� : memx�������ڴ�飻ptr���ڴ��׵�ַ 
* �� �� ֵ : ��
* ��    ע : ��
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
* �� �� �� : mymalloc
* ����˵�� : �����ڴ棨�ⲿ���ã�
* ��    �� : memx�������ڴ�飻size���ڴ��С(�ֽ�)
* �� �� ֵ : ���䵽���ڴ��׵�ַ
* ��    ע : ��
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
* �� �� �� : myrealloc
* ����˵�� : ���·����ڴ棨�ⲿ���ã�
* ��    �� : memx�������ڴ�飻ptr��:���ڴ��׵�ַ��size��Ҫ������ڴ��С(�ֽ�)
* �� �� ֵ : �·��䵽���ڴ��׵�ַ
* ��    ע : ��
*********************************************************************************************************/ 
void *myrealloc(unsigned char memx, void *ptr, unsigned int size)  
{  
    unsigned int offset;   
	
    offset = my_mem_malloc(memx, size);   	
    if(offset==0XFFFFFFFF)	return NULL;     
    else  
    {  									   
	    mymemcpy((void*)((unsigned int)mallco_dev.membase[memx] + offset), ptr, size);	//�������ڴ����ݵ����ڴ�   
        myfree(memx, ptr);  											  				//�ͷž��ڴ�
        return (void*)((unsigned int)mallco_dev.membase[memx] + offset);  				//�������ڴ��׵�ַ
    }  
}












