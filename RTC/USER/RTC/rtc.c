#include "stm32f4xx.h"
#include "key.h"
#include "led.h"
#include "uart.h"
#include "exti.h"
#include "systick.h"
#include "stdio.h"
#include "rtc.h"
/*一、RTC日历配置一般步骤
1、使能PWR时钟：RCC_APB1PeriphClockCmd();
2、使能后备寄存器访问：PWR_BackupAccessCmd();
3、配置RTC时钟源，使能RTC时钟：RCC_RTCCLKConfig();  RCC_RTCCLKCmd();如果要使用LSE，要打开LSE：RCC_LSEConfig(RCC_LSE_ON);
4、初始化RTC（同步/异步分系数和时钟格式）：RTC_Init();
5、设置时间：RTC_SetTime();
6、设置日期：RTC_SetDate();*/

u8 buf[50];
u8 buf1[50];
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;
void RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
	RTC_TimeStruct.RTC_H12 =ampm;
  RTC_TimeStruct.RTC_Hours = hour;
  RTC_TimeStruct.RTC_Minutes = min;
  RTC_TimeStruct.RTC_Seconds = sec; 	
	RTC_SetTime(RTC_Format_BIN,&RTC_TimeStruct);
}

void RTC_Set_Date(u8 weekday,u16 year,u8 month,u8 date)
{
	RTC_DateStruct.RTC_WeekDay =weekday;
  RTC_DateStruct.RTC_Date = date;
  RTC_DateStruct.RTC_Month =month;
  RTC_DateStruct.RTC_Year = year;
//	RTC_DateStructInit(&RTC_DateStruct);//不能加这句，加了就设置不了时间
 	RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);
	
	
}

u8 rtc_Init(void)
{
	u16 ret=0x1FFF;
	u16 RTC_BKP=0x8579;  //随便改变这个值可以重新设置时间
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	PWR_BackupAccessCmd(ENABLE);//使能或者失能 RTC 和后备寄存器访问（系统复位或从待机模式唤醒后，RTC的设置和时间维持不变。）
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR19)!=RTC_BKP)
	{
		RCC_LSEConfig(RCC_LSE_ON);
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET)//等待外部晶体振荡器准备好	
		{
			ret--;
			Delay_ms(10);
		}
		if(ret==0) return 1;//LSE开启失败
		
	
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	RCC_RTCCLKCmd(ENABLE);
		
	RTC_InitTypeDef RTC_InitStruct;
	RTC_InitStruct.RTC_AsynchPrediv=0x7F;//0X7FFF=32767,所以32768HZ/PSC+1=32768/32767+1=1HZ,定时1s
	RTC_InitStruct.RTC_SynchPrediv=0XFF;
	RTC_InitStruct.RTC_HourFormat=RTC_HourFormat_24;
	RTC_StructInit(&RTC_InitStruct);
	RTC_Init(&RTC_InitStruct);
  
  RTC_Set_Time(23,59,45,RTC_H12_AM);		
	RTC_Set_Date(3,22,9,14);
	RTC_SetAlarmA(3,23,59,50);
	RTC_SetAlarmB(3,23,59,55);
	RTC_WriteBackupRegister(RTC_BKP_DR19, RTC_BKP);//判断是否要重置日期和时间
	}
	
	return 0;
}

void RTC_Set_WakeUp(void)
{
	RTC_WakeUpCmd(DISABLE);//关闭唤醒功能
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);//为唤醒功能选择RTC配置好的时钟源
	RTC_SetWakeUpCounter(0);
	
	RTC_ClearITPendingBit(RTC_IT_WUT);
	EXTI_ClearITPendingBit(EXTI_Line22);
	
	RTC_ITConfig(RTC_IT_WUT, ENABLE);//配置RTC唤醒中断
	RTC_WakeUpCmd(ENABLE);
  
	
	EXTI_InitTypeDef EXTI_InitStruct={0};
	EXTI_InitStruct.EXTI_Line = EXTI_Line22;//EXTI 线 22 连接到 RTC 唤醒事件
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=RTC_WKUP_IRQn;//stm32f4xx.h
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;//先占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;//从优先级
	NVIC_Init(&NVIC_InitStruct);
}

void RTC_WKUP_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_WUT)!=RESET)
	{
	  RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
		sprintf((char*)buf,"Time:%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
		printf("%s\r\n",buf);
		
		RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
		sprintf((char*)buf1,"DATA:%d:%d:%02d:%02d",RTC_DateStruct.RTC_WeekDay, RTC_DateStruct.RTC_Year, RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date );
		printf("%s\r\n",buf1);
		LED3=~LED3;
	}
	RTC_ClearITPendingBit(RTC_IT_WUT);
	EXTI_ClearITPendingBit(EXTI_Line22);
}

/*********************************************************************************************************
* 函 数 名 : RTC_SetAlarmA
* 功能说明 : 设置闹钟A时间
* 形    参 : weekday:星期几(1~7) hour,min,sec:小时,分钟,秒钟
* 返 回 值 : 无
* 备    注 : 1.先关闭闹钟
			 2.再设置时间
			 3.中断配置（配置中断 1.闹钟中断开关使能 ；2.EXTI 开关使能；3.NVIC 开关使能）
*********************************************************************************************************/ 
void RTC_SetAlarmA(u8 weekday,u8 hour,u8 min,u8 sec)
{
	RTC_AlarmTypeDef RTC_AlarmStruct = {0};
	EXTI_InitTypeDef EXTI_InitStruct = {0};
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);	//关闭闹钟A
	
	RTC_TimeStruct.RTC_H12 =RTC_H12_AM;
  RTC_TimeStruct.RTC_Hours = hour;
  RTC_TimeStruct.RTC_Minutes = min;
  RTC_TimeStruct.RTC_Seconds = sec;

	
	RTC_AlarmStruct.RTC_AlarmDateWeekDay=weekday;//星期几生效
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_WeekDay;//星期生效
	RTC_AlarmStruct.RTC_AlarmMask=RTC_AlarmMask_None; //全部字段有效
	RTC_AlarmStruct.RTC_AlarmTime=RTC_TimeStruct; //闹钟时间
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStruct);

	
	EXTI_InitStruct.EXTI_Line = EXTI_Line17;
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel 	= RTC_Alarm_IRQn;	//中断源
	NVIC_InitStruct.NVIC_IRQChannelCmd  = ENABLE;			//使能中断源
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority 		  = 1;	//响应优先级2
	NVIC_Init(&NVIC_InitStruct);	
	
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);		//使能闹钟A中断
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);		//开启闹钟A
}


/*********************************************************************************************************
* 函 数 名 : RTC_SetAlarmA
* 功能说明 : 设置闹钟A时间
* 形    参 : weekday:星期几(1~7) hour,min,sec:小时,分钟,秒钟
* 返 回 值 : 无
* 备    注 : 1.先关闭闹钟
			 2.再设置时间
			 3.中断配置（配置中断 1.闹钟中断开关使能 ；2.EXTI 开关使能；3.NVIC 开关使能）
*********************************************************************************************************/ 
void RTC_SetAlarmB(u8 weekday,u8 hour,u8 min,u8 sec)
{
	RTC_AlarmTypeDef RTC_AlarmStruct = {0};
	EXTI_InitTypeDef EXTI_InitStruct = {0};
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	
	RTC_AlarmCmd(RTC_Alarm_B, DISABLE);	//关闭闹钟A
	
	RTC_TimeStruct.RTC_H12 =RTC_H12_AM;
  RTC_TimeStruct.RTC_Hours = hour;
  RTC_TimeStruct.RTC_Minutes = min;
  RTC_TimeStruct.RTC_Seconds = sec;

	
	RTC_AlarmStruct.RTC_AlarmDateWeekDay=weekday;//星期几生效
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_WeekDay;//星期生效
	RTC_AlarmStruct.RTC_AlarmMask=RTC_AlarmMask_None; //全部字段有效
	RTC_AlarmStruct.RTC_AlarmTime=RTC_TimeStruct; //闹钟时间
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_B, &RTC_AlarmStruct);

	
	EXTI_InitStruct.EXTI_Line = EXTI_Line17;
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel 	= RTC_Alarm_IRQn;	//中断源
	NVIC_InitStruct.NVIC_IRQChannelCmd  = ENABLE;			//使能中断源
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority 		  = 1;	//响应优先级2
	NVIC_Init(&NVIC_InitStruct);	
	
	RTC_ITConfig(RTC_IT_ALRB, ENABLE);		//使能闹钟B中断
	RTC_AlarmCmd(RTC_Alarm_B, ENABLE);		//开启闹钟B
}
/*********************************************************************************************************
* 函 数 名 : RTC_Alarm_IRQHandler
* 功能说明 : 闹钟中断服务函数
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 需要分别判断触发、清零
*********************************************************************************************************/ 
void RTC_Alarm_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_ALRA))
	{
		printf("闹钟A\r\n");
		while(1)
		{
				LED1=~LED1;
				Delay_ms(100);
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
		{
			Delay_ms(100);
			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
			{
				while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0));
				LED1=1;
				printf("按键1按下\r\n");
				break;
			}	
		}
	}
		RTC_ClearITPendingBit(RTC_IT_ALRA);		//清除闹钟A中断标志
		EXTI_ClearITPendingBit(EXTI_Line17);	//清除线17标志
	}
	if(RTC_GetITStatus(RTC_IT_ALRB))
	{
		printf("闹钟B\r\n");
		while(1)
		{
				LED2=~LED2;
				Delay_ms(100);
		if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13))
		{
			Delay_ms(100);
			if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13))
			{
				while(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13));
				LED2=1;
				printf("按键2按下\r\n");
				break;
			}	
		}
			RTC_ClearITPendingBit(RTC_IT_ALRB);		//清除闹钟B中断标志
			EXTI_ClearITPendingBit(EXTI_Line17);	//清除线17标志
		}
	}
}
