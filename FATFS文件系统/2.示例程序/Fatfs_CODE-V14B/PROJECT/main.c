#include "led.h"
#include "key.h"
#include "systick.h"
#include "usart1.h"
#include "sd_driver.h"
#include "ff.h"
#include "W25QXX.h"
#include "SPI.h"
#include <stdlib.h>
int main()
{	
	FATFS fs;
	FRESULT res;
	FIL fp;
	UINT bw;
	UINT br;
	char* readBuf;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�����жϷ��飬ֻ����һ��
	Systick_Inti(168);			//ϵͳ�δ��ʼ��
	Usart1_Init(115200);		//��ʼ��USART1��������Ϊ115200
	Key_Init();			//�����˿ڳ�ʼ��
	w25q64_Init();
	lcd_Init();
//	LCD_FLASH_SHOW_CHINESE_STR(100,100,"ƽ������", LCD_BLACK,LCD_WHITE,16);
	/*FATFS����*/
	//1. ע��
	res = f_mount(&fs,"0",1);
	if(res ==FR_OK)
	{
		printf("SD��ע��ɹ�\r\n");
	}else{
		printf("SD��ע��ʧ��\r\n");
	}
	//2.�򿪻��ߴ���
	res = f_open(&fp,"truth.txt",FA_CREATE_ALWAYS|FA_WRITE);
	if(res ==FR_OK)
	{
		printf("�ɹ��򿪻򴴽��ļ�\r\n");
	}else{
		printf("%d��ʧ��\r\n",res);
	}
	//3.д����
	res = f_write(&fp,"��һ�����л����񹲺͹��ǹ��˽׼��쵼�ġ��Թ�ũ����Ϊ��������������ר�������������ҡ���������ƶ����л����񹲺͹��ĸ����ƶȡ��й��������쵼���й���ɫ���������ʵ���������ֹ�κ���֯���߸����ƻ���������ƶȡ�",3000,&bw);
	if(res ==FR_OK)
	{
		printf("�ɹ�д��%d���ֽ�\r\n",bw);
	}else{
		printf("д���ļ�ʧ��\r\n");
	}
	//4.�ر��ļ�
	f_close(&fp); //�ر��ļ�

	//5.��ֻ����ʽ���ļ�
	f_open(&fp,"truth.txt",FA_READ);
	int i=f_size(&fp);
	//6.��ȡ�ļ�
	res = f_read(&fp,readBuf,3000,&br);
	if(res ==FR_OK)
	{
		printf("�ɹ���ȡ%d���ֽڣ�����������Ϊ:%s\r\n",br,readBuf);
		LCD_FLASH_SHOW_CHINESE_STR(0,0,readBuf, LCD_BLACK,LCD_WHITE,16);
	}else{
		printf("��ȡ�ļ�ʧ��\r\n");
	}
	while(1)
	{

	}
}
