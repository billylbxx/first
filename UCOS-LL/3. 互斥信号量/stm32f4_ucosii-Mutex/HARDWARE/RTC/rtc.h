#ifndef __RTC_H
#define __RTC_H	 

#include "stm32f4xx.h" 

typedef  struct {
	u8 hour;
	u8 min;
	u8 sec;
	u8 ampm;
	u8 year;
	u8 month;
	u8 date;
	u8 week;
} RTC_TIME;


extern RTC_TIME time_date;


u8 rtc_init(void);						//RTC初始化
u8 waitint_rtc_synchro(void);				//等待同步
u8 rtc_init_mode(void);					//进入初始化模式
u8 dec_to_bcd(u8 val);					//十进制转换为BCD码
u8 bcd_to_dec(u8 val);					//BCD码转换为十进制数据
u8 set_rtc_time(u8 year,u8 month,u8 date,u8 week,u8 hour,u8 min,u8 sec,u8 ampm);//时间设置
void get_rtc_time(RTC_TIME *p);	//获取RTC时间
void set_rtc_wakeup_time(u8 wksel,u16 cnt);								//设置唤醒时间
void set_rtc_alarm_time(u8 week,u8 hour,u8 min,u8 sec);   // 设置闹钟时间
#endif



