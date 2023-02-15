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
}s={"隔壁老陈",18,'M'};



OS_STK startPtos[256];  //起始任务的栈
OS_STK musicPtos[512];  //音乐任务的栈

//ucosII每个任务的优先级必须不同 0-63(62)  
#define startTaskPrio 60 //起始任务的优先级
#define musicTaskPrio 10 //音乐任务的优先级

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

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//配置中断分组，只配置一次
	Systick_Inti(168);			//系统滴答初始化
	Usart1_Init(115200);		//初始化USART1，波特率为115200
	Led2Pwm_Init(100, 840);		//1khz，CCR越大，灯越亮
	HollowPwm_Init(100, 840);	//1khz，CCR越大，空心杯转的越快，注意：尽量不要用IO直接高低电平驱动
	SteeringPwm_Init(2000,840);	//50hz，注意：必须是50hz
//	Led_Init();			//LED端口初始化 
	Beep_Init();		//蜂鸣器端口初始化
	Key_Init();			//按键端口初始化

	Sterilize_Init();	//继电器端口初始化（控制加热）
	Rgb_Init();			//初始化RGB
	W25QXX_Init();		//初始化W25Q64
	Lcd_Init();			//初始化LCD
	Sht30_Init();		//初始化SHT30
	Mlx90614_Init();	//初始化MLX90614
	//MemEx_Init();  //外部SRAM初始化
	MemIn_Init();	 //内部SRAM初始化
	Usart3_Init(115200);
	Usart2_Init(115200);
	res = f_mount(&fs,"0",1);
	if(res ==FR_OK)
	{
		printf("SD卡注册成功\r\n");
	}else{
		printf("SD卡注册失败\r\n");
	}
	Music_Init();  //音乐播放初始化
	
	SetServoAngle(0);		//转回 0°的位置
	key = Scanf_Key(0);			//按住任意按键复位将进入烧写模式
	if(key)
	{
		USART1_W25QXX(0,64);	//烧写完毕会自动退出该模式
	}
	
//	ESP8266_ConnectAlliyun();
	
	printf("The program starts running!\r\n");
	LCD_ShowFlashString(0, 0, "智慧生活 Intelligent Life！", LCD_RED, LCD_WHITE);	//字符换成本地字模会好看一些
 
	OS_CPU_SysTickInit(168000000/OS_TICKS_PER_SEC); //设置心跳节拍 --多长时间进一次中断？
	OSInit();//ucso初始化
	
	//创建任务
	//add your code
	OSTaskCreate (startTask, //起始任务
								NULL,
                &startPtos[256-1], //小端模式，从栈顶开始存
                 startTaskPrio); //任务的优先级。
//	
//  OSTaskCreate (musicTask,
//								NULL,
//                &musicPtos[512-1], //小端模式，从栈顶开始存
//                musicTaskPrio);
	
	OSStart(); //启动ucso
	//在OSStart()之后的程序不会运行
	#if 0
	while(1)
	{
		if(Usart3.RecFlag)
		{
			if(Usart3.RxBuff[0] == 0x4f && Usart3.RxBuff[3] == 0xf4)
			{
				switch(Usart3.RxBuff[2])
				{
					case 0x55: WAV_Play("0:/MUSIC/刘德华 - 暗里着迷.wav");break;
					case 0xAA: WAV_Play("0:/prompt/嗯我来了.wav"); 				break;
					case 0xAB: WAV_Play("0:/prompt/我先退下了.wav");			break;
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
						WAV_Play("0:/MUSIC/刘德华 - 暗里着迷.wav");
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

//ucos一定要释放CPU的使用权
//不然比它的优先级低的任务获取不到CPU
//怎么释放CPU
void musicTask (void *p_arg)
{
	p_arg = p_arg; //防止出现野指针
  while(1) 
	{
     printf("musicTask***************************\r\n");
		 //Delay_Ms(500); //不是释放CPU使用权--为什么？？
		//释放100心跳节拍的时间，一个心跳节拍的时间为5ms--从哪里来--OS_TICKS_PER_SEC
		 OSTimeDly(100); //释放CPU使用权 --500ms --
  }
}

void startTask(void *p_arg)
{
	INT8U   perr;
	OS_CPU_SR  cpu_sr = 0u;
	p_arg = p_arg;
	OS_ENTER_CRITICAL();  //进入临界
	
	//创建一个软件定时器 --只是创建了，没有启动。
	timer1 = OSTmrCreate ( 400, //400
												 0,
                      OS_TMR_OPT_ONE_SHOT, //单次模式
                      MyCallback,
                      NULL,
                      "XING",
                      &perr);
	if(perr == OS_ERR_NONE){
		printf("软件定时器创建成功\r\n");
	}
	//创建一个事件标志组--初始值为0
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
	OS_EXIT_CRITICAL();  //退出临界
	OSTaskSuspend(OS_PRIO_SELF);//挂起自己
}

//软件定时器的回调函数
void MyCallback (OS_TMR *ptmr, void *p_arg)
{
		printf("定时时间到了\r\n");
}

void  OS_TaskH (void *p_arg)
{
		INT8U perr;
    p_arg = p_arg; 
    while(1) {	
			//等待事件成立
			OSFlagPend(flag1,0x06,//等待第2位和第1位成立
								OS_FLAG_WAIT_SET_ALL+ OS_FLAG_CONSUME,//等待的位都为1时成立，并且清标志
								0,&perr);
			if(perr ==OS_ERR_NONE){ //条件成立
					printf("启动软件定时器\r\n");
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
					OSFlagPost(flag1,0x02,//设置第一位为1
								OS_FLAG_SET,&perr);
			 }else if (key == 2){
					OSFlagPost(flag1,0x04,//设置第一位为1
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
练习：
1 .通过按键停止软件定时器，实现不同的情况并验证其功能
2. 创建一个循环模式的软件定时器并验证其功能
*/


