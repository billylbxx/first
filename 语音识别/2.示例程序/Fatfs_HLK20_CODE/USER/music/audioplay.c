#include "app_task.h"
#include "revert.h"

__audiodev audiodev;		//音乐播放控制器  
/*********************************************************************************************************
* 函 数 名 : Audio_Start
* 功能说明 : 开始播放音频
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 实际上就是启动dma传输
*********************************************************************************************************/ 
void Audio_Start(void)
{
	audiodev.status=3<<0;	//开始播放+非暂停
	I2S_Play_Start();
} 
/*********************************************************************************************************
* 函 数 名 : Audio_Stop
* 功能说明 : 关闭音频播放
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 实际上就是关闭dma传输
*********************************************************************************************************/ 
void Audio_Stop(void)
{
	audiodev.status=0;
	I2S_Play_Stop();
}  
/*********************************************************************************************************
* 函 数 名 : Audio_GetTnum
* 功能说明 : 得到指定路径下的文件数量
* 形    参 : path：路径名
* 返 回 值 : 总有效文件数量
* 备    注 : 无
*********************************************************************************************************/ 
unsigned short Audio_GetTnum(unsigned char *path)
{	  
	unsigned char 	res  = 0;
	unsigned short	rval = 0;
	unsigned char *fn   = NULL; 			 
 	DIR tdir;	 				//临时目录
	FILINFO tfileinfo;			//临时文件信息		
				   			     
    res = f_opendir(&tdir, (const TCHAR *)path); 				//打开目录
  	tfileinfo.fsize =FF_MAX_LFN*2+1;							//长文件名最大长度
	//tfileinfo.fname = mymalloc(SRAMIN, tfileinfo.fsize);		//为长文件缓存区分配内存
	if(res==FR_OK)
	{
		while(1)	//查询总的有效文件数
		{
	        res = f_readdir(&tdir, &tfileinfo);       				//读取目录下的一个文件
	        if(res!=FR_OK || tfileinfo.fname[0]==0) break;			//错误了/到末尾了,退出		  
     		fn = (unsigned char *)(*tfileinfo.fname?tfileinfo.fname:tfileinfo.fname);			 
			res = f_typetell(fn);	
			if((res&0XF0) == 0X40)	//取高四位,看看是不是音乐文件	
			{
				rval++;				//有效文件数增加1
			}	    
		}  
	} 
	myfree(SRAMIN, tfileinfo.fname);
	return rval;
}
/*********************************************************************************************************
* 函 数 名 : Audio_IndexShow
* 功能说明 : 显示曲目当前索引
* 形    参 : fn: 歌名，index：当前索引，total：总文件数量
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Audio_IndexShow(unsigned char *fn, unsigned short index, unsigned short total)
{
//	memset(Usart1.RxBuff, 0, sizeof(Usart1.RxBuff));
	taskENTER_CRITICAL();
	LCD_ShowFlashString(5, 190, "                            ",  0xbf3f, LCD_WHITE);
	LCD_ShowFlashString(5, 190, fn, 0xbf3f, LCD_WHITE);
	taskEXIT_CRITICAL();
//	sprintf((char *)Usart1.RxBuff, "当前播放:%d 总曲目:%d", index, total);
//	LCD_ShowFlashString(0, 160, Usart1.RxBuff, LCD_RED, LCD_WHITE);		  	  
}
/*********************************************************************************************************
* 函 数 名 : Audio_MsgShow
* 功能说明 : 显示播放时间
* 形    参 : totsec：音频文件总时间长度，cursec：当前播放时间
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Audio_MsgShow(unsigned int totsec, unsigned int cursec)
{	
	static u16 playtime=0XFFFF;			//播放时间标记	  
	
	if(playtime != cursec)			//需要更新显示时间
	{
		playtime=cursec;
		lv_bar_set_value(guider_ui.screen_barMusic, (double)cursec/(double)totsec*100.0, LV_ANIM_ON);
//		sprintf((char *)Usart1.RxBuff, "%02d:%02d / %02d:%02d", playtime/60,playtime%60, totsec/60, totsec%60);
//		//LCD_ShowFlashString(0, 180, Usart1.RxBuff, LCD_RED, LCD_WHITE);
	} 		
}
/*********************************************************************************************************
* 函 数 名 : Audio_MusicPlay
* 功能说明 : 历遍指定路径，并按顺序播放改路径下的wav音频文件
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Audio_MusicPlay(void)
{
 	DIR wavdir;	 							//目录
	FILINFO wavfileinfo;					//文件信息
	unsigned char  *fn 	= NULL;   		//长文件名
	unsigned char  *pname 	= NULL;			//带路径的文件名
	unsigned short *wavindextbl = NULL;	//音乐索引表
	unsigned char  key 	= 0;			//键值
	unsigned char  res 	= 0;
	unsigned short totwavnum 	= 0; 		//音乐文件总数
	unsigned short curindex 	= 0;		//图片当前索引
 	unsigned short temp 		= 0;
	
 	while(f_opendir(&wavdir, "0:/MUSIC"))	//打开音乐文件夹
 	{	    
		printf("MUSIC文件夹错误!\r\n");
		Delay_Ms(200);				  			  
	} 									  
	totwavnum = Audio_GetTnum("0:/MUSIC");  //得到总有效文件数
  	while(totwavnum == NULL)				//音乐文件总数为0		
 	{	    
		printf("没有音乐文件!\r\n");
		Delay_Ms(200);			  
	}										   
	wavfileinfo.fsize = FF_MAX_LFN * 2 + 1;									//长文件名最大长度
	//wavfileinfo.fname = mymalloc(SRAMIN, wavfileinfo.fsize);				//为长文件缓存区分配内存
 	pname = mymalloc(SRAMIN, wavfileinfo.fsize);							//为带路径的文件名分配内存
 	wavindextbl = mymalloc(SRAMIN, 2*totwavnum);							//申请2*totwavnum个字节的内存,用于存放音乐文件索引
 	while(pname==NULL || wavindextbl==NULL)									//内存分配出错
 	{	    
		printf("内存分配失败!\r\n");
		Delay_Ms(200);				  			  
	}  	 

    res = f_opendir(&wavdir, "0:/MUSIC"); //打开目录
	if(res == FR_OK)
	{
		curindex = 0;	//当前索引为0
		while(1)		//全部查询一遍
		{
			temp = wavdir.dptr;								//记录当前index
	        res = f_readdir(&wavdir, &wavfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK || wavfileinfo.fname[0]==0)	break;	//错误了/到末尾了,退出		  
     		fn = (unsigned char *)(*wavfileinfo.fname?wavfileinfo.fname:wavfileinfo.fname);			 
			res = f_typetell(fn);	
			if((res&0XF0) == 0X40)				//取高四位,看看是不是音乐文件	
			{
				wavindextbl[curindex] = temp;	//记录索引
				curindex++;
			}	    
		} 
	}   
	curindex = 0;											//从0开始显示
	res = f_opendir(&wavdir, (const TCHAR *)"0:/MUSIC"); 	//打开目录
	while(res == FR_OK)	//打开成功
	{	
		dir_sdi(&wavdir, wavindextbl[curindex]);			//改变当前目录索引	   
		res = f_readdir(&wavdir,&wavfileinfo);       		//读取目录下的一个文件
		if(res!=FR_OK || wavfileinfo.fname[0]==0) break;	//错误了/到末尾了,退出
		fn = (unsigned char *)(*wavfileinfo.fname?wavfileinfo.fname:wavfileinfo.fname);			 
		strcpy((char*)pname, "0:/MUSIC/");					//复制路径(目录)
		strcat((char*)pname, (const char*)fn);  			//将文件名接在后面
		//printf("%s\r\n", fn);								//显示歌曲名字 
		Audio_IndexShow(fn, curindex+1, totwavnum);	//显示歌曲名称、总曲数、当前曲数
		key = Audio_PlaySong(pname); 			 			//播放这个音频文件
		if(key == 1)				//上一曲
		{
			if(curindex)	curindex--;
			else 			curindex = totwavnum-1;
 		}else if(key==0 || key==2)	//下一曲
		{
			curindex++;		   	
			if(curindex >= totwavnum)	curindex=0;	//到末尾的时候,自动从头开始
				
 		}else		
		{	
			break;	//产生了错误 	 
		}
	} 		

	myfree(SRAMIN, wavfileinfo.fname);		//释放内存			    
	myfree(SRAMIN, pname);					//释放内存			    
	myfree(SRAMIN, wavindextbl);			//释放内存	 
} 
/*********************************************************************************************************
* 函 数 名 : Audio_PlaySong
* 功能说明 : 播放某个音频文件
* 形    参 : fname：文件路径+文件名
* 返 回 值 : 0：下一首（自然播放），1：上一首，2：下一首，其他：停止播放或者发生了错误
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char Audio_PlaySong(unsigned char *fname)
{
	unsigned char  res = f_typetell(fname);  

	switch(res)
	{
		case T_WAV:
			res = Wav_PlaySong(fname);
			break;
		default:	//其他文件,自动跳转到下一曲
			printf("can't play:%s\r\n",fname);
			res = 0;
			break;
	}
	return res;
}
/*********************************************************************************************************
* 函 数 名 : BatchedAction
* 功能说明 : 处理语音识别结果
* 形    参 : action：识别到的指令编码
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void BatchedAction(unsigned char action)
{
	static char speed = 3;
	static unsigned char state = 0;
	
	if(action > 0)
	{
		WM8978_SPKvol_Set(VOL);
		switch(action)
		{
			case 0x00: printf("Warning: Identifying Sentence Error\r\n");	break;
			case 0x01:	
				SetHollowSpeed(0);	
				Wav_PlayRevert("0:/prompt/好的已为您关闭风扇.wav");	
				break;
			case 0x02:	
				SetHollowSpeed(speed);	
				Wav_PlayRevert("0:/prompt/好的已为您打开风扇.wav");	
				break;
			case 0x03: /*定时打开风扇*/	break;
			case 0x04: /*定时关闭风扇*/	break;
			case 0x05:	
				SetHollowSpeed(3);	
				Wav_PlayRevert("0:/prompt/好的风扇已为您调到一档.wav");	
				break;
			case 0x06:	
				SetHollowSpeed(4);	
				Wav_PlayRevert("0:/prompt/好的风扇已为您调到二档.wav");	
				break;
			case 0x07:	
				SetHollowSpeed(5);	
				Wav_PlayRevert("0:/prompt/好的风扇已为您调到三档.wav");	
				break;
			case 0x08:	
				SetHollowSpeed(6);	
				Wav_PlayRevert("0:/prompt/好的风扇已为您调到四档.wav");	
				break;
			case 0x09:	
				speed += 1;
				if(speed >= 6)	speed = 6;
					SetHollowSpeed(speed);
				Wav_PlayRevert("0:/prompt/风速已为您增加百分之十.wav");	
				break;
			case 0x10:
				speed -= 1;
				if(speed <= 0)	speed = 0;
					SetHollowSpeed(speed);
				Wav_PlayRevert("0:/prompt/风速已为您减小百分之十.wav");	
				break;
			case 0x11:/*开启抽风*/	break;
			case 0x12:/*开始吹风*/	break;
			
			case 0x13:	//打开所有的灯
				LED1 = 0;	LED2 = 0;	LED3 = 0;
				W2812_WriteColorData(RGB_WHITE);
				W2812_WriteColorData(RGB_WHITE);
				lv_led_on(guider_ui.screen_ledBedroom);
				lv_led_on(guider_ui.screen_ledKitchen);
				lv_led_on(guider_ui.screen_ledSaloon);
				Wav_PlayRevert("0:/prompt/好的已为您打开所有的灯.wav");	
				break;
			case 0x14:	//关闭所有的灯
				LED1 = 1;	LED2 = 1;	LED3 = 1;
				W2812_WriteColorData(RGB_BLACK);
				W2812_WriteColorData(RGB_BLACK);
				lv_led_off(guider_ui.screen_ledBedroom);
				lv_led_off(guider_ui.screen_ledKitchen);
				lv_led_off(guider_ui.screen_ledSaloon);
				Wav_PlayRevert("0:/prompt/好的已为您关闭所有的灯.wav");	
				break;
			case 0x15:/*定时开灯*/	break;
			case 0x16:/*定时关灯*/	break;
			case 0x17:	//卧室的灯
				LED1 = 0;	
				lv_led_on(guider_ui.screen_ledBedroom);
				Wav_PlayRevert("0:/prompt/好的已为您打开.wav");	
				break;	
			case 0x18:	//卧室的灯
				LED1 = 1;
				lv_led_off(guider_ui.screen_ledBedroom);
				Wav_PlayRevert("0:/prompt/好的已为您关闭.wav");
				break;	
			case 0x19:	
				LED2 = 0;	
				lv_led_on(guider_ui.screen_ledSaloon);
				Wav_PlayRevert("0:/prompt/好的已为您打开.wav");	
				break;	//客厅的灯
			case 0x20:	
				LED2 = 1;	
				lv_led_off(guider_ui.screen_ledSaloon);
				Wav_PlayRevert("0:/prompt/好的已为您关闭.wav");
				break;	//客厅的灯
			case 0x21:	
				LED3 = 0;
				lv_led_on(guider_ui.screen_ledKitchen);
				Wav_PlayRevert("0:/prompt/好的已为您打开.wav");
				break;	//厨房的灯
			case 0x22:	
				LED3 = 1;
				lv_led_off(guider_ui.screen_ledKitchen);
				Wav_PlayRevert("0:/prompt/好的已为您关闭.wav");
				break;	//厨房的灯
			case 0x23:	//打开浴室的灯
				W2812_WriteColorData(RGB_BLACK);
				W2812_WriteColorData(RGB_WHITE);
				Wav_PlayRevert("0:/prompt/好的已为您打开.wav");
				break;
			case 0x24:	//关闭浴室的灯
				W2812_WriteColorData(RGB_BLACK);
				W2812_WriteColorData(RGB_BLACK);
				Wav_PlayRevert("0:/prompt/好的已为您关闭.wav");
				break;	
			case 0x25:	//打开台灯
				W2812_WriteColorData(RGB_WHITE);	
				W2812_WriteColorData(RGB_BLACK);
				Wav_PlayRevert("0:/prompt/好的已为您打开.wav");
				break;	
			case 0x26:	//关闭台灯
				W2812_WriteColorData(RGB_BLACK);
				W2812_WriteColorData(RGB_BLACK);
				Wav_PlayRevert("0:/prompt/好的已为您关闭.wav");
				break;	
			case 0x27:/*灯调到最亮*/	break;
			case 0x28:/*灯调到最暗*/	break;
			case 0x29:/*灯亮一点*/		break;
			case 0x30:/*灯暗一点*/		break;
			case 0x31:/*打开吸顶灯*/	break;
			case 0x32:/*关闭吸顶灯*/	break;		
			case 0x33:/*温暖模式*/		break;
			case 0x34:/*音乐模式*/		break;
			case 0x35:/*睡眠模式*/		break;
			
			case 0x36:/*打开空调*/	
				Infrared_SendCmd(IR_IN_SEND, 0);
				Wav_PlayRevert("0:/prompt/好的已为您打开.wav");
				break;
			case 0x37:/*关闭空调*/	
				Infrared_SendCmd(IR_IN_SEND, 1);
				Wav_PlayRevert("0:/prompt/好的已为您关闭.wav");
				break;
			case 0x38:/*制冷模式*/	break;
			case 0x39:/*制热模式*/	break;
			case 0x40:/*升高温度*/	
				Infrared_SendCmd(IR_IN_SEND, 2);
				Wav_PlayRevert("0:/prompt/好的已为你设置.wav");
				break;
			case 0x41:/*降低温度*/	
				Infrared_SendCmd(IR_IN_SEND, 3);
				Wav_PlayRevert("0:/prompt/好的已为你设置.wav");
				break;
			case 0x42:/*上下摆风*/	break;
			case 0x43:/*左右摆风*/	break;
			case 0x44:/*除湿模式*/	break;
			case 0x45:/*自动模式*/	break;
			
			case 0x46:	//打开杀菌
				STERILIZE = 1;
				Wav_PlayRevert("0:/prompt/好的已为您打开.wav");
				break;	
			case 0x47:	//关闭杀菌
				STERILIZE = 0;	
				Wav_PlayRevert("0:/prompt/好的已为您关闭.wav");
				break;	
			case 0x48:	
				STERILIZE = 1;	
				Wav_PlayRevert("0:/prompt/好的已为您打开.wav");
				break;	//打开烘干
			case 0x49:	
				STERILIZE = 0;	
				Wav_PlayRevert("0:/prompt/好的已为您关闭.wav");
			break;	//关闭烘干
			
			case 0x50:	//打开窗户
				SetServoAngle(140);
				lv_led_on(guider_ui.screen_ledFinestra);
				Wav_PlayRevert("0:/prompt/好的已为您打开.wav");
				break;	
			case 0x51:	
				SetServoAngle(0);	
				lv_led_off(guider_ui.screen_ledFinestra);
				Wav_PlayRevert("0:/prompt/好的已为您关闭.wav");
				break;	//关闭窗户
			case 0x52:	
				SetServoAngle(90);	
				Wav_PlayRevert("0:/prompt/好的已为您打开.wav");
				break;	//半开窗户
			case 0x53:	//开门
				SetServoAngle(180);	
				Wav_PlayRevert("0:/prompt/好的已为您打开.wav");
				break;	
			case 0x54:	//关门
				SetServoAngle(0);	
				Wav_PlayRevert("0:/prompt/好的已为您关闭.wav");
				break;	
			/*待完善...*/
			
			case 0x55:	//播放音乐
				Wav_PlayRevert("0:/prompt/好的已为您播放.wav");
				WM8978_SPKvol_Set(VOL);
				status_dev.PlayState = PLAY_PLAY;
				status_dev.PlayMode = true;
				WM8978_SPKvol_Set(0);
				xTaskNotify(PlayMusic_Handler, 1, eSetValueWithOverwrite);
				break;
			case 0x56:	//暂停播放
				status_dev.PlayState = PLAY_PAUSE;	
				Wav_PlayRevert("0:/prompt/好的已为您暂停.wav");
				WM8978_SPKvol_Set(0);
				break;	
			case 0x57:	//停止播放
				status_dev.PlayState = PLAY_STOP;	
				status_dev.PlayMode = false;
				Wav_PlayRevert("0:/prompt/好的已为您关闭.wav");
				WM8978_SPKvol_Set(0);
				break;	
			case 0x58:	//上一首	
				status_dev.PlayState = PLAY_PREVIOUS;	
				Wav_PlayRevert("0:/prompt/好的，已为您切换.wav");
				WM8978_SPKvol_Set(0);
				break;	
			case 0x59:	//下一首
				status_dev.PlayState = PLAY_NEXT;	
				Wav_PlayRevert("0:/prompt/好的，已为您切换.wav");
				WM8978_SPKvol_Set(0);
				break;	
			
			case 0x60:	//打开报警
				BEEP = 1;	
				lv_led_on(guider_ui.screen_ledAlarms);
				Wav_PlayRevert("0:/prompt/好的已为您打开.wav");
				break;	
			case 0x61:	//关闭报警
				BEEP = 0;	
				lv_led_off(guider_ui.screen_ledAlarms);
				Wav_PlayRevert("0:/prompt/好的已为您关闭.wav");
				break;	
			
			case 0x62:	/*开启体温测量*/	
			{	
				float temperature = 0.0;
				BaseType_t xHigherPriorityTaskWoken;
				
				Mlx90614_ReadTemperature(&temperature);	//把结果发送到消息队列
				xQueueSendFromISR(MessageQueue_Tem, &temperature, &xHigherPriorityTaskWoken);	
				Wav_PlayRevert("0:/prompt/好的已为您打开.wav");
			}	break;
			
			case 0x63:	//减小音量，此部分固件依旧未实现
				status_dev.volume -= 10; 
				if(status_dev.volume <= 10)	status_dev.volume = 10; 
				WM8978_SPKvol_Set(status_dev.volume);
				break;	
			case 0x64:	//增大音量
				status_dev.volume += 10;	
				if(status_dev.volume >= 60)	status_dev.volume = 63; 
				WM8978_SPKvol_Set(status_dev.volume);
				break;	
			case 0x65:	//最小音量
				status_dev.volume = 10;	
				WM8978_SPKvol_Set(status_dev.volume);
				break;	
			case 0x66:	//最大音量
				status_dev.volume = 63;	
				WM8978_SPKvol_Set(status_dev.volume);
				break;	
			
			case 0xaa:	//唤醒
			{	
				switch(state++)
				{
					case 0:	Wav_PlayRevert("0:/prompt/嗨我在呢.wav");						break;
					case 1:	Wav_PlayRevert("0:/prompt/嗯我来了.wav");						break;	
					case 2:	Wav_PlayRevert("0:/prompt/有什么可以帮助您.wav");	state = 0;	break;	
				}
			}	break;
			case 0xab:	
				Wav_PlayRevert("0:/prompt/有需要再叫我.wav");	
				break;
			case 0xac:	
				Wav_PlayRevert("0:/prompt/我先退下了.wav");	
				break;
			default:
				printf("Warning: There is no such command\r\n");
			break;
		}
	}
}
/*********************************************************************************************************
* 函 数 名 : IdentifyResults
* 功能说明 : 语音模块识别结果
* 形    参 : buf：存放结果的缓冲区
* 返 回 值 : 提取到的编码
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char IdentifyResults(unsigned char *buf)
{
	unsigned char cnt = 0;
	
	if(buf[0] == 0x01)				//刚上电第一次识别会收到一个0x01
		cnt = 1;

	if(buf[cnt++] == 0x4F)			//0, 1
	{
		if(buf[cnt++] == 0x00)		//1, 2
		{
			if(buf[cnt+1] ==  0xF4)	//3, 4
			{
				return buf[cnt];	//2, 3
			}else return 0;
		}else return 0;
	}else return 0;
}



