#include "rtc.h"
#include "usart1.h"
#include "lcd.h"
#include "systick.h"

#define RTCBKP 0X1000			/*如需修改时间，修改该值即可*/
RTC_DateTypeDef RTC_DateStruct;
RTC_TimeTypeDef RTC_TimeStruct;
u8 buf[50];
u8 buf1[50];
/*********************************************************************************************************
* 函 数 名 : Rtc_Init
* 功能说明 : 实时时钟初始化
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 按默认分频系数分频后，计数器频率1HZ
*********************************************************************************************************/ 
void Rtc_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);		//开启电源时钟
	PWR_BackupAccessCmd(ENABLE);							//使能备份寄存器的访问
	if(RTC_ReadBackupRegister(RTC_BKP_DR19) != RTCBKP)
	{
		RCC_LSEConfig(RCC_LSE_ON);							//开启LSE时钟
	
		while(!RCC_GetFlagStatus(RCC_FLAG_LSERDY));			//等待LSE稳定
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);				//选择LSE作为RTC时钟源
		RCC_RTCCLKCmd(ENABLE);								//使能RTC时钟
		 
		RTC_SetTimeDate(21, 1, 11, 1, 11, 5, 40);			//年月日周时分秒
		RTC_SetAlarmA(1,11,5,50);							//周时分秒
		RTC_SetAlarmB(1,11,5,55);
		RTC_WriteBackupRegister(RTC_BKP_DR19,RTCBKP);		//写入后备寄存器
	}
}
/*********************************************************************************************************
* 函 数 名 : RTC_SetTimeDate
* 功能说明 : 设置实时时钟初始参数
* 形    参 : 日期、时间
* 返 回 值 : 无
* 备    注 : 示例：RTC_SetTimeDate(21,1,11,1,11,5,0);
*********************************************************************************************************/ 
void RTC_SetTimeDate(unsigned char year,unsigned char month,unsigned char day,
	 unsigned char week,unsigned char hour,unsigned char min,unsigned char sec)
{	
	RTC_DateStruct.RTC_Year 	= year;
	RTC_DateStruct.RTC_Month 	= month;
	RTC_DateStruct.RTC_Date 	= day;
	RTC_DateStruct.RTC_WeekDay 	= week;
	RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);
	
	RTC_TimeStruct.RTC_H12 		= RTC_H12_AM;
	RTC_TimeStruct.RTC_Hours 	= hour;
	RTC_TimeStruct.RTC_Minutes 	= min;
	RTC_TimeStruct.RTC_Seconds 	= sec;
	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
}
/*********************************************************************************************************
* 函 数 名 : RTC_GetTimeDate
* 功能说明 : 获取时间和日期
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void RTC_GetTimeDate(void)
{
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
}
/*********************************************************************************************************
* 函 数 名 : RTC_SetAlarmA
* 功能说明 : 设置闹钟A时间
* 形    参 : week:星期几(1~7) hour,min,sec:小时,分钟,秒钟
* 返 回 值 : 无
* 备    注 : 1.先关闭闹钟
			 2.再设置时间
			 3.中断配置（配置中断 1.闹钟中断开关使能 ；2.EXTI 开关使能；3.NVIC 开关使能）
*********************************************************************************************************/ 
void RTC_SetAlarmA(unsigned char week, unsigned char hour, unsigned char min, unsigned char sec)
{
	RTC_AlarmTypeDef RTC_AlarmStruct = {0};
	EXTI_InitTypeDef EXTI_InitStruct = {0};
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);	//关闭闹钟A
	
	RTC_TimeStruct.RTC_H12 		= RTC_H12_AM;
	RTC_TimeStruct.RTC_Hours 	= hour;
	RTC_TimeStruct.RTC_Minutes 	= min;
	RTC_TimeStruct.RTC_Seconds 	= sec;
	
	RTC_AlarmStruct.RTC_AlarmDateWeekDay 	= week;		//星期几
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;	//星期触发
	RTC_AlarmStruct.RTC_AlarmMask = RTC_AlarmMask_None;	//完全匹配
	RTC_AlarmStruct.RTC_AlarmTime = RTC_TimeStruct;		//要设置的闹钟时间
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStruct);
	
	EXTI_InitStruct.EXTI_Line 	 = EXTI_Line17;			//中断线
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;				//中断线使能
	EXTI_InitStruct.EXTI_Mode 	 = EXTI_Mode_Interrupt;	//中断模式
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;	//上升沿触发	
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
* 函 数 名 : RTC_SetAlarmB
* 功能说明 : 设置闹钟B时间
* 形    参 : week:星期几(1~7) hour,min,sec:小时,分钟,秒钟
* 返 回 值 : 无
* 备    注 : 1.先关闭闹钟
			 2.再设置时间
			 3.中断配置（配置中断 1.闹钟中断开关使能 ；2.EXTI 开关使能；3.NVIC 开关使能）
*********************************************************************************************************/ 
void RTC_SetAlarmB(unsigned char week, unsigned char hour, unsigned char min, unsigned char sec)
{
	RTC_AlarmTypeDef RTC_AlarmStruct = {0};
	EXTI_InitTypeDef EXTI_InitStruct = {0};
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	
	RTC_AlarmCmd(RTC_Alarm_B, DISABLE);		//关闭闹钟B
	
	RTC_TimeStruct.RTC_H12 		= RTC_H12_AM;
	RTC_TimeStruct.RTC_Hours 	= hour;
	RTC_TimeStruct.RTC_Minutes 	= min;
	RTC_TimeStruct.RTC_Seconds 	= sec;
	
	RTC_AlarmStruct.RTC_AlarmDateWeekDay 	= week;		//星期几
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;	//星期触发
	RTC_AlarmStruct.RTC_AlarmMask = RTC_AlarmMask_None;	//完全匹配
	RTC_AlarmStruct.RTC_AlarmTime = RTC_TimeStruct;		//要设置的闹钟时间
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_B, &RTC_AlarmStruct);
	
	EXTI_InitStruct.EXTI_Line 		= EXTI_Line17;			//中断线
	EXTI_InitStruct.EXTI_LineCmd 	= ENABLE;				//中断线使能
	EXTI_InitStruct.EXTI_Mode 		= EXTI_Mode_Interrupt;	//中断模式
	EXTI_InitStruct.EXTI_Trigger 	= EXTI_Trigger_Rising;	//上升沿触发	
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel 	= RTC_Alarm_IRQn;	//中断源
	NVIC_InitStruct.NVIC_IRQChannelCmd  = ENABLE;			//使能中断源
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority 		  = 1;	//响应优先级2
	NVIC_Init(&NVIC_InitStruct);	
	
	RTC_ITConfig(RTC_IT_ALRB, ENABLE);		//使能闹钟A中断
	RTC_AlarmCmd(RTC_Alarm_B, ENABLE);		//开启闹钟A
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
		RTC_ClearITPendingBit(RTC_IT_ALRA);		//清除闹钟A中断标志
		EXTI_ClearITPendingBit(EXTI_Line17);	//清除线17标志
	}
	if(RTC_GetITStatus(RTC_IT_ALRB))
	{
		printf("闹钟B\r\n");
		RTC_ClearITPendingBit(RTC_IT_ALRB);		//清除闹钟B中断标志
		EXTI_ClearITPendingBit(EXTI_Line17);	//清除线17标志
	}
}
/*********************************************************************************************************
* 函 数 名 : RTC_Set_WakeUp
* 功能说明 : 设置周期唤醒
* 形    参 : wksel:  @ref RTC_Wakeup_Timer_Definitions
			 #define RTC_WakeUpClock_RTCCLK_Div16        ((uint32_t)0x00000000)
			 #define RTC_WakeUpClock_RTCCLK_Div8         ((uint32_t)0x00000001)
			 #define RTC_WakeUpClock_RTCCLK_Div4         ((uint32_t)0x00000002)
			 #define RTC_WakeUpClock_RTCCLK_Div2         ((uint32_t)0x00000003)
			 #define RTC_WakeUpClock_CK_SPRE_16bits      ((uint32_t)0x00000004)
			 #define RTC_WakeUpClock_CK_SPRE_17bits      ((uint32_t)0x00000006)
			 cnt：自动重装载值，减到0产生中断
* 返 回 值 : 无
* 备    注 : 示例：RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);	//1秒唤醒一次
*********************************************************************************************************/ 
void RTC_Set_WakeUp(unsigned int wksel, unsigned short cnt)
{ 
	EXTI_InitTypeDef   EXTI_InitStructure = {0};
	NVIC_InitTypeDef   NVIC_InitStructure = {0};
	
	RTC_WakeUpCmd(DISABLE);				//关闭WAKE UP
	RTC_WakeUpClockConfig(wksel);		//唤醒时钟选择
	RTC_SetWakeUpCounter(cnt);			//设置WAKE UP自动重装载寄存器
	
	RTC_ClearITPendingBit(RTC_IT_WUT); 	//清除RTC WAKE UP的标志
	EXTI_ClearITPendingBit(EXTI_Line22);//清除LINE22上的中断标志位 
	 
	RTC_ITConfig(RTC_IT_WUT,ENABLE);	//开启WAKE UP 定时器中断
	RTC_WakeUpCmd(ENABLE);				//开启WAKE UP 定时器　

	EXTI_InitStructure.EXTI_Line 	= EXTI_Line22;				//LINE22
	EXTI_InitStructure.EXTI_Mode 	= EXTI_Mode_Interrupt;		//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 		//上升沿触发 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;					//使能LINE22
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn; 		//中断源
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);
}
/*********************************************************************************************************
* 函 数 名 : RTC_WKUP_IRQHandler
* 功能说明 : 周期唤醒中断服务函数
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void RTC_WKUP_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_WUT))
	{
		RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
		sprintf((char*)buf,"Time:%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
		printf("%s\r\n",buf);
		
		RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
		sprintf((char*)buf1,"DATA:%d:%d:%02d:%02d",RTC_DateStruct.RTC_WeekDay, RTC_DateStruct.RTC_Year, RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date );
		printf("%s\r\n",buf1);
		RTC_ClearITPendingBit(RTC_IT_WUT);
	}
}
