#ifndef __APP_TASK_H
#define __APP_TASK_H

#include "io_bit.h"
#include "systick.h"
#include "led.h"
#include "key.h"
#include "systick.h"
#include "beep.h"
#include "sterilize.h"
#include "usart1.h"
#include "pwm.h"
#include "rgb.h"
#include "sht30.h"
#include "lcd.h"
#include "w25qxx.h"
#include "mlx90614.h"
#include "adc.h"
#include "usart2.h"
#include "usart3.h"
#include "esp12.h"
#include <stdbool.h>
#include <rtc.h>
#include "sd_driver.h"
#include "ff.h"
#include "exfuns.h"     
#include "wm8978.h"	 
#include "audioplay.h"	
#include "mymalloc.h"	
#include "i2s.h"
#include "uart4.h"
#include "ir_learn.h"
#include "wavplay.h"
#include "json.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#include "lvgl.h"
#include "lv_port_disp.h"
#include "lvgl_app.h"

void FreeRTOS_Init(void);

#define GatherData_TASK_PRIO		3	  	//�������ȼ�
#define GatherData_STK_SIZE 		256   //�����ջ��С	
void GatherData_Task(void *pvParameters);

#define VoiceRecognition_TASK_PRIO		4	  		//�������ȼ�
#define VoiceRecognition_STK_SIZE 		1024   	//�����ջ��С	
void VoiceRecognition_Task(void *pvParameters);

#define PlayMusic_TASK_PRIO		1	  		//�������ȼ�
#define PlayMusic_STK_SIZE 		1024   	//�����ջ��С	
void PlayMusic_Task(void *pvParameters);

#define LvglGui_TASK_PRIO		5	  		//�������ȼ�
#define LvglGui_STK_SIZE 		512   	//�����ջ��С	
void LvglGui_Task(void *pvParameters);

extern TaskHandle_t LvglGui_Handler;
extern TaskHandle_t GatherDataTask_Handler;	 	//������
extern TaskHandle_t PlayMusic_Handler;
extern QueueHandle_t MessageQueue_Tem;


#endif
