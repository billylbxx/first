#include "app_task.h"

__status_dev status_dev;
/*********************************************************************************************************
* 函 数 名 : JsonString_Parsing
* 功能说明 : json字符串解析，目前只支持解析普通键值对和rgb键值对
* 形    参 : str：需要解析的json字符串
* 返 回 值 : true：发生错误；false：成功
* 备    注 : 无
*********************************************************************************************************/ 
char JsonString_Parsing(unsigned char *str, JSON *json) 
{
	char i = 0, k = 0;
	char *sp = NULL;
	unsigned char color[3] = {0};
	int value = 0;
	
	if((sp=strstr((char *)str, "+MQTTSUBRECV:0")) != NULL)
	{
		if((sp=strstr((char *)str, "params")) != NULL)	//先找params字段，该字段后面携带实际需要的字段 
		{
			sp += 10;	//偏移到实际需要解析的字段 
			for(i=0; sp[i]!='"'; i++)	//解析键 
				json->key[i] = sp[i];
			sp = strstr(sp, ":") + 1;		//因为sp指针本身并没有偏移，所以要偏移到下一个冒号+1的位置 
			if(!strstr(json->key, "RGB"))	//如果本次数据的数据为RGB，那么做特殊处理 
			{	
				for(i=0; sp[i]!='}'; i++)
				{
					if(sp[i]>='0' && sp[i]<='9')
					{
						value *= 10;
						value += sp[i] - 48; 
					}
					else
					{
						printf("Warning: Illegal data segment, terminate parsing\r\n");
						return true;	
					} 
				}
				json->value = value;
				printf("%s:%d", json->key, json->value);
			}
			else
			{
				for(k=0; k<3; k++)
				{
					sp = strstr(sp, ":") + 1;	
					for(i=0; sp[i]!=','&&sp[i]!='}'; i++)
					{
						if(sp[i]>='0' && sp[i]<='9')
						{
							color[k] *= 10;
							color[k] += sp[i] - 48;
						}
						else
						{
							printf("Warning: Illegal data segment, terminate parsing\r\n");
							return true;	
						} 
					}
				}
				json->Rgb.R = color[0];
				json->Rgb.G = color[1];
				json->Rgb.B = color[2];	
				printf("%s:%d,%d,%d", json->key, json->Rgb.R, json->Rgb.G, json->Rgb.B);
			}
		}else return true;	
	}else return true;	
	return false;
}
/*********************************************************************************************************
* 函 数 名 : JsonString_Dispose
* 功能说明 : 根据键值对的值做出相应的动作
* 形    参 : json键值对
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void JsonString_Dispose(JSON *json)
{
	if(strstr(json->key, "RGB"))
	{
		W2812_WriteColorData(json->Rgb.R<<16|json->Rgb.B<<8|json->Rgb.G);
		W2812_WriteColorData(json->Rgb.R<<16|json->Rgb.B<<8|json->Rgb.G);
		lv_colorwheel_set_rgb(guider_ui.screen_cpicker_color, lv_color_make(json->Rgb.R, json->Rgb.B, json->Rgb.G));
	}
	else if(strstr(json->key, "BedroomLed"))
	{
		LED1 = !json->value;
		json->value ? lv_led_on(guider_ui.screen_ledBedroom) : lv_led_off(guider_ui.screen_ledBedroom);
	}
	else if(strstr(json->key, "LivingRoomLed"))
	{
		LED2 = !json->value;
		json->value ? lv_led_on(guider_ui.screen_ledSaloon) : lv_led_off(guider_ui.screen_ledSaloon);
	}
	else if(strstr(json->key, "KitchenLed"))
	{
		LED3 = !json->value;
		json->value ? lv_led_on(guider_ui.screen_ledKitchen) : lv_led_off(guider_ui.screen_ledKitchen);
	}
	else if(strstr(json->key, "Disinfect"))
	{
		STERILIZE = json->value;
	}
	else if(strstr(json->key, "doorSwitch"))
	{
		if(json->value == 1)	
		{
			SetServoAngle(180);	
			lv_led_on(guider_ui.screen_ledFinestra);
		}
		else 					
		{
			SetServoAngle(0);	
			lv_led_off(guider_ui.screen_ledBedroom);
		}
	}
	else if(strstr(json->key, "airSwitch"))
	{
		if(json->value == 1)	
			Infrared_SendCmd(IR_IN_SEND, 1);
		else Infrared_SendCmd(IR_IN_SEND, 0);
	}
	else if(strstr(json->key, "airTemperatureSet"))
	{
		if(json->value == 1)	
			Infrared_SendCmd(IR_IN_SEND, 3);
		else Infrared_SendCmd(IR_IN_SEND, 2);
	}
	else if(strstr(json->key, "fanSwitch"))
	{
		if(json->value == 1)	
		{
			status_dev.fanSwitch = true;
			SetHollowSpeed(5);
		}
		else 					
		{
			status_dev.fanSwitch = false;
			SetHollowSpeed(0);
		}
	}
	else if(strstr(json->key, "fanGear"))
	{
		if(status_dev.fanSwitch == true)
		{
			if(json->value>=0 && json->value<=4)
				SetHollowSpeed(json->value*3);
		}
	}
	else if(strstr(json->key, "MusicPlayControl"))
	{
		switch(json->value)
		{
			case 0:/*还需修改app*/
				status_dev.PlayMode = true;
				status_dev.PlayState = PLAY_PLAY; 					//播放 / 暂停
				xTaskNotify(PlayMusic_Handler, true, eSetValueWithOverwrite);
			break;
			case 1:	status_dev.PlayState = PLAY_PAUSE; 		break;	//暂停播放
			case 2:	status_dev.PlayState = PLAY_PREVIOUS; 	break;	//上一首
			case 3:	status_dev.PlayState = PLAY_NEXT; 		break;	//下一首
		}
	}
	
}
