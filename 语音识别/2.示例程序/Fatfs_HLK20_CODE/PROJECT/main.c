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
	printf("The program starts running!\r\n");
	LCD_ShowFlashString(0, 0, "�ǻ����� Intelligent Life��", LCD_RED, LCD_WHITE);	//�ַ����ɱ�����ģ��ÿ�һЩ
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
	}
}

/*��ҵ
1.ѭ������Ŀ¼�µ�����wav����.
2.��LCD������ʾ�����ŵĸ������ֺͽ��ȡ�
3.����ͨ�������л�������
4.��չ,��ʾ��������һ���ļ�����
 
*/



