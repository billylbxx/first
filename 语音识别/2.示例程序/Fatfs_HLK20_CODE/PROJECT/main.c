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
int main()
{	
	u8 i;
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
	printf("The program starts running!\r\n");
	LCD_ShowFlashString(0, 0, "智慧生活 Intelligent Life！", LCD_RED, LCD_WHITE);	//字符换成本地字模会好看一些
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
	}
}

/*作业
1.循环播放目录下的所有wav歌曲.
2.在LCD屏上显示所播放的歌曲名字和进度。
3.可以通过按键切换歌曲。
4.扩展,显示歌曲（是一个文件，）
 
*/



