#include "delay.h"
#include "ucos_ii.h"
//168 000 000
static u8  fac_us=168;							//us��ʱ������			   
static u16 fac_ms=1000/OS_TICKS_PER_SEC;		//ms��ʱ������,��os��,����ÿ�����ĵ�ms��

//��ʱnus
//nus:Ҫ��ʱ��us��.	
//nus:0~204522252(���ֵ��2^32/fac_us@fac_us=21)	    								   
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD��ֵ	    	 
	ticks=nus*fac_us; 						//��Ҫ�Ľ�����     168000000
	OSSchedLock();						//��ֹOS���ȣ���ֹ���us��ʱ
	told=SysTick->VAL;        //�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	}
	OSSchedUnlock();					//�ָ�OS����											    
}  
//��ʱnms
//nms:Ҫ��ʱ��ms��
//nms:0~65535
void delay_ms(u32 nms) //100
{	
	if(OSRunning&&OSIntNesting==0)//���OS�Ѿ�������,���Ҳ������ж�����(�ж����治���������)	    
	{		
		if(nms>=fac_ms)						//��ʱ��ʱ�����OS������ʱ������ 
		{ 
   			OSTimeDly(nms/fac_ms);	//OS��ʱ
		}
		nms%=fac_ms;						//OS�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ    
	}
	delay_us((u32)(nms*1000));			//��ʱ
}

			 



































