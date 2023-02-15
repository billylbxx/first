#ifndef _RTC_H_
#define _RTC_H_

#include "io_bit.h" 

extern RTC_DateTypeDef RTC_DateStruct;
extern RTC_TimeTypeDef RTC_TimeStruct;
void Rtc_Init(void);
void RTC_GetTimeDate(void);
void RTC_SetAlarmA(unsigned char week,unsigned char hour,unsigned char min,unsigned char sec);
void RTC_SetAlarmB(unsigned char week,unsigned char hour,unsigned char min,unsigned char sec);
void RTC_Set_WakeUp(u32 wksel,u16 cnt);
void RTC_SetTimeDate(unsigned char year,unsigned char month,unsigned char day,
	 unsigned char week,unsigned char hour,unsigned char min,unsigned char sec);

#endif

