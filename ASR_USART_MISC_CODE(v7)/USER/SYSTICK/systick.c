#include "systick.h"
#include "lvgl.h"

static unsigned char fac_us  = 0;	//us延时倍乘数
static unsigned short fac_ms = 0;	//ms延时倍乘数

#if ENABLED_RTOS		//如果使能了RTOS
	#include "FreeRTOS.h"
	#include "task.h"

	extern void xPortSysTickHandler(void);
	void SysTick_Handler(void)
	{
		if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
		{
			xPortSysTickHandler();
		}
		lv_tick_inc(5);
	}
	/*********************************************************************************************************
	* 函 数 名 : Systick_Inti
	* 功能说明 : 初始化系统滴答
	* 形    参 : clk：系统运行频率
	* 返 回 值 : 无
	* 备    注 : 内部时钟源 = HCLK = 168M；
	*********************************************************************************************************/ 
	void Systick_Init(unsigned char clk)
	{
		unsigned int reload = 0;
		SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);	//选择内部时钟  HCLK
		fac_us = clk;										//不论是否使用OS,fac_us都需要使用
		reload = clk;										//每秒钟的计数次数 单位为M  
		reload *= 1000000/configTICK_RATE_HZ;				//根据configTICK_RATE_HZ设定溢出时间
															//reload为24位寄存器,最大值:16777216,在168M下,约合0.099s左右	
		fac_ms = 1000/configTICK_RATE_HZ;					//代表OS可以延时的最少单位	   
		SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;   		//开启SYSTICK中断
		SysTick->LOAD = reload; 							//每1/configTICK_RATE_HZ秒中断一次	
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   		//开启SYSTICK    
	}
	/*********************************************************************************************************
	* 函 数 名 : Delay_Us
	* 功能说明 : us级延迟
	* 形    参 : us：需要延迟的us数
	* 返 回 值 : 无
	* 备    注 : 不会发生任务调度
	*********************************************************************************************************/ 
	void Delay_Us(unsigned int us)
	{
		unsigned int ticks = 0;
		unsigned int told = 0, tnow = 0, tcnt = 0;
		unsigned int reload = SysTick->LOAD;			//LOAD的值	    	 
		ticks = us*fac_us; 								//需要的节拍数 
		told = SysTick->VAL;        					//刚进入时的计数器值
		while(1)
		{
			tnow = SysTick->VAL;	
			if(tnow != told)
			{	    
				if(tnow < told)	tcnt += told-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
				else 			tcnt += reload-tnow+told;	    
				told = tnow;
				if(tcnt >= ticks)	break;			//时间超过/等于要延迟的时间,则退出.
			}  
		}						
	}
	/*********************************************************************************************************
	* 函 数 名 : Delay_Ms
	* 功能说明 : ms级延迟
	* 形    参 : ms：要延迟的ms数
	* 返 回 值 : 无
	* 备    注 : 会发生任务调度
	*********************************************************************************************************/ 
	void Delay_Ms(u32 ms)
	{	
		if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)	//系统已经运行
		{		
			if(ms >= fac_ms)				//延时的时间大于OS的最少时间周期 
				vTaskDelay(ms/fac_ms);	 	//FreeRTOS延时
			ms %= fac_ms;					//OS已经无法提供这么小的延时了,采用普通方式延时    
		}
		if(ms)	Delay_Us((u32)(ms*1000));							//普通方式延时
	}
#else

	/*********************************************************************************************************
	* 函 数 名 : Systick_Init
	* 功能说明 : 初始化系统滴答
	* 形    参 : clk：系统运行频率
	* 返 回 值 : 无
	* 备    注 : 外部时钟源 = 系统运行频率 / 8；
	*********************************************************************************************************/ 
	void Systick_Init(unsigned char clk)
	{
		SysTick->CTRL &= ~(1<<2);	//选择外部时钟作为滴答的时钟源
		fac_us = clk / 8.0;			//得到计1us需要计的数
		fac_ms = clk / 8.0 * 1000;	//得到计1ms需要计的数
	}
	/*********************************************************************************************************
	* 函 数 名 : Delay_Us
	* 功能说明 : us级延迟
	* 形    参 : us：需要延迟的us数
	* 返 回 值 : 无
	* 备    注 : 最大延迟时间 = （2^24-1）/ 10.5 = 798915(us)，注意：尽量不要在中断中调用
	*********************************************************************************************************/ 
	void Delay_Us(unsigned int us)
	{
		unsigned int temp = 0;
		SysTick->LOAD = us*fac_us;				//计10.5个数是1us
		SysTick->VAL  = 0;						//清除当前计数值
		SysTick->CTRL |= (1<<0);				//使能计数器
		do
		{										//必须要先读出寄存器的值再判断，否则会产生误差
			temp = SysTick->CTRL;				//等待第16位为1
		}while(!(temp&(1<<16))&&(temp&(1<<0)));	//并且确定定时器还在运行
		SysTick->CTRL &= ~(1<<0);				//关闭计数器
	}
	/*********************************************************************************************************
	* 函 数 名 : Delay_Ms
	* 功能说明 : ms级延迟
	* 形    参 : ms：要延迟的ms数
	* 返 回 值 : 无
	* 备    注 : 最大延迟 = （2^24-1）/ 21 = 798(ms)
	*********************************************************************************************************/ 
	void Delay_Ms(unsigned int Ms)
	{
		unsigned int temp = 0;
		SysTick->LOAD = Ms*fac_ms;					//计10.5个数是1us，1ms有1000us
		SysTick->VAL  = 0;							//清除当前计数值
		SysTick->CTRL |= (1<<0);					//使能计数器
		do
		{											//必须要先读出寄存器的值再判断，否则会产生误差
			temp = SysTick->CTRL;					//等待第16位为1
		}while(!(temp&(1<<16))&&(temp&(1<<0)));		//并且确定定时器还在运行
		SysTick->CTRL &= ~(1<<0);					//关闭计数器
	}

#endif
