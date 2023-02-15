#include "app_task.h"

FATFS fp;
int main()
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	//FreeRTOS�ж�����û�д��������ȼ�������ֻ��������
	Systick_Init(168);			//ϵͳ�δ��ʼ��
	Usart1_Init(115200);		//��ʼ��USART1��������Ϊ115200
	Usart2_Init(115200);		//��ʼ��USART2��������Ϊ115200
	Usart3_Init(115200);		//��ʼ��USART3��������Ϊ115200
	Uart4_Init(115200);			//��ʼ��UART4��������Ϊ115200
	Led2Pwm_Init(100, 840);		//1khz��CCRԽ�󣬵�Խ��
	HollowPwm_Init(100, 840);	//1khz��CCRԽ�󣬿��ı�ת��Խ�죬ע�⣺������Ҫ��IOֱ�Ӹߵ͵�ƽ����
	SteeringPwm_Init(2000,840);	//50hz��ע�⣺������50hz
	Led_Init();			//LED�˿ڳ�ʼ�� 
	Beep_Init();		//�������˿ڳ�ʼ��

	Key_Init();			//�����˿ڳ�ʼ��
	Sterilize_Init();	//�̵����˿ڳ�ʼ�������Ƽ��ȣ�
	Rgb_Init();			//��ʼ��RGB
	W25QXX_Init();		//��ʼ��W25Q64
	Lcd_Init();			//��ʼ��LCD
	Sht30_Init();		//��ʼ��SHT30
	Mlx90614_Init();	//��ʼ��MLX90614
	Adc_Init();			//��ʼ��ADC�˿�
 	//Rtc_Init();			//��ʼ��RTC
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);
	WM8978_Init();				//��ʼ��WM8978
	WM8978_HPvol_Set(0, 0);		//������������
	WM8978_SPKvol_Set(20);		//������������
	WM8978_ADDA_Cfg(1, 0);		//����DAC
	WM8978_Input_Cfg(0, 0, 0);	//�ر�����ͨ��
	WM8978_Output_Cfg(1, 0);	//����DAC���   
	WM8978_I2S_Cfg(2, 0);		//�����ֱ�׼,16λ���ݳ���
	I2S2_Init(I2S_Standard_Phillips, I2S_Mode_MasterTx, I2S_CPOL_Low, I2S_DataFormat_16bextended);	//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,16λ��չ֡����
	I2S2_SampleRate_Set(44100);	//���ò�����
	I2S2_TX_DMA_Init(NULL, NULL, WAV_I2S_TX_DMA_BUFSIZE/2); 				//����TX DMA
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ�� 
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ�� 
	f_mount(&fp, "0:", 1);
	
//	SetServoAngle(0);			//ת�� 0���λ��
			
	if(Scanf_Key(0))				//��ס���ⰴ����λ��������дģʽ
	{
		USART1_W25QXX(0,64);	//��д��ϻ��Զ��˳���ģʽ
	}
	
	printf("The program starts running!\r\n");
	
	FreeRTOS_Init();			//��ʼ��
	
	while(1)
	{
	
	}
}

