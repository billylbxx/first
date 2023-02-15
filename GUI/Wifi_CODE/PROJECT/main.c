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
#include "tim7.h"
#include "lcd.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lvgl_app.h"

#include "gui_guider.h"
#include "events_init.h"
#include "guider_lv_conf.h"
lv_ui guider_ui;
OS_STK ptos[256];
void Task (void *p_arg)
{
   for (;;) 
	 {
       printf("haha\r\n");
   }
}

int main()
{	
	u8 i;
	FATFS fs;
	FRESULT res;
	unsigned char key = 0;
	char *sp;
	u16 count = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//配置中断分组，只配置一次
	Systick_Inti(168);			//系统滴答初始化
	Usart1_Init(115200);		//初始化USART1，波特率为115200
//	Led2Pwm_Init(100, 840);		//1khz，CCR越大，灯越亮
//	HollowPwm_Init(100, 840);	//1khz，CCR越大，空心杯转的越快，注意：尽量不要用IO直接高低电平驱动
//	SteeringPwm_Init(2000,840);	//50hz，注意：必须是50hz
//	Led_Init();			//LED端口初始化 
//	Beep_Init();		//蜂鸣器端口初始化
//	Key_Init();			//按键端口初始化
//	Sterilize_Init();	//继电器端口初始化（控制加热）
//	Rgb_Init();			//初始化RGB	
//	Sht30_Init();		//初始化SHT30
//	Mlx90614_Init();	//初始化MLX90614
//	MemEx_Init();  //外部SRAM初始化
//	MemIn_Init();	 //内部SRAM初始化
//	Usart3_Init(115200);
//	Usart2_Init(115200);


	W25QXX_Init();		//初始化W25Q64
	Lcd_Init();			//初始化LCD
	Timer7_Init(840,100);
	lv_init();						// 初始化lvgl
	lv_port_disp_init();  			// 显示初始化
//	Lvgl_Lable_Demo();
	setup_ui(&guider_ui);
	events_init(&guider_ui);

if(Scanf_Key(0))
{
	USART1_W25QXX(259,256);	//烧写完毕会自动退出该模式，偏移0x2000等于偏移8192字节
}
	
	printf("The program starts running!\r\n");
  while(1)
	{     
		lv_task_handler();
		Delay_Ms(1);
	}
}




