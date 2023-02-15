#include "malloc.h"


/********************* �ⲿ�ڴ��ʼ��*****************************/




#if USE_MEM_EX 

/*�ⲿSRAM�ڴ��*/					
#define MemExPool 	((u8 *)MEMEX_BASE)

/*�ⲿSRAM�ڴ�������*/
#define MemExTable	((u16*)(MEMEX_BASE+MEMEX_SIZE))

/*�ⲿ�ڴ������־*/
u8 MemExReady = 0;

/*************************************************************************************
* �������ƣ�MemEx_Copy
* ���������������ڴ�
* ��ڲ�����-src:�ڴ�Դ��ַ
*			-des:�ڴ�Ŀ�ĵ�ַ
*			-num:���Ƶĸ���
* ���ڲ�������
* �� �� ֵ����
* ˵    ������
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
* �������ƣ�MemEx_SetValue
* ���������������ڴ��ֵ
* ��ڲ�����-start:��Ҫ���õ��ڴ���ʼ��ַ
*			-value:��Ҫ���õ�ֵ
*			-num:���õ��ֽڸ���
* ���ڲ�������
* �� �� ֵ����
* ˵    ������
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
* �������ƣ�MemEx_Init
* �����������ڴ��ʼ��
* ��ڲ�������
* ���ڲ�������
* �� �� ֵ����
* ˵    ������
*************************************************************************************/
void MemEx_Init(void)
{ 	
	MemEx_SetValue(MemExTable, 0, MEMEX_TABLE_LEN * 2);	//����ڴ�������
	MemEx_SetValue(MemExPool, 0, MEMEX_SIZE);			//����ڴ��
	MemExReady = 1;										//�ڴ����
}


/*************************************************************************************
* �������ƣ�MemEx_GetRatio
* ������������ȡ�ڴ�ʹ����
* ��ڲ�������
* ���ڲ�������
* �� �� ֵ��-ratio:�ڴ�ʹ����(��λΪ%)
* ˵    ������
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
* �������ƣ�MemEx_Malloc
* �����������ڴ�����
* ��ڲ�����-NumByte:���䳤��(�ֽ���)
* ���ڲ�������
* �� �� ֵ��-void*: ����ɹ�:�ڴ����׵�ַ; ����ʧ��:NULL;
* ˵    ������
*************************************************************************************/
void* MemEx_Malloc(u32 NumByte)
{
	u32 i;
	s32 OffSet;											//�ڴ����ƫ����	
	u32 NumBlock;										//��Ҫ���ڴ����
	u32 CountBlock = 0;									//�������ڴ��������
	void *MallocAddr;									//�����ڴ�ָ��	

	if(!MemExReady)										//�ж��ڴ��Ƿ����
		MemEx_Init();
	if(NumByte <= 0)									//����Ҫ����
		return NULL;	
	NumBlock = NumByte / MEMEX_BLOCK_SIZE;				//����Ҫ���ڴ����
	if(NumByte % MEMEX_BLOCK_SIZE)
		NumBlock++;
		
	for(OffSet=MEMEX_TABLE_LEN-1; OffSet>=0; OffSet--)	//���������ڴ��
	{
		if(!MemExTable[OffSet])							//�ҵ����ڴ�������
			CountBlock++;
		else											//�ҵ����ڴ�鼴������,��0���¼���
			CountBlock = 0;								
		if(CountBlock == NumBlock)						//�ҵ�NumBlock�������Ŀ��ڴ��
		{
			for(i=0; i<NumBlock; i++)	 				//��ע��������ڴ��
			{
				MemExTable[OffSet + i] = NumBlock;
			}
			MallocAddr = (void *)(OffSet * MEMEX_BLOCK_SIZE + MemExPool);	//��������ڴ����ʼ��ַ			
			return MallocAddr;												//���ص�ַ
		}					
	}			 
	return NULL;
}


/*************************************************************************************
* �������ƣ�MemEx_Free
* �����������ͷ��ڴ�
* ��ڲ�����-p:�ͷ�p��ָ����ڴ�ռ�
* ���ڲ�������
* �� �� ֵ����
* ˵    ������
*************************************************************************************/
void MemEx_Free(void *p)
{
	if(!MemExReady)													//�ж��ڴ��Ƿ����
	{
		MemEx_Init();
		return; 
	}
			
	if((p >= MemExPool) &&	(p < MemExPool + MEMEX_SIZE))
	{
		u32 i;
		u32 Index = ((u32)p - (u32)MemExPool) / MEMEX_BLOCK_SIZE;	//p�����ڴ���
		u32 NumBlock = 	MemExTable[Index];							//��ȡ��Ҫ�ͷŵ��ڴ����
		for(i=0; i<NumBlock; i++)
		{
			MemExTable[Index + i] = 0;								//����ڴ�������	
		}
		return;		  	
	}							
}

#endif //USE_MEM_EX 






/********************* �ⲿ�ڴ��ʼ��*****************************/




#if USE_MEM_IN

/*�ڲ�SRAM�ڴ��*/					
u8 MemInPool[MEMIN_SIZE];

/*�ڲ�SRAM�ڴ�������*/
u16 MemInTable[MEMIN_TABLE_LEN];

/*�ڲ��ڴ������־*/
u8 MemInReady = 0;



/*************************************************************************************
* �������ƣ�MemIn_Copy
* ���������������ڴ�
* ��ڲ�����-src:�ڴ�Դ��ַ
*			-des:�ڴ�Ŀ�ĵ�ַ
*			-num:���Ƶĸ���
* ���ڲ�������
* �� �� ֵ����
* ˵    ������
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
* �������ƣ�MemIn_SetValue
* ���������������ڴ��ֵ
* ��ڲ�����-start:��Ҫ���õ��ڴ���ʼ��ַ
*			-value:��Ҫ���õ�ֵ
*			-num:���õ��ֽڸ���
* ���ڲ�������
* �� �� ֵ����
* ˵    ������
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
* �������ƣ�MemIn_Init
* �����������ڴ��ʼ��
* ��ڲ�������
* ���ڲ�������
* �� �� ֵ����
* ˵    ������
*************************************************************************************/
void MemIn_Init(void)
{ 	
	MemIn_SetValue(MemInTable, 0, MEMIN_TABLE_LEN * 2);	//����ڴ�������
	MemIn_SetValue(MemInPool, 0, MEMIN_SIZE);			//����ڴ��
	MemInReady = 1;										//�ڴ����
}


/*************************************************************************************
* �������ƣ�MemIn_GetRatio
* ������������ȡ�ڴ�ʹ����
* ��ڲ�������
* ���ڲ�������
* �� �� ֵ��-ratio:�ڴ�ʹ����(��λΪ%)
* ˵    ������
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
* �������ƣ�MemIn_Malloc
* �����������ڴ�����
* ��ڲ�����-NumByte:���䳤��(�ֽ���)
* ���ڲ�������
* �� �� ֵ��-void*: ����ɹ�:�ڴ����׵�ַ; ����ʧ��:NULL;
* ˵    ������
*************************************************************************************/
void* MemIn_Malloc(u32 NumByte)
{
	u32 i;
	s32 OffSet;											//�ڴ����ƫ����	
	u32 NumBlock;										//��Ҫ���ڴ����
	u32 CountBlock = 0;									//�������ڴ��������
	void *MallocAddr;									//�����ڴ�ָ��	

	if(!MemInReady)										//�ж��ڴ��Ƿ����
		MemIn_Init();
	if(NumByte <= 0)									//����Ҫ����
		return NULL;	
	NumBlock = NumByte / MEMIN_BLOCK_SIZE;				//����Ҫ���ڴ����
	if(NumByte % MEMIN_BLOCK_SIZE)
		NumBlock++;
		
	for(OffSet=MEMIN_TABLE_LEN-1; OffSet>=0; OffSet--)	//���������ڴ��
	{
		if(!MemInTable[OffSet])							//�ҵ����ڴ�������
			CountBlock++;
		else											//�ҵ����ڴ�鼴������,��0���¼���
			CountBlock = 0;								
		if(CountBlock == NumBlock)						//�ҵ�NumBlock�������Ŀ��ڴ��
		{
			for(i=0; i<NumBlock; i++)	 				//��ע��������ڴ��
			{
				MemInTable[OffSet + i] = NumBlock;
			}
			MallocAddr = (void *)(OffSet * MEMIN_BLOCK_SIZE + MemInPool);	//��������ڴ����ʼ��ַ			
			return MallocAddr;												//���ص�ַ
		}					
	}			 
	return NULL;
}


/*************************************************************************************
* �������ƣ�MemIn_Free
* �����������ͷ��ڴ�
* ��ڲ�����-p:�ͷ�p��ָ����ڴ�ռ�
* ���ڲ�������
* �� �� ֵ����
* ˵    ������
*************************************************************************************/
void MemIn_Free(void *p)
{
	if(!MemInReady)													//�ж��ڴ��Ƿ����
	{
		MemIn_Init();
		return; 
	}
			
	if((p >= MemInPool) &&	(p < MemInPool + MEMIN_SIZE))
	{
		u32 i;
		u32 Index = ((u32)p - (u32)MemInPool) / MEMIN_BLOCK_SIZE;	//p�����ڴ���
		u32 NumBlock = 	MemInTable[Index];							//��ȡ��Ҫ�ͷŵ��ڴ����
		for(i=0; i<NumBlock; i++)
		{
			MemInTable[Index + i] = 0;								//����ڴ�������	
		}
		return;		  	
	}					
}
	

#endif	//USE_MEM_IN









