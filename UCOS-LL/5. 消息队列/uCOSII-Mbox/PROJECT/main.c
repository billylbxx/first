#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"   
#include "led.h"
#include "key.h"
#include "usart1.h"
#include "ucos_ii.h"
#include "RGB.h"

//START ����
//�����������ȼ�
#define START_TASK_PRIO      			20 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				64
//�����ջ	
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata);	


#define OS_TASK_LED_PRIO 14
#define OS_TASK_LED_STK_SIZE 128
OS_STK OSTaskLEDStk[OS_TASK_LED_STK_SIZE];

#define OS_TASK_KEY_PRIO 12
#define OS_TASK_KEY_STK_SIZE 128
OS_STK OSTaskKEYStk[OS_TASK_KEY_STK_SIZE];

OS_EVENT *key_mbox;
OS_EVENT *CommQ;
void *CommMsg[2];
void OS_TaskLED (void *p_arg);
void OS_TaskKEY (void *p_arg);

int main(void)
{
	Led_Init();
    Key_init();
		RGB_Init();
    USART1_Init(115200);
    
    OS_CPU_SysTickInit(84000000 / OS_TICKS_PER_SEC);   //ϵͳ�δ��ʼ��
    OSInit();   //OS��ʼ��
    
    OSTaskCreate(start_task,
                (void *)NULL,
                (OS_STK *)&START_TASK_STK[START_STK_SIZE-1],
                START_TASK_PRIO );//������ʼ����
                
    OSStart();//���� os,��ʼ����,�����Ѿ��������ȼ���ߵ�������
}
    

 //��ʼ����
void start_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
   
	
		pdata = pdata; //ʹ��һ�£�û��ʵ������
  	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)  
	
	//������Ϣ����
    CommQ=OSQCreate (&CommMsg[0], sizeof(CommMsg)/sizeof(void*));//����һ��ָ����Ϣ�����¼����ƿ��ָ��
		
	
 	//��������
    OSTaskCreate(OS_TaskLED,
                (void *)NULL,
                &OSTaskLEDStk[OS_TASK_LED_STK_SIZE - 1u],
                OS_TASK_LED_PRIO);
                
    OSTaskCreate(OS_TaskKEY,
                (void *)NULL,
                &OSTaskKEYStk[OS_TASK_KEY_STK_SIZE - 1u],
                OS_TASK_KEY_PRIO);
                
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
} 



void OS_TaskLED (void *p_arg)
{
    INT8U err;
		char *mQ_msg;
    OSTimeDlyHMSM(0,0,5,0);
    
    while(1)
    {
       
			mQ_msg=OSQPend(CommQ, 0, &err);
        if(OS_ERR_NONE == err)
		{
			printf("Rx Msg:\r\n%s\r\n",mQ_msg);
            if(strcmp(mQ_msg,"Toggle LED1")==0) //�Ƚϻ�ȡ������Ϣ��������
			{
				GPIO_ToggleBits(GPIOC, GPIO_Pin_4);     //LED1
			}
			else if(strcmp(mQ_msg,"Toggle LED2")==0) //�Ƚϻ�ȡ������Ϣ��������
			{
				GPIO_ToggleBits(GPIOC, GPIO_Pin_5);
			} 
			else if(strcmp(mQ_msg,"Toggle LED3")==0) //�Ƚϻ�ȡ������Ϣ��������
			{
				GPIO_ToggleBits(GPIOB, GPIO_Pin_0);
			} 
		}
//		OSTimeDly(100); //�ͷ� CPU ��ʹ��Ȩ
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
                
        case (1<<0):        //��ǰKEY1�ǰ��µ�״̬
            if(key1) bKeyValue |= 0x80; 
            else bKeyValue = 0;
            break;
                
        case (1<<0)|0x80:
            if(!key1)   /*�ȴ������ͷ�*/
            {
								OSQPost(CommQ,"Toggle LED1");
                OSQPost(CommQ,"Toggle LED2");
                OSQPost(CommQ,"Toggle LED3");
                OSQPost(CommQ,"Toggle LED4");
                //OSQPostFront(CommQ,"Toggle LED5");//���
                bKeyValue = 0;
            }
            break;
                
        case (1<<1):        //��ǰKEY2�ǰ��µ�״̬
            if(!key2) bKeyValue |= 0x80;
            else bKeyValue = 0;
            break;
                
        case (1<<1)|0x80:
            if(key2)   /*�ȴ������ͷ�*/
            {
                OSMboxPost(key_mbox,"Toggle LED2");
                bKeyValue = 0;
            }
            break;
                
        case (1<<2):        //��ǰKEY3�ǰ��µ�״̬
            if(!key3) bKeyValue |= 0x80;
            else bKeyValue = 0;
            break;
                
        case (1<<2)|0x80:
            if(key3)   /*�ȴ������ͷ�*/
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


