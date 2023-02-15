#ifndef __RTC_H
#define __RTC_H
#include "io_bit.h" 

extern int RTC_flag;

u8 rtc_Init(void);
void RTC_Set_WakeUp(void);
void RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm);
void RTC_Set_Date(u8 weekday,u16 year,u8 month,u8 date);
void RTC_SetAlarmA(u8 weekday,u8 hour,u8 min,u8 sec);
void RTC_SetAlarmB(u8 weekday,u8 hour,u8 min,u8 sec);
extern u16 RTC_BKP;
#endif
