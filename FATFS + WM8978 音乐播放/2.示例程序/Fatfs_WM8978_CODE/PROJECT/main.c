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
	char* s1="0:/MUSIC/�ղ���.wav";
	char*s2="0:/MUSIC/���ԡ������� - ����.wav";
	char*	s3="0:/MUSIC/�ܽ��� - �����.wav";
	char* songs[3]={s1,s2,s3};
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�����жϷ��飬ֻ����һ��
	Systick_Inti(168);			//ϵͳ�δ��ʼ��
	Usart1_Init(115200);		//��ʼ��USART1��������Ϊ115200
	Led2Pwm_Init(100, 840);		//1khz��CCRԽ�󣬵�Խ��
	HollowPwm_Init(100, 840);	//1khz��CCRԽ�󣬿��ı�ת��Խ�죬ע�⣺������Ҫ��IOֱ�Ӹߵ͵�ƽ����
	SteeringPwm_Init(2000,840);	//50hz��ע�⣺������50hz
	Led_Init();			//LED�˿ڳ�ʼ�� 
	Beep_Init();		//�������˿ڳ�ʼ��
	Key_Init();			//�����˿ڳ�ʼ��
	Sterilize_Init();	//�̵����˿ڳ�ʼ�������Ƽ��ȣ�
	//Rgb_Init();			//��ʼ��RGB
	W25QXX_Init();		//��ʼ��W25Q64
	Lcd_Init();			//��ʼ��LCD
	Sht30_Init();		//��ʼ��SHT30
	Mlx90614_Init();	//��ʼ��MLX90614
	MemEx_Init();       //�ⲿSRAM��ʼ��
	MemIn_Init();	    //�ڲ�SRAM��ʼ��
	res = f_mount(&fs,"0",1);
	if(res ==FR_OK)
	{
		printf("SD��ע��ɹ�\r\n");
	}else{
		printf("SD��ע��ʧ��\r\n");
	}
	Music_Init();  //���ֲ��ų�ʼ��
	
//	SetServoAngle(0);		//ת�� 0���λ��
//	key = Scanf_Key(0);			//��ס���ⰴ����λ��������дģʽ
//	if(key)
//	{
//		USART1_W25QXX(0,64);	//��д��ϻ��Զ��˳���ģʽ
//	}
//	printf("The program starts running!\r\n");
//	LCD_ShowFlashString(0, 0, "�ǻ����� Intelligent Life��", LCD_RED, LCD_WHITE);	//�ַ����ɱ�����ģ��ÿ�һЩ
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

/*��ҵ
  1. �Լ�������ֲFATFS���������ֲ��ͬ�İ汾����R0.14a.
  2. ����FAFTFS�����ĺ������ܡ���f_lseek�ȣ��ο���Fatfs-0.11_APIʹ���ֲ�.pdf����
  3. ʵ��һ���򵥵ĵ����鹦�ܡ���һ���Ƚϴ��txt�ļ��ŵ�SD����Ȼ���ȡ��������ݣ�����LCD������ʾ��
     ���Խ��л�ҳ���ܣ�������
  4.��չ����bmp��ʽ��ͼƬ�ŵ�SD����Ȼ���ȡ������LCD������ʾ��---�ٶ�
*/



