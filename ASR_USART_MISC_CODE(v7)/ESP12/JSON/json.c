#include "app_task.h"

__status_dev status_dev;
/*********************************************************************************************************
* �� �� �� : JsonString_Parsing
* ����˵�� : json�ַ���������Ŀǰֻ֧�ֽ�����ͨ��ֵ�Ժ�rgb��ֵ��
* ��    �� : str����Ҫ������json�ַ���
* �� �� ֵ : true����������false���ɹ�
* ��    ע : ��
*********************************************************************************************************/ 
char JsonString_Parsing(unsigned char *str, JSON *json) 
{
	char i = 0, k = 0;
	char *sp = NULL;
	unsigned char color[3] = {0};
	int value = 0;
	
	if((sp=strstr((char *)str, "+MQTTSUBRECV:0")) != NULL)
	{
		if((sp=strstr((char *)str, "params")) != NULL)	//����params�ֶΣ����ֶκ���Я��ʵ����Ҫ���ֶ� 
		{
			sp += 10;	//ƫ�Ƶ�ʵ����Ҫ�������ֶ� 
			for(i=0; sp[i]!='"'; i++)	//������ 
				json->key[i] = sp[i];
			sp = strstr(sp, ":") + 1;		//��Ϊspָ�뱾��û��ƫ�ƣ�����Ҫƫ�Ƶ���һ��ð��+1��λ�� 
			if(!strstr(json->key, "RGB"))	//����������ݵ�����ΪRGB����ô�����⴦�� 
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
* �� �� �� : JsonString_Dispose
* ����˵�� : ���ݼ�ֵ�Ե�ֵ������Ӧ�Ķ���
* ��    �� : json��ֵ��
* �� �� ֵ : ��
* ��    ע : ��
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
			case 0:/*�����޸�app*/
				status_dev.PlayMode = true;
				status_dev.PlayState = PLAY_PLAY; 					//���� / ��ͣ
				xTaskNotify(PlayMusic_Handler, true, eSetValueWithOverwrite);
			break;
			case 1:	status_dev.PlayState = PLAY_PAUSE; 		break;	//��ͣ����
			case 2:	status_dev.PlayState = PLAY_PREVIOUS; 	break;	//��һ��
			case 3:	status_dev.PlayState = PLAY_NEXT; 		break;	//��һ��
		}
	}
	
}
