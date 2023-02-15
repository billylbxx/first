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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//配置中断分组，只配置一次
	Systick_Inti(168);			//系统滴答初始化
	Usart1_Init(115200);		//初始化USART1，波特率为115200
	Key_Init();			//按键端口初始化
	w25q64_Init();
	lcd_Init();
//	LCD_FLASH_SHOW_CHINESE_STR(100,100,"平安喝着", LCD_BLACK,LCD_WHITE,16);
	/*FATFS测试*/
	//1. 注册
	res = f_mount(&fs,"0",1);
	if(res ==FR_OK)
	{
		printf("SD卡注册成功\r\n");
	}else{
		printf("SD卡注册失败\r\n");
	}
	//2.打开或者创建
	res = f_open(&fp,"truth.txt",FA_CREATE_ALWAYS|FA_WRITE);
	if(res ==FR_OK)
	{
		printf("成功打开或创建文件\r\n");
	}else{
		printf("%d打开失败\r\n",res);
	}
	//3.写数据
	res = f_write(&fp,"第一条　中华人民共和国是工人阶级领导的、以工农联盟为基础的人民民主专政的社会主义国家。社会主义制度是中华人民共和国的根本制度。中国共产党领导是中国特色社会主义最本质的特征。禁止任何组织或者个人破坏社会主义制度。",3000,&bw);
	if(res ==FR_OK)
	{
		printf("成功写入%d个字节\r\n",bw);
	}else{
		printf("写入文件失败\r\n");
	}
	//4.关闭文件
	f_close(&fp); //关闭文件

	//5.以只读方式打开文件
	f_open(&fp,"truth.txt",FA_READ);
	int i=f_size(&fp);
	//6.读取文件
	res = f_read(&fp,readBuf,3000,&br);
	if(res ==FR_OK)
	{
		printf("成功读取%d个字节，读到的内容为:%s\r\n",br,readBuf);
		LCD_FLASH_SHOW_CHINESE_STR(0,0,readBuf, LCD_BLACK,LCD_WHITE,16);
	}else{
		printf("读取文件失败\r\n");
	}
	while(1)
	{

	}
}
