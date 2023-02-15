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


u8 rtc_init(void);						//RTC��ʼ��
u8 waitint_rtc_synchro(void);				//�ȴ�ͬ��
u8 rtc_init_mode(void);					//�����ʼ��ģʽ
u8 dec_to_bcd(u8 val);					//ʮ����ת��ΪBCD��
u8 bcd_to_dec(u8 val);					//BCD��ת��Ϊʮ��������
u8 set_rtc_time(u8 year,u8 month,u8 date,u8 week,u8 hour,u8 min,u8 sec,u8 ampm);//ʱ������
void get_rtc_time(RTC_TIME *p);	//��ȡRTCʱ��
void set_rtc_wakeup_time(u8 wksel,u16 cnt);								//���û���ʱ��
void set_rtc_alarm_time(u8 week,u8 hour,u8 min,u8 sec);   // ��������ʱ��
#endif



