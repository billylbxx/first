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

OS_EVENT  *Sem;
int main(void)//当前进程并未执行完毕，它又不愿放弃CPU的使用权，而被强制终止执行，使得CPU的使用权归其他进程，这种内核属于可剥夺型内核。(优先级反转)
{
    delay_init(168);        //延时初始化
    NVIC_SetPriorityGrouping(7-2); //设置为分组2    
    USART1_Init(115200);        //串口波特率设置
//    Led_Init();
//    Key_Init();
//    rtc_init();
//    //set_rtc_wakeup_time(4,0);
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
    INT8U perr;
    OS_CPU_SR  cpu_sr = 0u;
    p_arg = p_arg; /* Prevent compiler warning for not using 'p_arg'*/
    
    OS_ENTER_CRITICAL();//进入临界区
    Sem=OSSemCreate(1);
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
    for (;;)
    {
        OSTimeDlyHMSM(0,0,0,500);
        printf("任务H抢到CPU\r\n");  // （5） 
        printf("任务H想获取信号量\r\n");  // （5） 
        OSSemPend(Sem,0,&perr);                  //  （6）
        printf("任务H得到信号量，开始执行\r\n");
//        cnt++;                                 //  （13）   
        OSSemPost(Sem);

        OSTimeDlyHMSM(0,0,1,0);
    }
}

void  OS_TaskM (void *p_arg)
{
    u8 cnt=0;
    p_arg = p_arg; /* Prevent compiler warning for not using 'p_arg'*/
    for (;;)
    {
        OSTimeDlyHMSM(0,0,0,600);
 
        cnt++;                         // （8）
        printf("任务M抢到CPU正在执行，执行了%d次\r\n",cnt);
        OSTimeDlyHMSM(0,0,1,0);
    }
}

void  OS_TaskL (void *p_arg)
{
    INT8U perr;
    u32 i,j;
    p_arg = p_arg; /* Prevent compiler warning for not using 'p_arg'     */
    for (;;) 
    {
        printf("任务L正在执行\r\n");
        OSSemPend(Sem,0,&perr);  //（2）
        for(i=0;i<30000;i++)   //
        {
            for(j=0;j<3000;j++);
        }
        printf("任务L即将释放信号量\r\n");
        OSSemPost(Sem);               //（12） 释放信号量

        OSTimeDlyHMSM(0,0,1,0);
    }
}
