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

int main()
{	
	FATFS fs;
	FRESULT res;
	unsigned char key = 0;
	char* s1="0:/MUSIC/日不落.wav";
	char*s2="0:/MUSIC/许嵩、何曼婷 - 素颜.wav";
	char*	s3="0:/MUSIC/周杰伦 - 龙卷风.wav";
	char* songs[3]={s1,s2,s3};
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//配置中断分组，只配置一次
	Systick_Inti(168);			//系统滴答初始化
	Usart1_Init(115200);		//初始化USART1，波特率为115200
	Led2Pwm_Init(100, 840);		//1khz，CCR越大，灯越亮
	HollowPwm_Init(100, 840);	//1khz，CCR越大，空心杯转的越快，注意：尽量不要用IO直接高低电平驱动
	SteeringPwm_Init(2000,840);	//50hz，注意：必须是50hz
	Led_Init();			//LED端口初始化 
	Beep_Init();		//蜂鸣器端口初始化
	Key_Init();			//按键端口初始化
	Sterilize_Init();	//继电器端口初始化（控制加热）
	//Rgb_Init();			//初始化RGB
	W25QXX_Init();		//初始化W25Q64
	Lcd_Init();			//初始化LCD
	Sht30_Init();		//初始化SHT30
	Mlx90614_Init();	//初始化MLX90614
	MemEx_Init();       //外部SRAM初始化
	MemIn_Init();	    //内部SRAM初始化
	res = f_mount(&fs,"0",1);
	if(res ==FR_OK)
	{
		printf("SD卡注册成功\r\n");
	}else{
		printf("SD卡注册失败\r\n");
	}
	Music_Init();  //音乐播放初始化
	
//	SetServoAngle(0);		//转回 0°的位置
//	key = Scanf_Key(0);			//按住任意按键复位将进入烧写模式
//	if(key)
//	{
//		USART1_W25QXX(0,64);	//烧写完毕会自动退出该模式
//	}
//	printf("The program starts running!\r\n");
//	LCD_ShowFlashString(0, 0, "智慧生活 Intelligent Life！", LCD_RED, LCD_WHITE);	//字符换成本地字模会好看一些
	int i=0,j=1;
	while(1)
	{	
		
					float *t=(float*)MemIn_Malloc(20);
					Mlx90614_ReadTemperature(t);
					printf("%lf\r\n",*t);
					Delay_Ms(700);
//				if(Scanf_Key(0)==3||Scanf_Key(0)==2||Scanf_Key(0)==1)
//		
//				{
//					LED1=~LED1;
//				}
//		i=Scanf_Key(0);
//		switch(i)                      
//		{
//			case 1:Music_Init();j--;if(j<0){j=2;}printf("%s\r\n",songs[j]);WAV_Play((u8*)songs[j]);break;
//			case 2:WAV_Stop();break;
//			case 3:Music_Init();j++;if(3<j){j=0;}printf("%s\r\n",songs[j]);WAV_Play((u8*)songs[j]);break;
//		}                                                   
	}
}

/*作业
  1. 自己动手移植FATFS，你可以移植不同的版本，如R0.14a.
  2. 调用FAFTFS其他的函数功能。如f_lseek等，参考《Fatfs-0.11_API使用手册.pdf》。
  3. 实现一个简单的电子书功能。把一个比较大的txt文件放到SD卡，然后读取里面的内容，并在LCD屏上显示，
     可以进行换页功能（按键）
  4.扩展：把bmp格式的图片放到SD卡，然后读取出来在LCD屏上显示。---百度
*/



