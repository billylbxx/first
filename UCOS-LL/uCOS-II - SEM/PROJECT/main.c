#include "stm32f4xx.h"   
#include "led.h"
#include "key.h"
#include "ucos_ii.h"


//START 任务
//设置任务优先级
#define START_TASK_PRIO      			20 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				64
//任务堆栈	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);	


#define OS_TASK_LED1_PRIO 15
#define OS_TASK_LED1_STK_SIZE 64
OS_STK OSTaskLED1Stk[OS_TASK_LED1_STK_SIZE];

#define OS_TASK_LED2_PRIO 11
#define OS_TASK_LED2_STK_SIZE 64
OS_STK OSTaskLED2Stk[OS_TASK_LED2_STK_SIZE];

#define OS_TASK_KEY1_PRIO 12
#define OS_TASK_KEY1_STK_SIZE 64
OS_STK OSTaskKEY1Stk[OS_TASK_KEY1_STK_SIZE];

OS_EVENT *DispSem;//指向事件控制块的指针

void OS_TaskLED1 (void *p_arg);
void OS_TaskLED2 (void *p_arg);
void OS_TaskKEY1 (void *p_arg);

int main(void)
{
	Led_Init();
    Key_init();
    
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
//    uint8_t Test_pArg = 0xaa;
    OS_CPU_SR cpu_sr=0;   //OS_CPU_SR：CPU 状态寄存器 psr 的大小 32 位,cpu_sr用来保留为关闭中断前的状态
		pdata = pdata; 
    
  	OS_ENTER_CRITICAL();			//关闭中断，(临界区，指的是处理时不可以分割的代码),进入临界区(无法被中断打断)  
    //创建信号量
    DispSem = OSSemCreate(4); /* 建立显示设备的信号量 */
    //OSSemSet(DispSem,6,&Test_pArg);//改变当前信号量的计数值
 	//创建任务
    OSTaskCreate(OS_TaskLED1,
                (void *)0,
                &OSTaskLED1Stk[OS_TASK_LED1_STK_SIZE - 1u],
                OS_TASK_LED1_PRIO);
                
    OSTaskCreate(OS_TaskLED2,
                (void *)0,
                &OSTaskLED2Stk[OS_TASK_LED2_STK_SIZE - 1u],
                OS_TASK_LED2_PRIO);
                
    OSTaskCreate(OS_TaskKEY1,
                (void *)0,
                &OSTaskKEY1Stk[OS_TASK_KEY1_STK_SIZE - 1u],
                OS_TASK_KEY1_PRIO);
                
	OS_EXIT_CRITICAL();				//打开中断，退出临界区(可以被中断打断)
                
//	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务，想当于暂停
    OSTaskDel(START_TASK_PRIO);//删除任务，不让再有机会调度，除非重新创建
} 


void OS_TaskLED1 (void *p_arg)
{
    INT8U err;
    OS_SEM_DATA  SEM_INFO;
    
    while(1)
    {
        OSSemQuery(DispSem, &SEM_INFO);//获取一个信号量的相关信息
        
        OSSemPend(DispSem, 0, &err);//挂起任务等待信号量,该函数用于任务试图取得设备的使用权,信号值减1
        
        OSSemQuery(DispSem, &SEM_INFO);
        
        if(OS_ERR_NONE == err)
        {
            GPIO_ToggleBits(GPIOC, GPIO_Pin_4);     //LED1闪烁
        }
//        OSTimeDly(200);
        OSTimeDlyHMSM(0,0,0,200);
    }
}

void OS_TaskLED2 (void *p_arg)
{
    while(1)
    {
        GPIO_ToggleBits(GPIOC, GPIO_Pin_5);     //LED2闪烁
//        OSTimeDly(1000);
        OSTimeDlyHMSM(0,0,1,0);
    }
}


void OS_TaskKEY1 (void *p_arg)
{
    while(1)
    {
        if(1==Key_Scan())  /*如果按键的按下*/
        {
					OSSemPost(DispSem);//发送信号量，发送一次，信号值加1
					OSSemPost(DispSem);//发送信号量
					OSSemPost(DispSem);//发送信号量
					OSSemPost(DispSem);//发送信号量
					OSSemPost(DispSem);//发送信号量
					OSSemPost(DispSem);//发送信号量
					
        }
        
//        OSTimeDly(100);
        OSTimeDlyHMSM(0,0,0,100);
    }
}


