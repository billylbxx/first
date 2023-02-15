#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "bsp_lcd.h"
#include "gpio.h"
#include "rtc.h"
#include "timer.h"
#include "touch.h"
#include "sd.h"
#include "ucos_ii.h"
#include "RGB.h"
#include "key.h"
//start 任务
#define OS_TASK_START_PRIO 20
#define OS_TASK_START_STK_SIZE 128
OS_STK OSTaskStartStk[OS_TASK_START_STK_SIZE];
void start_task(void * p_arg);


//H任务
#define OS_TASK_H_PRIO 8
#define OS_TASK_H_STK_SIZE 128
OS_STK OSTaskHStk[OS_TASK_H_STK_SIZE];
void  OS_TaskH (void *p_arg);


//M任务
#define OS_TASK_M_PRIO 10
#define OS_TASK_M_STK_SIZE 128
OS_STK OSTaskMStk[OS_TASK_M_STK_SIZE];
void  OS_TaskM (void *p_arg);

//L任务
#define OS_TASK_L_PRIO 12
#define OS_TASK_L_STK_SIZE 128
OS_STK OSTaskLStk[OS_TASK_L_STK_SIZE];
void  OS_TaskL (void *p_arg);

OS_EVENT  *Mutex;
int main(void)
{
    delay_init(168);        //延时初始化
    NVIC_SetPriorityGrouping(7-2); //设置为分组2    
    USART1_Init(115200);        //串口波特率设置
    Led_Init();
    key_Init();
		RGB_Init();
//    rtc_init();
    //set_rtc_wakeup_time(4,0);
//    TFTLCD_Init();
//    sFLASH_Init();
    OS_CPU_SysTickInit(168000000/OS_TICKS_PER_SEC); //设置系统滴答中断时间（时钟节拍时间）
    OSInit(); //ucos 初始化
    //创建任务
    OSTaskCreate(start_task,NULL,&OSTaskStartStk[128-1],OS_TASK_START_PRIO);//处理就绪状态
    OSStart();//启用任务调度 
}
void start_task(void * p_arg)
{
    u8 perr;
    OS_CPU_SR  cpu_sr = 0u;
    p_arg = p_arg; /* Prevent compiler warning for not using 'p_arg'*/
    OS_ENTER_CRITICAL();//进入临界区
    
	Mutex=OSMutexCreate(6,&perr);  //互斥信号持有者的优先级为6
    
    OSTaskCreate(OS_TaskH,
                (void *)0,
                &OSTaskHStk[OS_TASK_H_STK_SIZE - 1u],
                OS_TASK_H_PRIO);                   
    OSTaskCreate(OS_TaskM,
                (void *)0,
                &OSTaskMStk[OS_TASK_M_STK_SIZE - 1u],
                OS_TASK_M_PRIO);
    OSTaskCreate(OS_TaskL,
                (void *)0,
                &OSTaskLStk[OS_TASK_L_STK_SIZE - 1u],
                OS_TASK_L_PRIO);
    OS_EXIT_CRITICAL();//退出临界区         
    for (;;)
    {
        OSTaskDel(OS_PRIO_SELF);
    }
}

void  OS_TaskH (void *p_arg)
{
    INT8U perr;
    u8 cnt=0;
    p_arg = p_arg; /* Prevent compiler warning for not using 'p_arg'     */
    
    OS_TCB TaskH_Info;
    OSTaskQuery (OS_TASK_H_PRIO, &TaskH_Info);
    
    for (;;)
    {      
        OSTimeDlyHMSM(0,0,0,50);
        printf("任务H想获取信号量\r\n");         // 
        OSMutexPend(Mutex,0,&perr);              //  阻塞任务等待互斥型信号量
        cnt++;                                   //  
        printf("任务H正在执行\r\n");
//					RGB_TO_GRB(0,0x452849);
//					RGB_ENABLE();
					
				OSMutexPost(Mutex);      //释放一个互斥型信号量
        OSTimeDlyHMSM(0,0,1,0);
    }
}

void  OS_TaskM (void *p_arg)
{
    u8 cnt=0;
    p_arg = p_arg; /* Prevent compiler warning for not using 'p_arg'*/
    
    OS_TCB TaskM_Info;
    OSTaskQuery (OS_TASK_M_PRIO, &TaskM_Info);
    
    	u32 x=0xFFFFFF;
    for (;;) 
    {
        OSTimeDlyHMSM(0,0,0,50);
//				OSTimeDlyHMSM(0,0,1,0);
        cnt++;                         //
        printf("任务M正在执行，执行了%d次\r\n",cnt);
//			if(x>0x646464)
//			{
//				x-=0x323232;
//			}
//			else x=0xFFFFFF;
//			RGB_TO_GRB(0,x);
//			RGB_ENABLE();
      OSTimeDlyHMSM(0,0,1,0);
    }
}

void  OS_TaskL (void *p_arg)
{
    INT8U perr;
    u32 i,j;
    p_arg = p_arg; /* Prevent compiler warning for not using 'p_arg'     */
    
    OS_TCB TaskL_Info;
    OSTaskQuery (OS_TASK_L_PRIO, &TaskL_Info);
    
    OS_MUTEX_DATA TaskLMutexInfo;
    OSMutexQuery (Mutex, &TaskLMutexInfo);
    
    for (;;) 
    {
        OSMutexPend(Mutex,0,&perr);  //阻塞任务等待互斥型信号量
        OSMutexQuery (Mutex, &TaskLMutexInfo);
        printf("任务L正在执行\r\n");
        for(i=0;i<30000;i++)   //
        {
            for(j=0;j<3000;j++);
        }
        printf("任务L即将释放信号量\r\n");
        
        OSTaskQuery (OS_TASK_L_PRIO, &TaskL_Info);
        
        OSMutexPost(Mutex);               //释放一个互斥型信号量
        OSTaskQuery (OS_TASK_L_PRIO, &TaskL_Info);
        OSMutexQuery (Mutex, &TaskLMutexInfo);
        
        OSTimeDlyHMSM(0,0,1,0);
    }
}
