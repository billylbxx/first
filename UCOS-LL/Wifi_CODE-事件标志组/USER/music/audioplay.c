#include "app_task.h"
#include "revert.h"

__audiodev audiodev;		//���ֲ��ſ�����  
/*********************************************************************************************************
* �� �� �� : Audio_Start
* ����˵�� : ��ʼ������Ƶ
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ʵ���Ͼ�������dma����
*********************************************************************************************************/ 
void Audio_Start(void)
{
	audiodev.status=3<<0;	//��ʼ����+����ͣ
	I2S_Play_Start();
} 
/*********************************************************************************************************
* �� �� �� : Audio_Stop
* ����˵�� : �ر���Ƶ����
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ʵ���Ͼ��ǹر�dma����
*********************************************************************************************************/ 
void Audio_Stop(void)
{
	audiodev.status=0;
	I2S_Play_Stop();
}  
/*********************************************************************************************************
* �� �� �� : Audio_GetTnum
* ����˵�� : �õ�ָ��·���µ��ļ�����
* ��    �� : path��·����
* �� �� ֵ : ����Ч�ļ�����
* ��    ע : ��
*********************************************************************************************************/ 
unsigned short Audio_GetTnum(unsigned char *path)
{	  
	unsigned char 	res  = 0;
	unsigned short	rval = 0;
	unsigned char *fn   = NULL; 			 
 	DIR tdir;	 				//��ʱĿ¼
	FILINFO tfileinfo;			//��ʱ�ļ���Ϣ		
				   			     
    res = f_opendir(&tdir, (const TCHAR *)path); 				//��Ŀ¼
  	tfileinfo.fsize =FF_MAX_LFN*2+1;							//���ļ�����󳤶�
	//tfileinfo.fname = mymalloc(SRAMIN, tfileinfo.fsize);		//Ϊ���ļ������������ڴ�
	if(res==FR_OK)
	{
		while(1)	//��ѯ�ܵ���Ч�ļ���
		{
	        res = f_readdir(&tdir, &tfileinfo);       				//��ȡĿ¼�µ�һ���ļ�
	        if(res!=FR_OK || tfileinfo.fname[0]==0) break;			//������/��ĩβ��,�˳�		  
     		fn = (unsigned char *)(*tfileinfo.fname?tfileinfo.fname:tfileinfo.fname);			 
			res = f_typetell(fn);	
			if((res&0XF0) == 0X40)	//ȡ����λ,�����ǲ��������ļ�	
			{
				rval++;				//��Ч�ļ�������1
			}	    
		}  
	} 
	myfree(SRAMIN, tfileinfo.fname);
	return rval;
}
/*********************************************************************************************************
* �� �� �� : Audio_IndexShow
* ����˵�� : ��ʾ��Ŀ��ǰ����
* ��    �� : fn: ������index����ǰ������total�����ļ�����
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void Audio_IndexShow(unsigned char *fn, unsigned short index, unsigned short total)
{
//	memset(Usart1.RxBuff, 0, sizeof(Usart1.RxBuff));
	taskENTER_CRITICAL();
	LCD_ShowFlashString(5, 190, "                            ",  0xbf3f, LCD_WHITE);
	LCD_ShowFlashString(5, 190, fn, 0xbf3f, LCD_WHITE);
	taskEXIT_CRITICAL();
//	sprintf((char *)Usart1.RxBuff, "��ǰ����:%d ����Ŀ:%d", index, total);
//	LCD_ShowFlashString(0, 160, Usart1.RxBuff, LCD_RED, LCD_WHITE);		  	  
}
/*********************************************************************************************************
* �� �� �� : Audio_MsgShow
* ����˵�� : ��ʾ����ʱ��
* ��    �� : totsec����Ƶ�ļ���ʱ�䳤�ȣ�cursec����ǰ����ʱ��
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void Audio_MsgShow(unsigned int totsec, unsigned int cursec)
{	
	static u16 playtime=0XFFFF;			//����ʱ����	  
	
	if(playtime != cursec)			//��Ҫ������ʾʱ��
	{
		playtime=cursec;
		lv_bar_set_value(guider_ui.screen_barMusic, (double)cursec/(double)totsec*100.0, LV_ANIM_ON);
//		sprintf((char *)Usart1.RxBuff, "%02d:%02d / %02d:%02d", playtime/60,playtime%60, totsec/60, totsec%60);
//		//LCD_ShowFlashString(0, 180, Usart1.RxBuff, LCD_RED, LCD_WHITE);
	} 		
}
/*********************************************************************************************************
* �� �� �� : Audio_MusicPlay
* ����˵�� : ����ָ��·��������˳�򲥷Ÿ�·���µ�wav��Ƶ�ļ�
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void Audio_MusicPlay(void)
{
 	DIR wavdir;	 							//Ŀ¼
	FILINFO wavfileinfo;					//�ļ���Ϣ
	unsigned char  *fn 	= NULL;   		//���ļ���
	unsigned char  *pname 	= NULL;			//��·�����ļ���
	unsigned short *wavindextbl = NULL;	//����������
	unsigned char  key 	= 0;			//��ֵ
	unsigned char  res 	= 0;
	unsigned short totwavnum 	= 0; 		//�����ļ�����
	unsigned short curindex 	= 0;		//ͼƬ��ǰ����
 	unsigned short temp 		= 0;
	
 	while(f_opendir(&wavdir, "0:/MUSIC"))	//�������ļ���
 	{	    
		printf("MUSIC�ļ��д���!\r\n");
		Delay_Ms(200);				  			  
	} 									  
	totwavnum = Audio_GetTnum("0:/MUSIC");  //�õ�����Ч�ļ���
  	while(totwavnum == NULL)				//�����ļ�����Ϊ0		
 	{	    
		printf("û�������ļ�!\r\n");
		Delay_Ms(200);			  
	}										   
	wavfileinfo.fsize = FF_MAX_LFN * 2 + 1;									//���ļ�����󳤶�
	//wavfileinfo.fname = mymalloc(SRAMIN, wavfileinfo.fsize);				//Ϊ���ļ������������ڴ�
 	pname = mymalloc(SRAMIN, wavfileinfo.fsize);							//Ϊ��·�����ļ��������ڴ�
 	wavindextbl = mymalloc(SRAMIN, 2*totwavnum);							//����2*totwavnum���ֽڵ��ڴ�,���ڴ�������ļ�����
 	while(pname==NULL || wavindextbl==NULL)									//�ڴ�������
 	{	    
		printf("�ڴ����ʧ��!\r\n");
		Delay_Ms(200);				  			  
	}  	 

    res = f_opendir(&wavdir, "0:/MUSIC"); //��Ŀ¼
	if(res == FR_OK)
	{
		curindex = 0;	//��ǰ����Ϊ0
		while(1)		//ȫ����ѯһ��
		{
			temp = wavdir.dptr;								//��¼��ǰindex
	        res = f_readdir(&wavdir, &wavfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
	        if(res!=FR_OK || wavfileinfo.fname[0]==0)	break;	//������/��ĩβ��,�˳�		  
     		fn = (unsigned char *)(*wavfileinfo.fname?wavfileinfo.fname:wavfileinfo.fname);			 
			res = f_typetell(fn);	
			if((res&0XF0) == 0X40)				//ȡ����λ,�����ǲ��������ļ�	
			{
				wavindextbl[curindex] = temp;	//��¼����
				curindex++;
			}	    
		} 
	}   
	curindex = 0;											//��0��ʼ��ʾ
	res = f_opendir(&wavdir, (const TCHAR *)"0:/MUSIC"); 	//��Ŀ¼
	while(res == FR_OK)	//�򿪳ɹ�
	{	
		dir_sdi(&wavdir, wavindextbl[curindex]);			//�ı䵱ǰĿ¼����	   
		res = f_readdir(&wavdir,&wavfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
		if(res!=FR_OK || wavfileinfo.fname[0]==0) break;	//������/��ĩβ��,�˳�
		fn = (unsigned char *)(*wavfileinfo.fname?wavfileinfo.fname:wavfileinfo.fname);			 
		strcpy((char*)pname, "0:/MUSIC/");					//����·��(Ŀ¼)
		strcat((char*)pname, (const char*)fn);  			//���ļ������ں���
		//printf("%s\r\n", fn);								//��ʾ�������� 
		Audio_IndexShow(fn, curindex+1, totwavnum);	//��ʾ�������ơ�����������ǰ����
		key = Audio_PlaySong(pname); 			 			//���������Ƶ�ļ�
		if(key == 1)				//��һ��
		{
			if(curindex)	curindex--;
			else 			curindex = totwavnum-1;
 		}else if(key==0 || key==2)	//��һ��
		{
			curindex++;		   	
			if(curindex >= totwavnum)	curindex=0;	//��ĩβ��ʱ��,�Զ���ͷ��ʼ
				
 		}else		
		{	
			break;	//�����˴��� 	 
		}
	} 		

	myfree(SRAMIN, wavfileinfo.fname);		//�ͷ��ڴ�			    
	myfree(SRAMIN, pname);					//�ͷ��ڴ�			    
	myfree(SRAMIN, wavindextbl);			//�ͷ��ڴ�	 
} 
/*********************************************************************************************************
* �� �� �� : Audio_PlaySong
* ����˵�� : ����ĳ����Ƶ�ļ�
* ��    �� : fname���ļ�·��+�ļ���
* �� �� ֵ : 0����һ�ף���Ȼ���ţ���1����һ�ף�2����һ�ף�������ֹͣ���Ż��߷����˴���
* ��    ע : ��
*********************************************************************************************************/ 
unsigned char Audio_PlaySong(unsigned char *fname)
{
	unsigned char  res = f_typetell(fname);  

	switch(res)
	{
		case T_WAV:
			res = Wav_PlaySong(fname);
			break;
		default:	//�����ļ�,�Զ���ת����һ��
			printf("can't play:%s\r\n",fname);
			res = 0;
			break;
	}
	return res;
}
/*********************************************************************************************************
* �� �� �� : BatchedAction
* ����˵�� : ��������ʶ����
* ��    �� : action��ʶ�𵽵�ָ�����
* �� �� ֵ : ��
* ��    ע : ��
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
				Wav_PlayRevert("0:/prompt/�õ���Ϊ���رշ���.wav");	
				break;
			case 0x02:	
				SetHollowSpeed(speed);	
				Wav_PlayRevert("0:/prompt/�õ���Ϊ���򿪷���.wav");	
				break;
			case 0x03: /*��ʱ�򿪷���*/	break;
			case 0x04: /*��ʱ�رշ���*/	break;
			case 0x05:	
				SetHollowSpeed(3);	
				Wav_PlayRevert("0:/prompt/�õķ�����Ϊ������һ��.wav");	
				break;
			case 0x06:	
				SetHollowSpeed(4);	
				Wav_PlayRevert("0:/prompt/�õķ�����Ϊ����������.wav");	
				break;
			case 0x07:	
				SetHollowSpeed(5);	
				Wav_PlayRevert("0:/prompt/�õķ�����Ϊ����������.wav");	
				break;
			case 0x08:	
				SetHollowSpeed(6);	
				Wav_PlayRevert("0:/prompt/�õķ�����Ϊ�������ĵ�.wav");	
				break;
			case 0x09:	
				speed += 1;
				if(speed >= 6)	speed = 6;
					SetHollowSpeed(speed);
				Wav_PlayRevert("0:/prompt/������Ϊ�����Ӱٷ�֮ʮ.wav");	
				break;
			case 0x10:
				speed -= 1;
				if(speed <= 0)	speed = 0;
					SetHollowSpeed(speed);
				Wav_PlayRevert("0:/prompt/������Ϊ����С�ٷ�֮ʮ.wav");	
				break;
			case 0x11:/*�������*/	break;
			case 0x12:/*��ʼ����*/	break;
			
			case 0x13:	//�����еĵ�
				LED1 = 0;	LED2 = 0;	LED3 = 0;
				W2812_WriteColorData(RGB_WHITE);
				W2812_WriteColorData(RGB_WHITE);
				lv_led_on(guider_ui.screen_ledBedroom);
				lv_led_on(guider_ui.screen_ledKitchen);
				lv_led_on(guider_ui.screen_ledSaloon);
				Wav_PlayRevert("0:/prompt/�õ���Ϊ�������еĵ�.wav");	
				break;
			case 0x14:	//�ر����еĵ�
				LED1 = 1;	LED2 = 1;	LED3 = 1;
				W2812_WriteColorData(RGB_BLACK);
				W2812_WriteColorData(RGB_BLACK);
				lv_led_off(guider_ui.screen_ledBedroom);
				lv_led_off(guider_ui.screen_ledKitchen);
				lv_led_off(guider_ui.screen_ledSaloon);
				Wav_PlayRevert("0:/prompt/�õ���Ϊ���ر����еĵ�.wav");	
				break;
			case 0x15:/*��ʱ����*/	break;
			case 0x16:/*��ʱ�ص�*/	break;
			case 0x17:	//���ҵĵ�
				LED1 = 0;	
				lv_led_on(guider_ui.screen_ledBedroom);
				Wav_PlayRevert("0:/prompt/�õ���Ϊ����.wav");	
				break;	
			case 0x18:	//���ҵĵ�
				LED1 = 1;
				lv_led_off(guider_ui.screen_ledBedroom);
				Wav_PlayRevert("0:/prompt/�õ���Ϊ���ر�.wav");
				break;	
			case 0x19:	
				LED2 = 0;	
				lv_led_on(guider_ui.screen_ledSaloon);
				Wav_PlayRevert("0:/prompt/�õ���Ϊ����.wav");	
				break;	//�����ĵ�
			case 0x20:	
				LED2 = 1;	
				lv_led_off(guider_ui.screen_ledSaloon);
				Wav_PlayRevert("0:/prompt/�õ���Ϊ���ر�.wav");
				break;	//�����ĵ�
			case 0x21:	
				LED3 = 0;
				lv_led_on(guider_ui.screen_ledKitchen);
				Wav_PlayRevert("0:/prompt/�õ���Ϊ����.wav");
				break;	//�����ĵ�
			case 0x22:	
				LED3 = 1;
				lv_led_off(guider_ui.screen_ledKitchen);
				Wav_PlayRevert("0:/prompt/�õ���Ϊ���ر�.wav");
				break;	//�����ĵ�
			case 0x23:	//��ԡ�ҵĵ�
				W2812_WriteColorData(RGB_BLACK);
				W2812_WriteColorData(RGB_WHITE);
				Wav_PlayRevert("0:/prompt/�õ���Ϊ����.wav");
				break;
			case 0x24:	//�ر�ԡ�ҵĵ�
				W2812_WriteColorData(RGB_BLACK);
				W2812_WriteColorData(RGB_BLACK);
				Wav_PlayRevert("0:/prompt/�õ���Ϊ���ر�.wav");
				break;	
			case 0x25:	//��̨��
				W2812_WriteColorData(RGB_WHITE);	
				W2812_WriteColorData(RGB_BLACK);
				Wav_PlayRevert("0:/prompt/�õ���Ϊ����.wav");
				break;	
			case 0x26:	//�ر�̨��
				W2812_WriteColorData(RGB_BLACK);
				W2812_WriteColorData(RGB_BLACK);
				Wav_PlayRevert("0:/prompt/�õ���Ϊ���ر�.wav");
				break;	
			case 0x27:/*�Ƶ�������*/	break;
			case 0x28:/*�Ƶ����*/	break;
			case 0x29:/*����һ��*/		break;
			case 0x30:/*�ư�һ��*/		break;
			case 0x31:/*��������*/	break;
			case 0x32:/*�ر�������*/	break;		
			case 0x33:/*��ůģʽ*/		break;
			case 0x34:/*����ģʽ*/		break;
			case 0x35:/*˯��ģʽ*/		break;
			
			case 0x36:/*�򿪿յ�*/	
				Infrared_SendCmd(IR_IN_SEND, 0);
				Wav_PlayRevert("0:/prompt/�õ���Ϊ����.wav");
				break;
			case 0x37:/*�رտյ�*/	
				Infrared_SendCmd(IR_IN_SEND, 1);
				Wav_PlayRevert("0:/prompt/�õ���Ϊ���ر�.wav");
				break;
			case 0x38:/*����ģʽ*/	break;
			case 0x39:/*����ģʽ*/	break;
			case 0x40:/*�����¶�*/	
				Infrared_SendCmd(IR_IN_SEND, 2);
				Wav_PlayRevert("0:/prompt/�õ���Ϊ������.wav");
				break;
			case 0x41:/*�����¶�*/	
				Infrared_SendCmd(IR_IN_SEND, 3);
				Wav_PlayRevert("0:/prompt/�õ���Ϊ������.wav");
				break;
			case 0x42:/*���°ڷ�*/	break;
			case 0x43:/*���Ұڷ�*/	break;
			case 0x44:/*��ʪģʽ*/	break;
			case 0x45:/*�Զ�ģʽ*/	break;
			
			case 0x46:	//��ɱ��
				STERILIZE = 1;
				Wav_PlayRevert("0:/prompt/�õ���Ϊ����.wav");
				break;	
			case 0x47:	//�ر�ɱ��
				STERILIZE = 0;	
				Wav_PlayRevert("0:/prompt/�õ���Ϊ���ر�.wav");
				break;	
			case 0x48:	
				STERILIZE = 1;	
				Wav_PlayRevert("0:/prompt/�õ���Ϊ����.wav");
				break;	//�򿪺��
			case 0x49:	
				STERILIZE = 0;	
				Wav_PlayRevert("0:/prompt/�õ���Ϊ���ر�.wav");
			break;	//�رպ��
			
			case 0x50:	//�򿪴���
				SetServoAngle(140);
				lv_led_on(guider_ui.screen_ledFinestra);
				Wav_PlayRevert("0:/prompt/�õ���Ϊ����.wav");
				break;	
			case 0x51:	
				SetServoAngle(0);	
				lv_led_off(guider_ui.screen_ledFinestra);
				Wav_PlayRevert("0:/prompt/�õ���Ϊ���ر�.wav");
				break;	//�رմ���
			case 0x52:	
				SetServoAngle(90);	
				Wav_PlayRevert("0:/prompt/�õ���Ϊ����.wav");
				break;	//�뿪����
			case 0x53:	//����
				SetServoAngle(180);	
				Wav_PlayRevert("0:/prompt/�õ���Ϊ����.wav");
				break;	
			case 0x54:	//����
				SetServoAngle(0);	
				Wav_PlayRevert("0:/prompt/�õ���Ϊ���ر�.wav");
				break;	
			/*������...*/
			
			case 0x55:	//��������
				Wav_PlayRevert("0:/prompt/�õ���Ϊ������.wav");
				WM8978_SPKvol_Set(VOL);
				status_dev.PlayState = PLAY_PLAY;
				status_dev.PlayMode = true;
				WM8978_SPKvol_Set(0);
				xTaskNotify(PlayMusic_Handler, 1, eSetValueWithOverwrite);
				break;
			case 0x56:	//��ͣ����
				status_dev.PlayState = PLAY_PAUSE;	
				Wav_PlayRevert("0:/prompt/�õ���Ϊ����ͣ.wav");
				WM8978_SPKvol_Set(0);
				break;	
			case 0x57:	//ֹͣ����
				status_dev.PlayState = PLAY_STOP;	
				status_dev.PlayMode = false;
				Wav_PlayRevert("0:/prompt/�õ���Ϊ���ر�.wav");
				WM8978_SPKvol_Set(0);
				break;	
			case 0x58:	//��һ��	
				status_dev.PlayState = PLAY_PREVIOUS;	
				Wav_PlayRevert("0:/prompt/�õģ���Ϊ���л�.wav");
				WM8978_SPKvol_Set(0);
				break;	
			case 0x59:	//��һ��
				status_dev.PlayState = PLAY_NEXT;	
				Wav_PlayRevert("0:/prompt/�õģ���Ϊ���л�.wav");
				WM8978_SPKvol_Set(0);
				break;	
			
			case 0x60:	//�򿪱���
				BEEP = 1;	
				lv_led_on(guider_ui.screen_ledAlarms);
				Wav_PlayRevert("0:/prompt/�õ���Ϊ����.wav");
				break;	
			case 0x61:	//�رձ���
				BEEP = 0;	
				lv_led_off(guider_ui.screen_ledAlarms);
				Wav_PlayRevert("0:/prompt/�õ���Ϊ���ر�.wav");
				break;	
			
			case 0x62:	/*�������²���*/	
			{	
				float temperature = 0.0;
				BaseType_t xHigherPriorityTaskWoken;
				
				Mlx90614_ReadTemperature(&temperature);	//�ѽ�����͵���Ϣ����
				xQueueSendFromISR(MessageQueue_Tem, &temperature, &xHigherPriorityTaskWoken);	
				Wav_PlayRevert("0:/prompt/�õ���Ϊ����.wav");
			}	break;
			
			case 0x63:	//��С�������˲��ֹ̼�����δʵ��
				status_dev.volume -= 10; 
				if(status_dev.volume <= 10)	status_dev.volume = 10; 
				WM8978_SPKvol_Set(status_dev.volume);
				break;	
			case 0x64:	//��������
				status_dev.volume += 10;	
				if(status_dev.volume >= 60)	status_dev.volume = 63; 
				WM8978_SPKvol_Set(status_dev.volume);
				break;	
			case 0x65:	//��С����
				status_dev.volume = 10;	
				WM8978_SPKvol_Set(status_dev.volume);
				break;	
			case 0x66:	//�������
				status_dev.volume = 63;	
				WM8978_SPKvol_Set(status_dev.volume);
				break;	
			
			case 0xaa:	//����
			{	
				switch(state++)
				{
					case 0:	Wav_PlayRevert("0:/prompt/��������.wav");						break;
					case 1:	Wav_PlayRevert("0:/prompt/��������.wav");						break;	
					case 2:	Wav_PlayRevert("0:/prompt/��ʲô���԰�����.wav");	state = 0;	break;	
				}
			}	break;
			case 0xab:	
				Wav_PlayRevert("0:/prompt/����Ҫ�ٽ���.wav");	
				break;
			case 0xac:	
				Wav_PlayRevert("0:/prompt/����������.wav");	
				break;
			default:
				printf("Warning: There is no such command\r\n");
			break;
		}
	}
}
/*********************************************************************************************************
* �� �� �� : IdentifyResults
* ����˵�� : ����ģ��ʶ����
* ��    �� : buf����Ž���Ļ�����
* �� �� ֵ : ��ȡ���ı���
* ��    ע : ��
*********************************************************************************************************/ 
unsigned char IdentifyResults(unsigned char *buf)
{
	unsigned char cnt = 0;
	
	if(buf[0] == 0x01)				//���ϵ��һ��ʶ����յ�һ��0x01
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



