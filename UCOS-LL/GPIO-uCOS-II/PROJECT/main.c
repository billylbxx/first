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
//START ����
//�����������ȼ�
#define START_TASK_PRIO      			20 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				512
//�����ջ	
OS_STK START_TASK_STK[START_STK_SIZE];
//������
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

OS_EVENT *DispSem;//ָ���¼����ƿ��ָ��

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
    uint8_t Test_pArg = 0xaa;
    
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
    
  	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)  
//    //�����ź���
   // DispSem = OSSemCreate(0); /* ������ʾ�豸���ź��� */
    
 	//��������
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
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
                
//	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
    OSTaskDel(START_TASK_PRIO);
} 



void OS_TaskLED1 (void *p_arg)
{
    while(1)
    {
        GPIO_ToggleBits(GPIOC, GPIO_Pin_4);     //LED1��˸
				char op[50]={0};
				static	int j=0;
				j++;
				sprintf(op,"LED1��˸����:%d",j);
				LCD_FLASH_SHOW_CHINESE_STR(0,0,op,LCD_WHITE,LCD_BLUE,16);
        OSTimeDly(1000);
    }
}

void OS_TaskLED2 (void *p_arg)
{
    while(1)
    {
        GPIO_ToggleBits(GPIOC, GPIO_Pin_5);     //LED2��˸
				char str[50]={0};
				static	int i=0;
				i++;
				//printf("%d\r\n",i);
				sprintf(str,"LED2��˸����:%d",i);
				LCD_FLASH_SHOW_CHINESE_STR(0,32,str,LCD_WHITE,LCD_BLUE,16);
        OSTimeDly(600);
    }
}


//void OS_TaskKEY1 (void *p_arg)
//{
//    while(1)
//    {
//        if(1==Key_Scan())  /*��������İ���*/
//        {
//            OSSemPost(DispSem);//�����ź���
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
        GPIO_ToggleBits(GPIOB, GPIO_Pin_0);     //LED2��˸
				char str[50]={0};
				static	int i=0;
				i++;
				sprintf(str,"LED3��˸����:%d",i);
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
