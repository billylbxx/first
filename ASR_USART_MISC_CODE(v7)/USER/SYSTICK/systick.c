#include "systick.h"
#include "lvgl.h"

static unsigned char fac_us  = 0;	//us��ʱ������
static unsigned short fac_ms = 0;	//ms��ʱ������

#if ENABLED_RTOS		//���ʹ����RTOS
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
	* �� �� �� : Systick_Inti
	* ����˵�� : ��ʼ��ϵͳ�δ�
	* ��    �� : clk��ϵͳ����Ƶ��
	* �� �� ֵ : ��
	* ��    ע : �ڲ�ʱ��Դ = HCLK = 168M��
	*********************************************************************************************************/ 
	void Systick_Init(unsigned char clk)
	{
		unsigned int reload = 0;
		SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);	//ѡ���ڲ�ʱ��  HCLK
		fac_us = clk;										//�����Ƿ�ʹ��OS,fac_us����Ҫʹ��
		reload = clk;										//ÿ���ӵļ������� ��λΪM  
		reload *= 1000000/configTICK_RATE_HZ;				//����configTICK_RATE_HZ�趨���ʱ��
															//reloadΪ24λ�Ĵ���,���ֵ:16777216,��168M��,Լ��0.099s����	
		fac_ms = 1000/configTICK_RATE_HZ;					//����OS������ʱ�����ٵ�λ	   
		SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;   		//����SYSTICK�ж�
		SysTick->LOAD = reload; 							//ÿ1/configTICK_RATE_HZ���ж�һ��	
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   		//����SYSTICK    
	}
	/*********************************************************************************************************
	* �� �� �� : Delay_Us
	* ����˵�� : us���ӳ�
	* ��    �� : us����Ҫ�ӳٵ�us��
	* �� �� ֵ : ��
	* ��    ע : ���ᷢ���������
	*********************************************************************************************************/ 
	void Delay_Us(unsigned int us)
	{
		unsigned int ticks = 0;
		unsigned int told = 0, tnow = 0, tcnt = 0;
		unsigned int reload = SysTick->LOAD;			//LOAD��ֵ	    	 
		ticks = us*fac_us; 								//��Ҫ�Ľ����� 
		told = SysTick->VAL;        					//�ս���ʱ�ļ�����ֵ
		while(1)
		{
			tnow = SysTick->VAL;	
			if(tnow != told)
			{	    
				if(tnow < told)	tcnt += told-tnow;	//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
				else 			tcnt += reload-tnow+told;	    
				told = tnow;
				if(tcnt >= ticks)	break;			//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
			}  
		}						
	}
	/*********************************************************************************************************
	* �� �� �� : Delay_Ms
	* ����˵�� : ms���ӳ�
	* ��    �� : ms��Ҫ�ӳٵ�ms��
	* �� �� ֵ : ��
	* ��    ע : �ᷢ���������
	*********************************************************************************************************/ 
	void Delay_Ms(u32 ms)
	{	
		if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)	//ϵͳ�Ѿ�����
		{		
			if(ms >= fac_ms)				//��ʱ��ʱ�����OS������ʱ������ 
				vTaskDelay(ms/fac_ms);	 	//FreeRTOS��ʱ
			ms %= fac_ms;					//OS�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ    
		}
		if(ms)	Delay_Us((u32)(ms*1000));							//��ͨ��ʽ��ʱ
	}
#else

	/*********************************************************************************************************
	* �� �� �� : Systick_Init
	* ����˵�� : ��ʼ��ϵͳ�δ�
	* ��    �� : clk��ϵͳ����Ƶ��
	* �� �� ֵ : ��
	* ��    ע : �ⲿʱ��Դ = ϵͳ����Ƶ�� / 8��
	*********************************************************************************************************/ 
	void Systick_Init(unsigned char clk)
	{
		SysTick->CTRL &= ~(1<<2);	//ѡ���ⲿʱ����Ϊ�δ��ʱ��Դ
		fac_us = clk / 8.0;			//�õ���1us��Ҫ�Ƶ���
		fac_ms = clk / 8.0 * 1000;	//�õ���1ms��Ҫ�Ƶ���
	}
	/*********************************************************************************************************
	* �� �� �� : Delay_Us
	* ����˵�� : us���ӳ�
	* ��    �� : us����Ҫ�ӳٵ�us��
	* �� �� ֵ : ��
	* ��    ע : ����ӳ�ʱ�� = ��2^24-1��/ 10.5 = 798915(us)��ע�⣺������Ҫ���ж��е���
	*********************************************************************************************************/ 
	void Delay_Us(unsigned int us)
	{
		unsigned int temp = 0;
		SysTick->LOAD = us*fac_us;				//��10.5������1us
		SysTick->VAL  = 0;						//�����ǰ����ֵ
		SysTick->CTRL |= (1<<0);				//ʹ�ܼ�����
		do
		{										//����Ҫ�ȶ����Ĵ�����ֵ���жϣ������������
			temp = SysTick->CTRL;				//�ȴ���16λΪ1
		}while(!(temp&(1<<16))&&(temp&(1<<0)));	//����ȷ����ʱ����������
		SysTick->CTRL &= ~(1<<0);				//�رռ�����
	}
	/*********************************************************************************************************
	* �� �� �� : Delay_Ms
	* ����˵�� : ms���ӳ�
	* ��    �� : ms��Ҫ�ӳٵ�ms��
	* �� �� ֵ : ��
	* ��    ע : ����ӳ� = ��2^24-1��/ 21 = 798(ms)
	*********************************************************************************************************/ 
	void Delay_Ms(unsigned int Ms)
	{
		unsigned int temp = 0;
		SysTick->LOAD = Ms*fac_ms;					//��10.5������1us��1ms��1000us
		SysTick->VAL  = 0;							//�����ǰ����ֵ
		SysTick->CTRL |= (1<<0);					//ʹ�ܼ�����
		do
		{											//����Ҫ�ȶ����Ĵ�����ֵ���жϣ������������
			temp = SysTick->CTRL;					//�ȴ���16λΪ1
		}while(!(temp&(1<<16))&&(temp&(1<<0)));		//����ȷ����ʱ����������
		SysTick->CTRL &= ~(1<<0);					//�رռ�����
	}

#endif
