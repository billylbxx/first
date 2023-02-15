#include "app_task.h"

FATFS fp;
int main()
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	//FreeRTOS中断配置没有处理亚优先级，所以只能配组四
	Systick_Init(168);			//系统滴答初始化
	Usart1_Init(115200);		//初始化USART1，波特率为115200
	Usart2_Init(115200);		//初始化USART2，波特率为115200
	Usart3_Init(115200);		//初始化USART3，波特率为115200
	Uart4_Init(115200);			//初始化UART4，波特率为115200
	Led2Pwm_Init(100, 840);		//1khz，CCR越大，灯越亮
	HollowPwm_Init(100, 840);	//1khz，CCR越大，空心杯转的越快，注意：尽量不要用IO直接高低电平驱动
	SteeringPwm_Init(2000,840);	//50hz，注意：必须是50hz
	Led_Init();			//LED端口初始化 
	Beep_Init();		//蜂鸣器端口初始化

	Key_Init();			//按键端口初始化
	Sterilize_Init();	//继电器端口初始化（控制加热）
	Rgb_Init();			//初始化RGB
	W25QXX_Init();		//初始化W25Q64
	Lcd_Init();			//初始化LCD
	Sht30_Init();		//初始化SHT30
	Mlx90614_Init();	//初始化MLX90614
	Adc_Init();			//初始化ADC端口
 	//Rtc_Init();			//初始化RTC
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);
	WM8978_Init();				//初始化WM8978
	WM8978_HPvol_Set(0, 0);		//耳机音量设置
	WM8978_SPKvol_Set(20);		//喇叭音量设置
	WM8978_ADDA_Cfg(1, 0);		//开启DAC
	WM8978_Input_Cfg(0, 0, 0);	//关闭输入通道
	WM8978_Output_Cfg(1, 0);	//开启DAC输出   
	WM8978_I2S_Cfg(2, 0);		//飞利浦标准,16位数据长度
	I2S2_Init(I2S_Standard_Phillips, I2S_Mode_MasterTx, I2S_CPOL_Low, I2S_DataFormat_16bextended);	//飞利浦标准,主机发送,时钟低电平有效,16位扩展帧长度
	I2S2_SampleRate_Set(44100);	//设置采样率
	I2S2_TX_DMA_Init(NULL, NULL, WAV_I2S_TX_DMA_BUFSIZE/2); 				//配置TX DMA
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	f_mount(&fp, "0:", 1);
	
//	SetServoAngle(0);			//转回 0°的位置
			
	if(Scanf_Key(0))				//按住任意按键复位将进入烧写模式
	{
		USART1_W25QXX(0,64);	//烧写完毕会自动退出该模式
	}
	
	printf("The program starts running!\r\n");
	
	FreeRTOS_Init();			//初始化
	
	while(1)
	{
	
	}
}

