#include "stm32f4xx.h"
#include "led.h"               
#include "Beep.h"
#include "key.h"
#include "uart.h"
#include "motor.h"
#include "exti.h"
#include "systick.h"
#include "tim.h"
#include "tim7.h"
#include "tim3.h"
#include "ADC.h"
#include "SPI.h"
#include "W25QXX.h"
#include "rtc.h"
#include <string.h>
#if 1
u8 buf[50];
u8 buf1[50];
u8 buf2[50];
u8 buf3[50];
#endif
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;
int main()
{
	
	
//const unsigned char* tp[6]={gImage_IMG00000,gImage_IMG00001,gImage_IMG00006,gImage_IMG00012,gImage_IMG00013,gImage_IMG00019};
	
		Led_Init();
		Beep_Init();
		key_Init();
		Uart_Init();
		Motor_Init();
		Systick_Init(168);
		//tim6_Init(50000,84);
		//tim7_Init(50000,84);
		//tim3_Init(500,840);
		//tim2_Init(500,840);
		exti_Init();
		ADC1_Init();
		lcd_Init();
		rtc_Init();
		RTC_Set_WakeUp();;
		w25q64_Init();
		LCD_Fill(0, 0, 240, 240, LCD_WHITE);
		
	while(1)
	{
		
		if(RTC_flag==1)
		{
			RTC_flag=0;
			RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
			sprintf((char*)buf,"时间:%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
			printf("%s\r\n",buf);
			LCD_FLASH_SHOW_CHINESE_STR(0,128,(char*)buf,LCD_WHITE,LCD_BLACK,32);
			
			RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
			sprintf((char*)buf1,"DATA:%d:%d:%02d:%02d",RTC_DateStruct.RTC_WeekDay, RTC_DateStruct.RTC_Year, RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date );
			sprintf((char*)buf2,"%d:%02d:%02d",RTC_DateStruct.RTC_Year, RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date );
			sprintf((char*)buf3,"%d",RTC_DateStruct.RTC_WeekDay);
			printf("%s\r\n",buf1);
			
			LCD_FLASH_SHOW_CHINESE_STR(0,64,"星期:",LCD_BLACK,LCD_WHITE,32);
			LCD_FLASH_SHOW_CHINESE_STR(80,64,(char*)buf3,LCD_BLACK,LCD_WHITE,32);
			LCD_FLASH_SHOW_CHINESE_STR(0,96,"日期:",LCD_BLACK,LCD_WHITE,32);
			LCD_FLASH_SHOW_CHINESE_STR(80,96,(char*)buf2,LCD_BLACK,LCD_WHITE,32);
			LED3=~LED3;
		}
	}
}



