#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"   
#include "led.h"
#include "key.h"
#include "usart1.h"
#include "ucos_ii.h"
#include "RGB.h"

//START 任务
//设置任务优先级
#define START_TASK_PRIO      			20 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				64
//任务堆栈	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);	


#define OS_TASK_LED_PRIO 14
#define OS_TASK_LED_STK_SIZE 128
OS_STK OSTaskLEDStk[OS_TASK_LED_STK_SIZE];

#define OS_TASK_KEY_PRIO 12
#define OS_TASK_KEY_STK_SIZE 128
OS_STK OSTaskKEYStk[OS_TASK_KEY_STK_SIZE];

OS_EVENT *key_mbox;

void OS_TaskLED (void *p_arg);
void OS_TaskKEY (void *p_arg);

int main(void)
{
	Led_Init();
    Key_init();
		RGB_Init();
    USART1_Init(115200);
    
    OS_CPU_SysTickInit(84000000 / OS_TICKS_PER_SEC);   //系统滴答初始化
    OSInit();   //OS初始化
    
    OSTaskCreate(start_task,
                (void *)NULL,
                (OS_STK *)&START_TASK_STK[START_STK_SIZE-1],
                START_TASK_PRIO );//创建起始任务
                
    OSStart();//开启 os,开始调度,跳到已经就绪优先级最高的任务中
}
    

 //开始任务
void start_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
    
		pdata = pdata; //使用一下，没有实际意义
  	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)  
    //创建消息邮箱
    key_mbox = OSMboxCreate(NULL); //创建一个消息邮箱，邮箱中的消息为 LEDON
    
 	//创建任务
    OSTaskCreate(OS_TaskLED,
                (void *)NULL,
                &OSTaskLEDStk[OS_TASK_LED_STK_SIZE - 1u],
                OS_TASK_LED_PRIO);
                
    OSTaskCreate(OS_TaskKEY,
                (void *)NULL,
                &OSTaskKEYStk[OS_TASK_KEY_STK_SIZE - 1u],
                OS_TASK_KEY_PRIO);
                
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
} 



void OS_TaskLED (void *p_arg)
{
    INT8U err;
	char *mbox_msg;
    u32 x=0xFFFFFF;
    OSTimeDlyHMSM(0,0,5,0);
    
    while(1)
    {
        mbox_msg = OSMboxPend(key_mbox,0,&err);//阻塞等待一个消息邮箱
		
        if(OS_ERR_NONE == err)
		{
			printf("Rx Msg:\r\n%s\r\n",mbox_msg);
            if(strcmp(mbox_msg,"Toggle LED1")==0) //比较获取到的消息邮箱内容
			{
//				if(x>0x323232)
//				{
//					x-=0x323232;
//				}else x=0xFFFFFF;
//				RGB_TO_GRB(0,x);
//				RGB_ENABLE();
				GPIO_ToggleBits(GPIOC, GPIO_Pin_4);     //LED1
			}
			else if(strcmp(mbox_msg,"Toggle LED2")==0) //比较获取到的消息邮箱内容
			{
				GPIO_ToggleBits(GPIOC, GPIO_Pin_5);
			} 
			else if(strcmp(mbox_msg,"Toggle LED3")==0) //比较获取到的消息邮箱内容
			{
				GPIO_ToggleBits(GPIOB, GPIO_Pin_0);
			} 
		}
//		OSTimeDly(100); //释放 CPU 的使用权
    }
}


void OS_TaskKEY (void *p_arg)
{
    uint8_t bKeyValue = 0;
    
    while(1)
    {
        switch(bKeyValue)
        {
        case 0:
            if(key1) bKeyValue = (1<<0);
            else if(!key2) bKeyValue = (1<<1);
            else if(!key3) bKeyValue = (1<<2);
            break;
                
        case (1<<0):        //此前KEY1是按下的状态
            if(key1) bKeyValue |= 0x80; 
            else bKeyValue = 0;
            break;
                
        case (1<<0)|0x80:
            if(!key1)   /*等待按键释放*/
            {
                OSMboxPost(key_mbox,"Toggle LED1");
                OSMboxPost(key_mbox,"Toggle LED2");
                OSMboxPost(key_mbox,"Toggle LED3");
                OSMboxPost(key_mbox,"Toggle LED4");
                OSMboxPost(key_mbox,"Toggle LED5");
                bKeyValue = 0;
            }
            break;
                
        case (1<<1):        //此前KEY2是按下的状态
            if(!key2) bKeyValue |= 0x80;
            else bKeyValue = 0;
            break;
                
        case (1<<1)|0x80:
            if(key2)   /*等待按键释放*/
            {
                OSMboxPost(key_mbox,"Toggle LED2");
                bKeyValue = 0;
            }
            break;
                
        case (1<<2):        //此前KEY3是按下的状态
            if(!key3) bKeyValue |= 0x80;
            else bKeyValue = 0;
            break;
                
        case (1<<2)|0x80:
            if(key3)   /*等待按键释放*/
            {
                OSMboxPost(key_mbox,"Toggle LED3");
                bKeyValue = 0;
            }
            break;
                
        default:
            bKeyValue = 0;
            break;
        }
        OSTimeDly(20);
    }
}


