#include <stdio.h>
#include "io_bit.h"
#include "uart1.h"
#include "systick.h"
#include "usart2.h"
#include "esp8266.h"
#include "led.h"	
#include "motor.h"
#include "tim3.h"//PWM
#include "adc.h"
#include "RGB.h"
#include "IIC.h"
#include "SPI.h"
#include "SHT31.h"
#include "W25QXX.h"
#include <math.h>
int len=0;
int X=0;
char  a[100]={0};
int sum[50]={0};
void qza(int a[]);
char shuju[100]={0};
char shuju2[100]={0};
void sl(int sum);
int main(){
  int state = 0,i=0,j=0;
	Systick_Inti(168);
	Led_Init();
	Usart2_Init(115200);
	uart1_init(115200);
	//ESP12_STA_Init();
	Motor_Init();
	tim3_Init(500,840);
	Adc1_Init();
	RGB_Init();
	SHT31_Init();
	spi1_Init();
	spi3_Init();
	lcd_Init();
	w25q64_Init();
//	printf("dddsd");
//	RGB_TO_GRB(0,RGB_RED);
//	RGB_ENABLE();
	
	while(1){
		SHT31_read_T_H();
		printf("温度：%.3f,湿度：%.3f\r\n",temp.t,temp.h);
		sprintf(shuju,"温度：%.3f",temp.t);
		sprintf(shuju2,"湿度：%.3f",temp.h);
		LCD_Fill(0,0,240,240,LCD_WHITE);
		LCD_FLASH_SHOW_CHINESE_STR(0,0,shuju,LCD_RED,LCD_WHITE,32);	
		LCD_FLASH_SHOW_CHINESE_STR(0,32,shuju2,LCD_BLUE,LCD_WHITE,32);		
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
//		if(i>5000){//温度
//				len ++;
//				i=0;
//			  X=Get_value(ADC_Channel_10);
//				publish_mqtt_message[0].value[0]=len;
//				publish_mqtt_message[1].value[1]=X;
//			Publish_MQTT_message(publish_mqtt_message,4); //发布主题
//			}			
//delay_ms(1);
//i++;			
		
////if(j>2000){
//			//j=0;
////		publish_mqtt_message->value[1]=(unsigned short)filter(12,ADC_Channel_10);
////		Publish_MQTT_message(publish_mqtt_message,4);
////}			
////delay_ms(1);
////j++;			
//		
//			
//		if(Usart2.RecFlag){//风扇，LED
//				Usart2.RecFlag = 0; //清标志	
//				SendUsart1Package(Usart2.RxBuff,Usart2.FlagLen&0x7fff);//回显串口助手
//				Usart2.FlagLen = 0;

//			if(strstr((char*)Usart2.RxBuff, "LED2ON"))
//			{			
//					LED2= 0;
//					printf("已为你打开灯\r\n");
//			
//			}		
//			else if(strstr((char*)Usart2.RxBuff, "LED2OFF"))
//			{			
//					LED2= 1;
//					printf("已为你关闭灯\r\n");
//			}
//			
//			else if(strstr((char*)Usart2.RxBuff, "\"Speed\":1"))
//			{
//				MOTOR_PWM(100);
//				printf("风扇1档\r\n");
//			}
//			
//			else if(strstr((char*)Usart2.RxBuff, "\"Speed\":2"))
//			{
//				MOTOR_PWM(300);
//				printf("风扇2档\r\n");
//			}
//			else if(strstr((char*)Usart2.RxBuff, "\"Speed\":3"))
//			{
//				MOTOR_PWM(500);
//				printf("风扇3档\r\n");
//			}
//			
			

			
			
//				JsonString_Parsing((char * )Usart2.RxBuff,&json); //解析阿里云下发数据
//				printf("key:%s val:%s\r\n",json.key,json.values); //输出解析数据
//		if(strstr(json.key, "LEDSwitch"))
//			{
//				state = atoi(json.values)? 0 : 1;   
//				if(state==0)
//				{
//					LED1= 0;
//					printf("已为你打开灯");
//				}
//				else
//				{
//					LED1= 1;
//					printf("已为你打关灯");
//				}	
//			}							
//			memset(Usart2.RxBuff,0,sizeof(Usart2.RxBuff));//清数组		
//		 }
//	 }
//	return 0;
}
	}

//void qza(int a[])
//{
//	RGB_TO_GRB(0,RGB_black);
//	u32 str;
//	int i=0,j=0,k=0,num=0,num2=0,num3=0,flag=0;
//	if(Usart2.RecFlag){//风扇，LED
//				Usart2.RecFlag = 0; //清标志
//SendUsart1Package(Usart2.RxBuff,Usart2.FlagLen&0x7fff);//回显串口助手		
//	char*sp=strstr((char*)Usart2.RxBuff,"Red");
//	char*p=sp;
//	while(*(sp+5+i)!=',')
//	{
//		a[i]=*(sp+5+i);
//		printf("%d\r\n",a[i]-48);
//		i++;
//		flag++;
//	}
//	for(int q=0;q<i;q++)
//	{
//		flag--;
//		num+=(a[flag]-48)*pow(10,q);
//		
//	}
//	printf("num=%d\r\n",num);
//	sp=strstr((char*)Usart2.RxBuff,"Blue");
//	while(*(sp+6+j)!=',')
//	{
//		flag=i;
//		a[i]=*(sp+6+j);
//		printf("%d\r\n",a[i]-48);
//		i++;
//		j++;
//		flag++;		
//	}
//	for(int s=0;s<j;s++)
//	{
//		flag--;
//		num2+=(a[flag]-48)*pow(10,s);	
//	}
//	printf("num=%d\r\n",num2);
//	sp=strstr((char*)Usart2.RxBuff,"Green");
//	while(*(sp+7+k)!=125)
//	{
//		flag=i;
//		a[i]=*(sp+7+k);
//		printf("%d\r\n",a[i]-48);
//		i++;
//		k++;
//		flag++;
//	}
//	for(int d=0;d<k;d++)
//	{	
//		flag--;
//		num3+=(a[flag]-48)*pow(10,d);
//	}
//	printf("num=%d\r\n",num3);
//	sprintf((char*)str,"%x%x%x",num,num2,num3);
//	RGB_TO_GRB(0,str);
//	RGB_ENABLE();
//}
//	
//}





