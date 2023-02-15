#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"   
#include "led.h"
#include "key.h"
#include "usart1.h"
#include "ucos_ii.h"

uint8_t TimePar = 0;

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
OS_TMR   *LED1TimLoop;
OS_TMR   *LED3TimOnce;

void TIM_LED1Callback (void *ptmr, void *callback_arg);
void TIM_LED3Callback (void *ptmr, void *callback_arg);

void OS_TaskLED (void *p_arg);
void OS_TaskKEY (void *p_arg);

int main(void)
{
	Led_Init();
    Key_init();
    USART1_Init(115200);
    
    OS_CPU_SysTickInit(168000000 / OS_TICKS_PER_SEC);   //系统滴答初始化
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
    INT8U err;
    
	pdata = pdata; 
  	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)  
    
    //创建消息邮箱
    key_mbox=OSMboxCreate(NULL); //创建一个消息邮箱，邮箱中的消息为 LEDON
    //创建软件定时器
    LED1TimLoop=OSTmrCreate(0,     /*初始值，20*10ms*/
                            20,
                            OS_TMR_OPT_PERIODIC,    /*周期模式*/
                            TIM_LED1Callback,
                            &TimePar,
                            (INT8U*)"LED1_LOOP",
                            &err);
    LED3TimOnce=OSTmrCreate(200,     /*初始值，200*10ms*/
                            0,
                            OS_TMR_OPT_ONE_SHOT,
                            TIM_LED3Callback,
                            NULL,
                            (INT8U*)"LED3_ONCE",
                            &err);
    
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
    
    
    TimePar = 3;
    OSTmrStart(LED1TimLoop, &err);  //启动定时器"LED1_LOOP"
    
    while(1)
    {
        mbox_msg = OSMboxPend(key_mbox,0,&err);//阻塞等待一个消息邮箱
		if(err == OS_ERR_NONE)
		{
			printf("Rx Msg:\r\n%s\r\n",mbox_msg);
            if( (strcmp(mbox_msg,"KEY1 Press")==0))
			{
				GPIO_ResetBits(GPIOB, GPIO_Pin_0);     //点亮LED3
                OSTmrStart(LED3TimOnce, &err);      //启动定时器"LED3_ONCE"
			}
            else if(strcmp(mbox_msg,"KEY2 Press")==0)
            {
                OSTmrStop(LED1TimLoop,
                          OS_TMR_OPT_NONE,
                          NULL,
                          &err);
                GPIO_SetBits(GPIOC, GPIO_Pin_4);     //熄灭LED1
            }
            else if(strcmp(mbox_msg,"KEY3 Press")==0)
            {
                OSTmrStart(LED1TimLoop, &err);  //启动定时器"LED1_LOOP"
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
                
        case (1<<0):
            if(key1) bKeyValue |= 0x80; 
            else bKeyValue = 0;
            break;
                
        case (1<<0)|0x80:
            if(!key1)   /*等待按键释放*/
            {
                OSMboxPost(key_mbox,"KEY1 Press");
                bKeyValue = 0;
            }
            break;
                
        case (1<<1):
            if(!key2) bKeyValue |= 0x80;
            else bKeyValue = 0;
            break;
                
        case (1<<1)|0x80:
            if(key2)   /*等待按键释放*/
            {
                OSMboxPost(key_mbox,"KEY2 Press");
                bKeyValue = 0;
            }
            break;
                
        case (1<<2):
            if(!key3) bKeyValue |= 0x80;
            else bKeyValue = 0;
            break;
                
        case (1<<2)|0x80:
            if(key3)   /*等待按键释放*/
            {
                OSMboxPost(key_mbox,"KEY3 Press");
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

void TIM_LED1Callback (void *ptmr, void *callback_arg)
{
    uint8_t a = *(uint8_t*)callback_arg;
    GPIO_ToggleBits(GPIOC, GPIO_Pin_4);    //LED1翻转
}

void TIM_LED3Callback (void *ptmr, void *callback_arg)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_0);     //熄灭LED3
}



