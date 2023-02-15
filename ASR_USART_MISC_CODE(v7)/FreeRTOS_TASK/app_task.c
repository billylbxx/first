#include "app_task.h"

TaskHandle_t GatherDataTask_Handler;	 	//任务句柄
TaskHandle_t VoiceRecognition_Handler;		//任务句柄
TaskHandle_t PlayMusic_Handler;				//任务句柄
TaskHandle_t LvglGui_Handler;				//任务句柄

QueueHandle_t MessageQueue_Tem;		//人体红外测量消息队列

lv_ui guider_ui;

void FreeRTOS_Init(void)
{
	 taskENTER_CRITICAL();           //进入临界区
	
	//创建数据采集处理的任务
    xTaskCreate((TaskFunction_t )GatherData_Task,     	
                (const char*    )"GatherData_Task",   	
                (uint16_t       )GatherData_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )GatherData_TASK_PRIO,	
                (TaskHandle_t*  )&GatherDataTask_Handler);
	//创建语音识别处理的任务
	xTaskCreate((TaskFunction_t )VoiceRecognition_Task,     	
                (const char*    )"VoiceRecognition_Task",   	
                (uint16_t       )VoiceRecognition_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )VoiceRecognition_TASK_PRIO,	
                (TaskHandle_t*  )&VoiceRecognition_Handler);     									
	//创建播放音乐的任务
	xTaskCreate((TaskFunction_t )PlayMusic_Task,     	
                (const char*    )"PlayMusic_Task",   	
                (uint16_t       )PlayMusic_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )PlayMusic_TASK_PRIO,	
                (TaskHandle_t*  )&PlayMusic_Handler);   
	//创建GUI处理的任务
	xTaskCreate((TaskFunction_t )LvglGui_Task,     	
                (const char*    )"LvglGui_Task",   	
                (uint16_t       )LvglGui_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LvglGui_TASK_PRIO,	
                (TaskHandle_t*  )&LvglGui_Handler);   
								
	MessageQueue_Tem = xQueueCreate(2, 5); 
	
	lv_init();						// 初始化lvgl
	lv_port_disp_init();
 	LVGL_GUI_APP(&guider_ui);
				
	vTaskStartScheduler();          //开启任务调度
	taskEXIT_CRITICAL();            //退出临界区
}

//负责数据采集、实时显示、网络服务等任务处理
void GatherData_Task(void *pvParameters)
{  
	unsigned short value1 = 0, value2 = 0,count = 0;
	float temperature = 0.0;
	SHT30_TypeDef HT = {0};
	BaseType_t xTaskWokenByReceive = pdFALSE;
	
	while(1)
	{
		if(++count >= 500)
		{
			count = 0;
			Sht30_ReadData(&HT);
			if(HT.Temperature>=0 && HT.Temperature<=100 && HT.Humidity>=0 && HT.Humidity<=100)
			{
				lv_bar_set_value(guider_ui.screen_barTemp, HT.Temperature, LV_ANIM_ON);
				lv_bar_set_value(guider_ui.screen_barHum, HT.Humidity, LV_ANIM_ON);
			}

			value1 = Get_AverageValue(NTC_CH);
			if(value1<=4095 && value2 <=4095)
			{
				lv_bar_set_value(guider_ui.screen_barLight, 100-(double)value1/40.96, LV_ANIM_ON);
			}

			if(ConnectFlag == true)				//连上云端才上报数据
				PublishMessage((void *)&HT);
		}
		
		//如果正在播放音乐，显示进度
		if(status_dev.PlayMode == true)	
		{
			Audio_MsgShow(wavctrl.totsec, wavctrl.cursec);
		}
		
		//接收到人体测量结果的消息
		if(xQueueReceiveFromISR(MessageQueue_Tem, &temperature, &xTaskWokenByReceive) == pdTRUE)	
		{
			if(temperature>=0 && temperature<=100)
			{
				lv_bar_set_value(guider_ui.screen_barFlame, temperature, LV_ANIM_ON);
			}
		}		
		
		vTaskDelay(2);		//一个时钟节拍5ms，5*2=10ms
	}
}   
void VoiceRecognition_Task(void *pvParameters)
{
	unsigned short count = 0;
	
	while(1)
	{
		TaskKeyMsg_Handle();		//按键处理
		TaskEsp12Msg_Handle();	//网络服务处理
		
		if(++count >= 500)
			count = 0;
		
		if((Get_Esp12ConnectionStatus() == false) && (count%50 == 0))	LED1 = !LED1;	//无网络连接时慢闪
		if((Get_Esp12ConnectionStatus() == THREE) && (count%10 == 0))	LED1 = !LED1;	//用户配网时快闪
		
		if(Usart3.RecFlag == true)	//asr
		{
			//Usart1_SendPackage(Usart3.RxBuff, Usart3.RecLen);
			BatchedAction(IdentifyResults(Usart3.RxBuff));	//处理语音指令
			Usart3.RecFlag = false;
			Usart3.RecLen = 0;
			memset(Usart3.RxBuff, 0, sizeof(Usart3.RxBuff));
		}
		
		if(Uart4.RecFlag == true)
		{
			Usart1_SendPackage(Uart4.RxBuff, Uart4.RecLen);
			if(Uart4.RxBuff[0] == 0x68 && Uart4.RxBuff[1] == 0x7c && Uart4.RxBuff[2] == 0x00 && Uart4.RxBuff[3] == 0x00 && Uart4.RxBuff[4] == 0x22)
			{
				//收到的数据是外部学习得到的编码
			}
			Uart4.RecFlag = false;
			Uart4.RecLen = 0;
			memset(Uart4.RxBuff, 0, sizeof(Uart4.RxBuff));
		}

		vTaskDelay(2);		//一个时钟节拍5ms，5*2=10ms
	}
}

void PlayMusic_Task(void *pvParameters)
{
	while(1)
	{
		if(ulTaskNotifyTake(pdTRUE, portMAX_DELAY) == pdTRUE)		
			vTaskDelay(10);      	
		else	vTaskDelay(10); 
			   
		status_dev.PlayState = PLAY_CLEAR;		//清除播放状态
		Audio_MusicPlay();
		//因为播放和暂停是同一条指令触发的，所以会出现已经播放了又发了一次信号，当停止播放时就会读到该信号，导致此任务继续执行
		//所以在停止播放的时候还要再获取一次信号量，这次获取不确定有没有，所以等待一个时钟节拍后超时退出
		ulTaskNotifyTake(pdTRUE, 1);	
	}
}

void LvglGui_Task(void *pvParameters)
{
	while(1)
	{
		lv_task_handler();
		vTaskDelay(1); 
	}
}

