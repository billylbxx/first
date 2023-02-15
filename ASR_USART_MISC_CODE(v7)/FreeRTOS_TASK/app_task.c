#include "app_task.h"

TaskHandle_t GatherDataTask_Handler;	 	//������
TaskHandle_t VoiceRecognition_Handler;		//������
TaskHandle_t PlayMusic_Handler;				//������
TaskHandle_t LvglGui_Handler;				//������

QueueHandle_t MessageQueue_Tem;		//������������Ϣ����

lv_ui guider_ui;

void FreeRTOS_Init(void)
{
	 taskENTER_CRITICAL();           //�����ٽ���
	
	//�������ݲɼ����������
    xTaskCreate((TaskFunction_t )GatherData_Task,     	
                (const char*    )"GatherData_Task",   	
                (uint16_t       )GatherData_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )GatherData_TASK_PRIO,	
                (TaskHandle_t*  )&GatherDataTask_Handler);
	//��������ʶ���������
	xTaskCreate((TaskFunction_t )VoiceRecognition_Task,     	
                (const char*    )"VoiceRecognition_Task",   	
                (uint16_t       )VoiceRecognition_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )VoiceRecognition_TASK_PRIO,	
                (TaskHandle_t*  )&VoiceRecognition_Handler);     									
	//�����������ֵ�����
	xTaskCreate((TaskFunction_t )PlayMusic_Task,     	
                (const char*    )"PlayMusic_Task",   	
                (uint16_t       )PlayMusic_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )PlayMusic_TASK_PRIO,	
                (TaskHandle_t*  )&PlayMusic_Handler);   
	//����GUI���������
	xTaskCreate((TaskFunction_t )LvglGui_Task,     	
                (const char*    )"LvglGui_Task",   	
                (uint16_t       )LvglGui_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LvglGui_TASK_PRIO,	
                (TaskHandle_t*  )&LvglGui_Handler);   
								
	MessageQueue_Tem = xQueueCreate(2, 5); 
	
	lv_init();						// ��ʼ��lvgl
	lv_port_disp_init();
 	LVGL_GUI_APP(&guider_ui);
				
	vTaskStartScheduler();          //�����������
	taskEXIT_CRITICAL();            //�˳��ٽ���
}

//�������ݲɼ���ʵʱ��ʾ����������������
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

			if(ConnectFlag == true)				//�����ƶ˲��ϱ�����
				PublishMessage((void *)&HT);
		}
		
		//������ڲ������֣���ʾ����
		if(status_dev.PlayMode == true)	
		{
			Audio_MsgShow(wavctrl.totsec, wavctrl.cursec);
		}
		
		//���յ���������������Ϣ
		if(xQueueReceiveFromISR(MessageQueue_Tem, &temperature, &xTaskWokenByReceive) == pdTRUE)	
		{
			if(temperature>=0 && temperature<=100)
			{
				lv_bar_set_value(guider_ui.screen_barFlame, temperature, LV_ANIM_ON);
			}
		}		
		
		vTaskDelay(2);		//һ��ʱ�ӽ���5ms��5*2=10ms
	}
}   
void VoiceRecognition_Task(void *pvParameters)
{
	unsigned short count = 0;
	
	while(1)
	{
		TaskKeyMsg_Handle();		//��������
		TaskEsp12Msg_Handle();	//���������
		
		if(++count >= 500)
			count = 0;
		
		if((Get_Esp12ConnectionStatus() == false) && (count%50 == 0))	LED1 = !LED1;	//����������ʱ����
		if((Get_Esp12ConnectionStatus() == THREE) && (count%10 == 0))	LED1 = !LED1;	//�û�����ʱ����
		
		if(Usart3.RecFlag == true)	//asr
		{
			//Usart1_SendPackage(Usart3.RxBuff, Usart3.RecLen);
			BatchedAction(IdentifyResults(Usart3.RxBuff));	//��������ָ��
			Usart3.RecFlag = false;
			Usart3.RecLen = 0;
			memset(Usart3.RxBuff, 0, sizeof(Usart3.RxBuff));
		}
		
		if(Uart4.RecFlag == true)
		{
			Usart1_SendPackage(Uart4.RxBuff, Uart4.RecLen);
			if(Uart4.RxBuff[0] == 0x68 && Uart4.RxBuff[1] == 0x7c && Uart4.RxBuff[2] == 0x00 && Uart4.RxBuff[3] == 0x00 && Uart4.RxBuff[4] == 0x22)
			{
				//�յ����������ⲿѧϰ�õ��ı���
			}
			Uart4.RecFlag = false;
			Uart4.RecLen = 0;
			memset(Uart4.RxBuff, 0, sizeof(Uart4.RxBuff));
		}

		vTaskDelay(2);		//һ��ʱ�ӽ���5ms��5*2=10ms
	}
}

void PlayMusic_Task(void *pvParameters)
{
	while(1)
	{
		if(ulTaskNotifyTake(pdTRUE, portMAX_DELAY) == pdTRUE)		
			vTaskDelay(10);      	
		else	vTaskDelay(10); 
			   
		status_dev.PlayState = PLAY_CLEAR;		//�������״̬
		Audio_MusicPlay();
		//��Ϊ���ź���ͣ��ͬһ��ָ����ģ����Ի�����Ѿ��������ַ���һ���źţ���ֹͣ����ʱ�ͻ�������źţ����´��������ִ��
		//������ֹͣ���ŵ�ʱ��Ҫ�ٻ�ȡһ���ź�������λ�ȡ��ȷ����û�У����Եȴ�һ��ʱ�ӽ��ĺ�ʱ�˳�
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

