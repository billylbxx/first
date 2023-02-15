#include "rtc.h"
#include "usart1.h"
#include "lcd.h"
#include "systick.h"

#define RTCBKP 0X1000			/*�����޸�ʱ�䣬�޸ĸ�ֵ����*/
RTC_DateTypeDef RTC_DateStruct;
RTC_TimeTypeDef RTC_TimeStruct;
u8 buf[50];
u8 buf1[50];
/*********************************************************************************************************
* �� �� �� : Rtc_Init
* ����˵�� : ʵʱʱ�ӳ�ʼ��
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ��Ĭ�Ϸ�Ƶϵ����Ƶ�󣬼�����Ƶ��1HZ
*********************************************************************************************************/ 
void Rtc_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);		//������Դʱ��
	PWR_BackupAccessCmd(ENABLE);							//ʹ�ܱ��ݼĴ����ķ���
	if(RTC_ReadBackupRegister(RTC_BKP_DR19) != RTCBKP)
	{
		RCC_LSEConfig(RCC_LSE_ON);							//����LSEʱ��
	
		while(!RCC_GetFlagStatus(RCC_FLAG_LSERDY));			//�ȴ�LSE�ȶ�
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);				//ѡ��LSE��ΪRTCʱ��Դ
		RCC_RTCCLKCmd(ENABLE);								//ʹ��RTCʱ��
		 
		RTC_SetTimeDate(21, 1, 11, 1, 11, 5, 40);			//��������ʱ����
		RTC_SetAlarmA(1,11,5,50);							//��ʱ����
		RTC_SetAlarmB(1,11,5,55);
		RTC_WriteBackupRegister(RTC_BKP_DR19,RTCBKP);		//д��󱸼Ĵ���
	}
}
/*********************************************************************************************************
* �� �� �� : RTC_SetTimeDate
* ����˵�� : ����ʵʱʱ�ӳ�ʼ����
* ��    �� : ���ڡ�ʱ��
* �� �� ֵ : ��
* ��    ע : ʾ����RTC_SetTimeDate(21,1,11,1,11,5,0);
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
* �� �� �� : RTC_GetTimeDate
* ����˵�� : ��ȡʱ�������
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void RTC_GetTimeDate(void)
{
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
}
/*********************************************************************************************************
* �� �� �� : RTC_SetAlarmA
* ����˵�� : ��������Aʱ��
* ��    �� : week:���ڼ�(1~7) hour,min,sec:Сʱ,����,����
* �� �� ֵ : ��
* ��    ע : 1.�ȹر�����
			 2.������ʱ��
			 3.�ж����ã������ж� 1.�����жϿ���ʹ�� ��2.EXTI ����ʹ�ܣ�3.NVIC ����ʹ�ܣ�
*********************************************************************************************************/ 
void RTC_SetAlarmA(unsigned char week, unsigned char hour, unsigned char min, unsigned char sec)
{
	RTC_AlarmTypeDef RTC_AlarmStruct = {0};
	EXTI_InitTypeDef EXTI_InitStruct = {0};
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);	//�ر�����A
	
	RTC_TimeStruct.RTC_H12 		= RTC_H12_AM;
	RTC_TimeStruct.RTC_Hours 	= hour;
	RTC_TimeStruct.RTC_Minutes 	= min;
	RTC_TimeStruct.RTC_Seconds 	= sec;
	
	RTC_AlarmStruct.RTC_AlarmDateWeekDay 	= week;		//���ڼ�
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;	//���ڴ���
	RTC_AlarmStruct.RTC_AlarmMask = RTC_AlarmMask_None;	//��ȫƥ��
	RTC_AlarmStruct.RTC_AlarmTime = RTC_TimeStruct;		//Ҫ���õ�����ʱ��
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStruct);
	
	EXTI_InitStruct.EXTI_Line 	 = EXTI_Line17;			//�ж���
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;				//�ж���ʹ��
	EXTI_InitStruct.EXTI_Mode 	 = EXTI_Mode_Interrupt;	//�ж�ģʽ
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;	//�����ش���	
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel 	= RTC_Alarm_IRQn;	//�ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelCmd  = ENABLE;			//ʹ���ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority 		  = 1;	//��Ӧ���ȼ�2
	NVIC_Init(&NVIC_InitStruct);	
	
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);		//ʹ������A�ж�
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);		//��������A
}
/*********************************************************************************************************
* �� �� �� : RTC_SetAlarmB
* ����˵�� : ��������Bʱ��
* ��    �� : week:���ڼ�(1~7) hour,min,sec:Сʱ,����,����
* �� �� ֵ : ��
* ��    ע : 1.�ȹر�����
			 2.������ʱ��
			 3.�ж����ã������ж� 1.�����жϿ���ʹ�� ��2.EXTI ����ʹ�ܣ�3.NVIC ����ʹ�ܣ�
*********************************************************************************************************/ 
void RTC_SetAlarmB(unsigned char week, unsigned char hour, unsigned char min, unsigned char sec)
{
	RTC_AlarmTypeDef RTC_AlarmStruct = {0};
	EXTI_InitTypeDef EXTI_InitStruct = {0};
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	
	RTC_AlarmCmd(RTC_Alarm_B, DISABLE);		//�ر�����B
	
	RTC_TimeStruct.RTC_H12 		= RTC_H12_AM;
	RTC_TimeStruct.RTC_Hours 	= hour;
	RTC_TimeStruct.RTC_Minutes 	= min;
	RTC_TimeStruct.RTC_Seconds 	= sec;
	
	RTC_AlarmStruct.RTC_AlarmDateWeekDay 	= week;		//���ڼ�
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;	//���ڴ���
	RTC_AlarmStruct.RTC_AlarmMask = RTC_AlarmMask_None;	//��ȫƥ��
	RTC_AlarmStruct.RTC_AlarmTime = RTC_TimeStruct;		//Ҫ���õ�����ʱ��
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_B, &RTC_AlarmStruct);
	
	EXTI_InitStruct.EXTI_Line 		= EXTI_Line17;			//�ж���
	EXTI_InitStruct.EXTI_LineCmd 	= ENABLE;				//�ж���ʹ��
	EXTI_InitStruct.EXTI_Mode 		= EXTI_Mode_Interrupt;	//�ж�ģʽ
	EXTI_InitStruct.EXTI_Trigger 	= EXTI_Trigger_Rising;	//�����ش���	
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel 	= RTC_Alarm_IRQn;	//�ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelCmd  = ENABLE;			//ʹ���ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority 		  = 1;	//��Ӧ���ȼ�2
	NVIC_Init(&NVIC_InitStruct);	
	
	RTC_ITConfig(RTC_IT_ALRB, ENABLE);		//ʹ������A�ж�
	RTC_AlarmCmd(RTC_Alarm_B, ENABLE);		//��������A
}
/*********************************************************************************************************
* �� �� �� : RTC_Alarm_IRQHandler
* ����˵�� : �����жϷ�����
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ��Ҫ�ֱ��жϴ���������
*********************************************************************************************************/ 
void RTC_Alarm_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_ALRA))
	{
		printf("����A\r\n");
		RTC_ClearITPendingBit(RTC_IT_ALRA);		//�������A�жϱ�־
		EXTI_ClearITPendingBit(EXTI_Line17);	//�����17��־
	}
	if(RTC_GetITStatus(RTC_IT_ALRB))
	{
		printf("����B\r\n");
		RTC_ClearITPendingBit(RTC_IT_ALRB);		//�������B�жϱ�־
		EXTI_ClearITPendingBit(EXTI_Line17);	//�����17��־
	}
}
/*********************************************************************************************************
* �� �� �� : RTC_Set_WakeUp
* ����˵�� : �������ڻ���
* ��    �� : wksel:  @ref RTC_Wakeup_Timer_Definitions
			 #define RTC_WakeUpClock_RTCCLK_Div16        ((uint32_t)0x00000000)
			 #define RTC_WakeUpClock_RTCCLK_Div8         ((uint32_t)0x00000001)
			 #define RTC_WakeUpClock_RTCCLK_Div4         ((uint32_t)0x00000002)
			 #define RTC_WakeUpClock_RTCCLK_Div2         ((uint32_t)0x00000003)
			 #define RTC_WakeUpClock_CK_SPRE_16bits      ((uint32_t)0x00000004)
			 #define RTC_WakeUpClock_CK_SPRE_17bits      ((uint32_t)0x00000006)
			 cnt���Զ���װ��ֵ������0�����ж�
* �� �� ֵ : ��
* ��    ע : ʾ����RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);	//1�뻽��һ��
*********************************************************************************************************/ 
void RTC_Set_WakeUp(unsigned int wksel, unsigned short cnt)
{ 
	EXTI_InitTypeDef   EXTI_InitStructure = {0};
	NVIC_InitTypeDef   NVIC_InitStructure = {0};
	
	RTC_WakeUpCmd(DISABLE);				//�ر�WAKE UP
	RTC_WakeUpClockConfig(wksel);		//����ʱ��ѡ��
	RTC_SetWakeUpCounter(cnt);			//����WAKE UP�Զ���װ�ؼĴ���
	
	RTC_ClearITPendingBit(RTC_IT_WUT); 	//���RTC WAKE UP�ı�־
	EXTI_ClearITPendingBit(EXTI_Line22);//���LINE22�ϵ��жϱ�־λ 
	 
	RTC_ITConfig(RTC_IT_WUT,ENABLE);	//����WAKE UP ��ʱ���ж�
	RTC_WakeUpCmd(ENABLE);				//����WAKE UP ��ʱ����

	EXTI_InitStructure.EXTI_Line 	= EXTI_Line22;				//LINE22
	EXTI_InitStructure.EXTI_Mode 	= EXTI_Mode_Interrupt;		//�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 		//�����ش��� 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;					//ʹ��LINE22
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn; 		//�ж�Դ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);
}
/*********************************************************************************************************
* �� �� �� : RTC_WKUP_IRQHandler
* ����˵�� : ���ڻ����жϷ�����
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ��
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
