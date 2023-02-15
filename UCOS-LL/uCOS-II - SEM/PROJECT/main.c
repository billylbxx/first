#include "stm32f4xx.h"   
#include "led.h"
#include "key.h"
#include "ucos_ii.h"


//START ����
//�����������ȼ�
#define START_TASK_PRIO      			20 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				64
//�����ջ	
OS_STK START_TASK_STK[START_STK_SIZE];
//������
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

OS_EVENT *DispSem;//ָ���¼����ƿ��ָ��

void OS_TaskLED1 (void *p_arg);
void OS_TaskLED2 (void *p_arg);
void OS_TaskKEY1 (void *p_arg);

int main(void)
{
	Led_Init();
    Key_init();
    
    OS_CPU_SysTickInit(168000000 / OS_TICKS_PER_SEC);   //ϵͳ�δ��ʼ��
    OSInit();   //OS��ʼ��
    
    OSTaskCreate(start_task,
                (void *)0,
                (OS_STK *)&START_TASK_STK[START_STK_SIZE-1],
                START_TASK_PRIO );//������ʼ����
                
    OSStart();//���� os,��ʼ����,�����Ѿ��������ȼ���ߵ�������
}
    

 //��ʼ����
void start_task(void *pdata)
{
//    uint8_t Test_pArg = 0xaa;
    OS_CPU_SR cpu_sr=0;   //OS_CPU_SR��CPU ״̬�Ĵ��� psr �Ĵ�С 32 λ,cpu_sr��������Ϊ�ر��ж�ǰ��״̬
		pdata = pdata; 
    
  	OS_ENTER_CRITICAL();			//�ر��жϣ�(�ٽ�����ָ���Ǵ���ʱ�����Էָ�Ĵ���),�����ٽ���(�޷����жϴ��)  
    //�����ź���
    DispSem = OSSemCreate(4); /* ������ʾ�豸���ź��� */
    //OSSemSet(DispSem,6,&Test_pArg);//�ı䵱ǰ�ź����ļ���ֵ
 	//��������
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
                
	OS_EXIT_CRITICAL();				//���жϣ��˳��ٽ���(���Ա��жϴ��)
                
//	OSTaskSuspend(START_TASK_PRIO);	//������ʼ�����뵱����ͣ
    OSTaskDel(START_TASK_PRIO);//ɾ�����񣬲������л�����ȣ��������´���
} 


void OS_TaskLED1 (void *p_arg)
{
    INT8U err;
    OS_SEM_DATA  SEM_INFO;
    
    while(1)
    {
        OSSemQuery(DispSem, &SEM_INFO);//��ȡһ���ź����������Ϣ
        
        OSSemPend(DispSem, 0, &err);//��������ȴ��ź���,�ú�������������ͼȡ���豸��ʹ��Ȩ,�ź�ֵ��1
        
        OSSemQuery(DispSem, &SEM_INFO);
        
        if(OS_ERR_NONE == err)
        {
            GPIO_ToggleBits(GPIOC, GPIO_Pin_4);     //LED1��˸
        }
//        OSTimeDly(200);
        OSTimeDlyHMSM(0,0,0,200);
    }
}

void OS_TaskLED2 (void *p_arg)
{
    while(1)
    {
        GPIO_ToggleBits(GPIOC, GPIO_Pin_5);     //LED2��˸
//        OSTimeDly(1000);
        OSTimeDlyHMSM(0,0,1,0);
    }
}


void OS_TaskKEY1 (void *p_arg)
{
    while(1)
    {
        if(1==Key_Scan())  /*��������İ���*/
        {
					OSSemPost(DispSem);//�����ź���������һ�Σ��ź�ֵ��1
					OSSemPost(DispSem);//�����ź���
					OSSemPost(DispSem);//�����ź���
					OSSemPost(DispSem);//�����ź���
					OSSemPost(DispSem);//�����ź���
					OSSemPost(DispSem);//�����ź���
					
        }
        
//        OSTimeDly(100);
        OSTimeDlyHMSM(0,0,0,100);
    }
}


