#include "stm32f4xx.h"   
#include "led.h"
#include "key.h"
#include "ucos_ii.h"
#include "SPI.h"
#include "systick.h"
#include "W25QXX.h"
#include "adc.h"
#include "uart.h"
#include <stdio.h>
//START 任务
//设置任务优先级
#define START_TASK_PRIO      			20 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				512
//任务堆栈	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);	


#define OS_TASK_LED1_PRIO 10
#define OS_TASK_LED1_STK_SIZE 512
OS_STK OSTaskLED1Stk[OS_TASK_LED1_STK_SIZE];

#define OS_TASK_LED2_PRIO 12
#define OS_TASK_LED2_STK_SIZE 512
OS_STK OSTaskLED2Stk[OS_TASK_LED2_STK_SIZE];

#define OS_TASK_LED3_PRIO 11
#define OS_TASK_LED3_STK_SIZE 512
OS_STK OSTaskLED3Stk[OS_TASK_LED3_STK_SIZE];

//#define OS_TASK_KEY1_PRIO 12
//#define OS_TASK_KEY1_STK_SIZE 64
//OS_STK OSTaskKEY1Stk[OS_TASK_KEY1_STK_SIZE];

#define OS_TASK_LCD_PRIO 13
#define OS_TASK_LCD_STK_SIZE 512
OS_STK OSTaskLCDStk[OS_TASK_LCD_STK_SIZE];

OS_EVENT *DispSem;//指向事件控制块的指针

void OS_TaskLED1 (void *p_arg);
void OS_TaskLED2 (void *p_arg);
void OS_TaskLED3 (void *p_arg);
void OS_TaskKEY1 (void *p_arg);
void OS_TaskLCD(void*p_arg);

int main(void)
{
	Led_Init();
    Key_init();
    lcd_Init();
		w25q64_Init();
		Adc1_Init();
		Uart_Init();
    OS_CPU_SysTickInit(168000000 / OS_TICKS_PER_SEC);   //系统滴答初始化
    OSInit();   //OS初始化
    
    OSTaskCreate(start_task,
                (void *)0,
                (OS_STK *)&START_TASK_STK[START_STK_SIZE-1],
                START_TASK_PRIO );//创建起始任务
                
    OSStart();//开启 os,开始调度,跳到已经就绪优先级最高的任务中
}
    

 //开始任务
void start_task(void *pdata)
{
    uint8_t Test_pArg = 0xaa;
    
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
    
  	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)  
//    //创建信号量
   // DispSem = OSSemCreate(0); /* 建立显示设备的信号量 */
    
 	//创建任务
    OSTaskCreate(OS_TaskLED1,
                (void *)&Test_pArg,
                &OSTaskLED1Stk[OS_TASK_LED1_STK_SIZE - 1u],
                OS_TASK_LED1_PRIO);
                
    OSTaskCreate(OS_TaskLED2,
                (void *)0,
                &OSTaskLED2Stk[OS_TASK_LED2_STK_SIZE - 1u],
                OS_TASK_LED2_PRIO);
                
//    OSTaskCreate(OS_TaskKEY1,
//                (void *)0,
//                &OSTaskKEY1Stk[OS_TASK_KEY1_STK_SIZE - 1u],
//                OS_TASK_KEY1_PRIO);
     
		OSTaskCreate(OS_TaskLED3,
                (void *)0,
                &OSTaskLED3Stk[OS_TASK_LED3_STK_SIZE - 1u],
                OS_TASK_LED3_PRIO);
								
		OSTaskCreate(OS_TaskLCD,
                (void *)&Test_pArg,
                &OSTaskLCDStk[OS_TASK_LCD_STK_SIZE - 1u],
                OS_TASK_LCD_PRIO);						
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
                
//	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
    OSTaskDel(START_TASK_PRIO);
} 



void OS_TaskLED1 (void *p_arg)
{
    while(1)
    {
        GPIO_ToggleBits(GPIOC, GPIO_Pin_4);     //LED1闪烁
				char op[50]={0};
				static	int j=0;
				j++;
				sprintf(op,"LED1闪烁次数:%d",j);
				LCD_FLASH_SHOW_CHINESE_STR(0,0,op,LCD_WHITE,LCD_BLUE,16);
        OSTimeDly(1000);
    }
}

void OS_TaskLED2 (void *p_arg)
{
    while(1)
    {
        GPIO_ToggleBits(GPIOC, GPIO_Pin_5);     //LED2闪烁
				char str[50]={0};
				static	int i=0;
				i++;
				//printf("%d\r\n",i);
				sprintf(str,"LED2闪烁次数:%d",i);
				LCD_FLASH_SHOW_CHINESE_STR(0,32,str,LCD_WHITE,LCD_BLUE,16);
        OSTimeDly(600);
    }
}


//void OS_TaskKEY1 (void *p_arg)
//{
//    while(1)
//    {
//        if(1==Key_Scan())  /*如果按键的按下*/
//        {
//            OSSemPost(DispSem);//发送信号量
//					
//        }
//        
//        OSTimeDly(100);
//    }
//}

void OS_TaskLED3 (void *p_arg)
{
    while(1)
    {
        GPIO_ToggleBits(GPIOB, GPIO_Pin_0);     //LED2闪烁
				char str[50]={0};
				static	int i=0;
				i++;
				sprintf(str,"LED3闪烁次数:%d",i);
				LCD_FLASH_SHOW_CHINESE_STR(0,60,str,LCD_WHITE,LCD_BLUE,16);
        OSTimeDly(800);
    }
}

void OS_TaskLCD(void*p_arg)
{
	while(1)
	{
		char str[20]={0};
		int i=0;
		i=Get_value(ADC_Channel_10);
		sprintf(str,"%d",i);
		LCD_FLASH_SHOW_CHINESE_STR(0,100,str,LCD_WHITE,LCD_BLUE,16);	
        OSTimeDly(500);
	}
}
