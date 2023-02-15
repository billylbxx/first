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
#include "sd_driver.h"
#include "ff.h"
#include "malloc.h"
#include "wm8978.h"
#include "wav.h"
#include "scanf_dir.h"
#include "usart3.h"
#include "usart2.h"
#include "esp8266.h"
#include "ucos_ii.h"
#include "delay.h"


struct STU
{
	char name[20];
	int  num;
	char sex;
}s={"�����ϳ�",18,'M'};



OS_STK startPtos[256];  //��ʼ�����ջ
OS_STK musicPtos[512];  //���������ջ

//ucosIIÿ����������ȼ����벻ͬ 0-63(62)  
#define startTaskPrio 60 //��ʼ��������ȼ�
#define musicTaskPrio 10 //������������ȼ�

//H����
#define OS_TASK_H_PRIO 8
#define OS_TASK_H_STK_SIZE 128
OS_STK OSTaskHStk[OS_TASK_H_STK_SIZE];
void  OS_TaskH (void *p_arg);

//M����
#define OS_TASK_M_PRIO 10
#define OS_TASK_M_STK_SIZE 128
OS_STK OSTaskMStk[OS_TASK_M_STK_SIZE];
void  OS_TaskM (void *p_arg);

//L����
#define OS_TASK_L_PRIO 12
#define OS_TASK_L_STK_SIZE 128
OS_STK OSTaskLStk[OS_TASK_L_STK_SIZE];
void  OS_TaskL (void *p_arg);

OS_TMR  *timer1;
OS_FLAG_GRP  *flag1;
	
void startTask (void *p_arg);
void musicTask (void *p_arg);
void MyCallback (OS_TMR *ptmr, void *p_arg);
int main()
{	
	FATFS fs;
	FRESULT res;
	unsigned char key = 0;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�����жϷ��飬ֻ����һ��
	Systick_Inti(168);			//ϵͳ�δ��ʼ��
	Usart1_Init(115200);		//��ʼ��USART1��������Ϊ115200
	Led2Pwm_Init(100, 840);		//1khz��CCRԽ�󣬵�Խ��
	HollowPwm_Init(100, 840);	//1khz��CCRԽ�󣬿��ı�ת��Խ�죬ע�⣺������Ҫ��IOֱ�Ӹߵ͵�ƽ����
	SteeringPwm_Init(2000,840);	//50hz��ע�⣺������50hz
//	Led_Init();			//LED�˿ڳ�ʼ�� 
	Beep_Init();		//�������˿ڳ�ʼ��
	Key_Init();			//�����˿ڳ�ʼ��

	Sterilize_Init();	//�̵����˿ڳ�ʼ�������Ƽ��ȣ�
	Rgb_Init();			//��ʼ��RGB
	W25QXX_Init();		//��ʼ��W25Q64
	Lcd_Init();			//��ʼ��LCD
	Sht30_Init();		//��ʼ��SHT30
	Mlx90614_Init();	//��ʼ��MLX90614
	//MemEx_Init();  //�ⲿSRAM��ʼ��
	MemIn_Init();	 //�ڲ�SRAM��ʼ��
	Usart3_Init(115200);
	Usart2_Init(115200);
	res = f_mount(&fs,"0",1);
	if(res ==FR_OK)
	{
		printf("SD��ע��ɹ�\r\n");
	}else{
		printf("SD��ע��ʧ��\r\n");
	}
	Music_Init();  //���ֲ��ų�ʼ��
	
	SetServoAngle(0);		//ת�� 0���λ��
	key = Scanf_Key(0);			//��ס���ⰴ����λ��������дģʽ
	if(key)
	{
		USART1_W25QXX(0,64);	//��д��ϻ��Զ��˳���ģʽ
	}
	
//	ESP8266_ConnectAlliyun();
	
	printf("The program starts running!\r\n");
	LCD_ShowFlashString(0, 0, "�ǻ����� Intelligent Life��", LCD_RED, LCD_WHITE);	//�ַ����ɱ�����ģ��ÿ�һЩ
 
	OS_CPU_SysTickInit(168000000/OS_TICKS_PER_SEC); //������������ --�೤ʱ���һ���жϣ�
	OSInit();//ucso��ʼ��
	
	//��������
	//add your code
	OSTaskCreate (startTask, //��ʼ����
								NULL,
                &startPtos[256-1], //С��ģʽ����ջ����ʼ��
                 startTaskPrio); //��������ȼ���
//	
//  OSTaskCreate (musicTask,
//								NULL,
//                &musicPtos[512-1], //С��ģʽ����ջ����ʼ��
//                musicTaskPrio);
	
	OSStart(); //����ucso
	//��OSStart()֮��ĳ��򲻻�����
	#if 0
	while(1)
	{
		if(Usart3.RecFlag)
		{
			if(Usart3.RxBuff[0] == 0x4f && Usart3.RxBuff[3] == 0xf4)
			{
				switch(Usart3.RxBuff[2])
				{
					case 0x55: WAV_Play("0:/MUSIC/���»� - ��������.wav");break;
					case 0xAA: WAV_Play("0:/prompt/��������.wav"); 				break;
					case 0xAB: WAV_Play("0:/prompt/����������.wav");			break;
				}	
			}
			for(i=0;i<Usart3.RecLen;i++)
			{
				printf("%x ",Usart3.RxBuff[i]);
			}
			printf("\r\n");
			memset(&Usart3,0,sizeof(Usart3));
		}
		if(Usart2.RecFlag)
		{
			printf("%s\r\n",Usart2.RxBuff);
			if(strstr((char*)Usart2.RxBuff,"+MQTTSUBRECV"))
			{		
				if((sp=strstr((char*)Usart2.RxBuff,"MusicPlayControl"))!=NULL)
				{
					sp+= 18;
					if(*sp == '0'){
						WAV_Play("0:/MUSIC/���»� - ��������.wav");
				  }
					//else if()
				}
				//else if()
			}
			memset(&Usart2,0,sizeof(Usart2));
		}	
	  count++;
		if(count>=500)
		{
			count = 0;
			if(ESP8266_Pub())
			{
				printf("MQTT pub OK");
			}
		}
		Delay_Ms(10);
	}
	#endif
}

//ucosһ��Ҫ�ͷ�CPU��ʹ��Ȩ
//��Ȼ���������ȼ��͵������ȡ����CPU
//��ô�ͷ�CPU
void musicTask (void *p_arg)
{
	p_arg = p_arg; //��ֹ����Ұָ��
  while(1) 
	{
     printf("musicTask***************************\r\n");
		 //Delay_Ms(500); //�����ͷ�CPUʹ��Ȩ--Ϊʲô����
		//�ͷ�100�������ĵ�ʱ�䣬һ���������ĵ�ʱ��Ϊ5ms--��������--OS_TICKS_PER_SEC
		 OSTimeDly(100); //�ͷ�CPUʹ��Ȩ --500ms --
  }
}

void startTask(void *p_arg)
{
	INT8U   perr;
	OS_CPU_SR  cpu_sr = 0u;
	p_arg = p_arg;
	OS_ENTER_CRITICAL();  //�����ٽ�
	
	//����һ�������ʱ�� --ֻ�Ǵ����ˣ�û��������
	timer1 = OSTmrCreate ( 400, //400
												 0,
                      OS_TMR_OPT_ONE_SHOT, //����ģʽ
                      MyCallback,
                      NULL,
                      "XING",
                      &perr);
	if(perr == OS_ERR_NONE){
		printf("�����ʱ�������ɹ�\r\n");
	}
	//����һ���¼���־��--��ʼֵΪ0
	flag1 = OSFlagCreate (0x00,&perr);
	
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
	OS_EXIT_CRITICAL();  //�˳��ٽ�
	OSTaskSuspend(OS_PRIO_SELF);//�����Լ�
}

//�����ʱ���Ļص�����
void MyCallback (OS_TMR *ptmr, void *p_arg)
{
		printf("��ʱʱ�䵽��\r\n");
}

void  OS_TaskH (void *p_arg)
{
		INT8U perr;
    p_arg = p_arg; 
    while(1) {	
			//�ȴ��¼�����
			OSFlagPend(flag1,0x06,//�ȴ���2λ�͵�1λ����
								OS_FLAG_WAIT_SET_ALL+ OS_FLAG_CONSUME,//�ȴ���λ��Ϊ1ʱ�������������־
								0,&perr);
			if(perr ==OS_ERR_NONE){ //��������
					printf("���������ʱ��\r\n");
				  OSTmrStart(timer1,&perr);	
			}
			delay_ms(500);
    }
}

void  OS_TaskM (void *p_arg)
{
		u8 key;
		INT8U perr;
    p_arg = p_arg;
    for (;;) {	
			key = Scanf_Key(0);
			 if(key == 1){
					OSFlagPost(flag1,0x02,//���õ�һλΪ1
								OS_FLAG_SET,&perr);
			 }else if (key == 2){
					OSFlagPost(flag1,0x04,//���õ�һλΪ1
								OS_FLAG_SET,&perr);	
			 }
			 OSTimeDlyHMSM(0,0,0,10);
    }
}

void  OS_TaskL (void *p_arg)
{

    p_arg = p_arg;
    for (;;) {
			//printf("33333\r\n");
		  //OSTimeDlyHMSM(0,0,1,0);
			delay_ms(500);
    }
}

/*
��ϰ��
1 .ͨ������ֹͣ�����ʱ����ʵ�ֲ�ͬ���������֤�书��
2. ����һ��ѭ��ģʽ�������ʱ������֤�书��
*/


