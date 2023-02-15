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
#include <string.h>
//void delay(int m);
void fff(USART_TypeDef* USARTx, char*string);
void tt(char*sp);
void qaz(char*str);
u32 addr=0;
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
//exti_Init();
ADC1_Init();
lcd_Init();
//	LCD_showstring(0,0,"ddwdwd",LCD_LIGHTBLUE,LCD_RED,32);
//Delay_ms(500);
w25q64_Init();
LCD_Fill(0, 0, 240, 240, LCD_WHITE);
printf("ID:%#x\r\n",W25Q64_readID());
char buf[100]="肖申克阿银";
//LCD_FLASH_SHOW_CHINESE_STR(200,150,buf,LCD_RED,LCD_WHITE,32);
//w25q64_clean_all();
//W25Q64_Write_NoCheck("123雕刻殴打",256,11);
//printf("%s\r\n",W25QXX_read_date(256,11));
while(1)
{
//	u8 dat=Usart_Rec_data();
//	W25Q64_Write_NoCheck(&dat,addr,1);
//	addr++;
	GPIO_ToggleBits(GPIOC, GPIO_Pin_4);     //LED1闪烁
				char str[50]={0};
				static	int j=0;
				j++;
				sprintf(str,"LED1闪烁次数:%d",j);
				LCD_FLASH_SHOW_CHINESE_STR(0,156,str,LCD_WHITE,LCD_BLACK,32);	
				char oi[50]={0};
				static	int i=0;
				i++;
				sprintf(oi,"LED2闪烁次数:%d",i);
				LCD_FLASH_SHOW_CHINESE_STR(0,60,oi,LCD_WHITE,LCD_BLUE,16);
				Delay_ms(200);
				GPIO_ToggleBits(GPIOC, GPIO_Pin_5);     //LED2闪烁
				char oy[50]={0};
				static	int k=0;
				k++;
				sprintf(oy,"LED3闪烁次数%d",i);
				LCD_FLASH_SHOW_CHINESE_STR(0,32,oy,LCD_WHITE,LCD_BLUE,16);
}
//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//同一组优先级别中，不同的抢占级别之间，其中一抢占级别正在做事，另外抢占级别不能打断他
//LCD_Fill(0, 0, 240, 240, LCD_RED); 

}



