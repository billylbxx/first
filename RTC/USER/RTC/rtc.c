#include "stm32f4xx.h"
#include "key.h"
#include "led.h"
#include "uart.h"
#include "exti.h"
#include "systick.h"
#include "stdio.h"
#include "rtc.h"
/*һ��RTC��������һ�㲽��
1��ʹ��PWRʱ�ӣ�RCC_APB1PeriphClockCmd();
2��ʹ�ܺ󱸼Ĵ������ʣ�PWR_BackupAccessCmd();
3������RTCʱ��Դ��ʹ��RTCʱ�ӣ�RCC_RTCCLKConfig();  RCC_RTCCLKCmd();���Ҫʹ��LSE��Ҫ��LSE��RCC_LSEConfig(RCC_LSE_ON);
4����ʼ��RTC��ͬ��/�첽��ϵ����ʱ�Ӹ�ʽ����RTC_Init();
5������ʱ�䣺RTC_SetTime();
6���������ڣ�RTC_SetDate();*/

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
//	RTC_DateStructInit(&RTC_DateStruct);//���ܼ���䣬���˾����ò���ʱ��
 	RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);
	
	
}

u8 rtc_Init(void)
{
	u16 ret=0x1FFF;
	u16 RTC_BKP=0x8579;  //���ı����ֵ������������ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	PWR_BackupAccessCmd(ENABLE);//ʹ�ܻ���ʧ�� RTC �ͺ󱸼Ĵ������ʣ�ϵͳ��λ��Ӵ���ģʽ���Ѻ�RTC�����ú�ʱ��ά�ֲ��䡣��
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR19)!=RTC_BKP)
	{
		RCC_LSEConfig(RCC_LSE_ON);
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET)//�ȴ��ⲿ��������׼����	
		{
			ret--;
			Delay_ms(10);
		}
		if(ret==0) return 1;//LSE����ʧ��
		
	
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	RCC_RTCCLKCmd(ENABLE);
		
	RTC_InitTypeDef RTC_InitStruct;
	RTC_InitStruct.RTC_AsynchPrediv=0x7F;//0X7FFF=32767,����32768HZ/PSC+1=32768/32767+1=1HZ,��ʱ1s
	RTC_InitStruct.RTC_SynchPrediv=0XFF;
	RTC_InitStruct.RTC_HourFormat=RTC_HourFormat_24;
	RTC_StructInit(&RTC_InitStruct);
	RTC_Init(&RTC_InitStruct);
  
  RTC_Set_Time(23,59,45,RTC_H12_AM);		
	RTC_Set_Date(3,22,9,14);
	RTC_SetAlarmA(3,23,59,50);
	RTC_SetAlarmB(3,23,59,55);
	RTC_WriteBackupRegister(RTC_BKP_DR19, RTC_BKP);//�ж��Ƿ�Ҫ�������ں�ʱ��
	}
	
	return 0;
}

void RTC_Set_WakeUp(void)
{
	RTC_WakeUpCmd(DISABLE);//�رջ��ѹ���
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);//Ϊ���ѹ���ѡ��RTC���úõ�ʱ��Դ
	RTC_SetWakeUpCounter(0);
	
	RTC_ClearITPendingBit(RTC_IT_WUT);
	EXTI_ClearITPendingBit(EXTI_Line22);
	
	RTC_ITConfig(RTC_IT_WUT, ENABLE);//����RTC�����ж�
	RTC_WakeUpCmd(ENABLE);
  
	
	EXTI_InitTypeDef EXTI_InitStruct={0};
	EXTI_InitStruct.EXTI_Line = EXTI_Line22;//EXTI �� 22 ���ӵ� RTC �����¼�
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=RTC_WKUP_IRQn;//stm32f4xx.h
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;//�����ȼ�
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
* �� �� �� : RTC_SetAlarmA
* ����˵�� : ��������Aʱ��
* ��    �� : weekday:���ڼ�(1~7) hour,min,sec:Сʱ,����,����
* �� �� ֵ : ��
* ��    ע : 1.�ȹر�����
			 2.������ʱ��
			 3.�ж����ã������ж� 1.�����жϿ���ʹ�� ��2.EXTI ����ʹ�ܣ�3.NVIC ����ʹ�ܣ�
*********************************************************************************************************/ 
void RTC_SetAlarmA(u8 weekday,u8 hour,u8 min,u8 sec)
{
	RTC_AlarmTypeDef RTC_AlarmStruct = {0};
	EXTI_InitTypeDef EXTI_InitStruct = {0};
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);	//�ر�����A
	
	RTC_TimeStruct.RTC_H12 =RTC_H12_AM;
  RTC_TimeStruct.RTC_Hours = hour;
  RTC_TimeStruct.RTC_Minutes = min;
  RTC_TimeStruct.RTC_Seconds = sec;

	
	RTC_AlarmStruct.RTC_AlarmDateWeekDay=weekday;//���ڼ���Ч
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_WeekDay;//������Ч
	RTC_AlarmStruct.RTC_AlarmMask=RTC_AlarmMask_None; //ȫ���ֶ���Ч
	RTC_AlarmStruct.RTC_AlarmTime=RTC_TimeStruct; //����ʱ��
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStruct);

	
	EXTI_InitStruct.EXTI_Line = EXTI_Line17;
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
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
* �� �� �� : RTC_SetAlarmA
* ����˵�� : ��������Aʱ��
* ��    �� : weekday:���ڼ�(1~7) hour,min,sec:Сʱ,����,����
* �� �� ֵ : ��
* ��    ע : 1.�ȹر�����
			 2.������ʱ��
			 3.�ж����ã������ж� 1.�����жϿ���ʹ�� ��2.EXTI ����ʹ�ܣ�3.NVIC ����ʹ�ܣ�
*********************************************************************************************************/ 
void RTC_SetAlarmB(u8 weekday,u8 hour,u8 min,u8 sec)
{
	RTC_AlarmTypeDef RTC_AlarmStruct = {0};
	EXTI_InitTypeDef EXTI_InitStruct = {0};
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	
	RTC_AlarmCmd(RTC_Alarm_B, DISABLE);	//�ر�����A
	
	RTC_TimeStruct.RTC_H12 =RTC_H12_AM;
  RTC_TimeStruct.RTC_Hours = hour;
  RTC_TimeStruct.RTC_Minutes = min;
  RTC_TimeStruct.RTC_Seconds = sec;

	
	RTC_AlarmStruct.RTC_AlarmDateWeekDay=weekday;//���ڼ���Ч
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_WeekDay;//������Ч
	RTC_AlarmStruct.RTC_AlarmMask=RTC_AlarmMask_None; //ȫ���ֶ���Ч
	RTC_AlarmStruct.RTC_AlarmTime=RTC_TimeStruct; //����ʱ��
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_B, &RTC_AlarmStruct);

	
	EXTI_InitStruct.EXTI_Line = EXTI_Line17;
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel 	= RTC_Alarm_IRQn;	//�ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelCmd  = ENABLE;			//ʹ���ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority 		  = 1;	//��Ӧ���ȼ�2
	NVIC_Init(&NVIC_InitStruct);	
	
	RTC_ITConfig(RTC_IT_ALRB, ENABLE);		//ʹ������B�ж�
	RTC_AlarmCmd(RTC_Alarm_B, ENABLE);		//��������B
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
				printf("����1����\r\n");
				break;
			}	
		}
	}
		RTC_ClearITPendingBit(RTC_IT_ALRA);		//�������A�жϱ�־
		EXTI_ClearITPendingBit(EXTI_Line17);	//�����17��־
	}
	if(RTC_GetITStatus(RTC_IT_ALRB))
	{
		printf("����B\r\n");
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
				printf("����2����\r\n");
				break;
			}	
		}
			RTC_ClearITPendingBit(RTC_IT_ALRB);		//�������B�жϱ�־
			EXTI_ClearITPendingBit(EXTI_Line17);	//�����17��־
		}
	}
}
