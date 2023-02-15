#include "delay.h"
#include "ucos_ii.h"
//168 000 000
static u8  fac_us=168;							//us延时倍乘数			   
static u16 fac_ms=1000/OS_TICKS_PER_SEC;		//ms延时倍乘数,在os下,代表每个节拍的ms数

//延时nus
//nus:要延时的us数.	
//nus:0~204522252(最大值即2^32/fac_us@fac_us=21)	    								   
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD的值	    	 
	ticks=nus*fac_us; 						//需要的节拍数     168000000
	OSSchedLock();						//阻止OS调度，防止打断us延时
	told=SysTick->VAL;        //刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}  
	}
	OSSchedUnlock();					//恢复OS调度											    
}  
//延时nms
//nms:要延时的ms数
//nms:0~65535
void delay_ms(u32 nms) //100
{	
	if(OSRunning&&OSIntNesting==0)//如果OS已经在跑了,并且不是在中断里面(中断里面不能任务调度)	    
	{		
		if(nms>=fac_ms)						//延时的时间大于OS的最少时间周期 
		{ 
   			OSTimeDly(nms/fac_ms);	//OS延时
		}
		nms%=fac_ms;						//OS已经无法提供这么小的延时了,采用普通方式延时    
	}
	delay_us((u32)(nms*1000));			//延时
}

			 



































