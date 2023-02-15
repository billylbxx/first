#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "esp12.h"
#include "systick.h"
#include "usart1.h"
#include "hmacsha1.h"
#include "sht30.h"
#include "w25qxx.h"
#include "json.h"
#include "led.h"
#include "lcd.h"
#include "lvgl_app.h"

JSON json;
unsigned char ConnectFlag = false;		//true�������ӣ�false��δ����
static unsigned char mqtt_message[256];				//������ݻ�����
static unsigned char ssid[MESSAGE_MAX/2] = "iPhone", pwsd[MESSAGE_MAX/2] = "12345678";
static unsigned char state 		= AT_AT;	//��ǰ״̬
static unsigned char before 		= AT_AT;	//��һ��״̬
static unsigned char next 		= AT_AT;	//��һ��״̬
static unsigned char wishack 		= AT_OK;	//ϣ��Ӧ��
static unsigned char response 	= AT_OK;	//�õ���Ӧ��
static unsigned short timeout 	= WAIT_100;	//��ʱʱ��
static unsigned short interval	= WAIT_100;	//�������̫������busy...��������100ms����
static unsigned short count 		= TRY_COUNT;//����������һ�������򴥷�ĳ������

static void JumpState(unsigned char sta, unsigned char offset);
static unsigned char MeasuringResponse(unsigned char ack);
static void CalculateSha1(unsigned char *password);
static unsigned char DownData_Handle(void);
static void mstrcat(char *s1, const char *s2);
static unsigned short CRC16_Compute(unsigned char *buffer, unsigned short len);

/*********************************************************************************************************
* �� �� �� : PublishMessage
* ����˵�� : �����ϱ�
* ��    �� : StructMessage�����ж���Ҫ�ϱ���ֵ���ֶ���
* �� �� ֵ : ��
* ��    ע : MQTT�̼��ڲ��Դ�15S���������Բ���Ҫ����ϱ�����Ҳ���Ա��ֳ�����
			 һ���ϱ������ݰ����ܴ��ڻ������Ĵ�С���ֶζ�ʱӦ�ֶ��ϱ�
*********************************************************************************************************/ 
void PublishMessage(void *StructMessage)	/*���βο����޸ĳɽṹ��ָ��,���������޸�*/
{	
	SHT30_TypeDef *ht = (SHT30_TypeDef *)StructMessage;
	
	memset(mqtt_message, 0, sizeof(mqtt_message));
	sprintf((char *)mqtt_message,	
		"AT+MQTTPUB=0,\"%s\",\"{\\\"method\\\":\\\"thing.service.property.set\\\"\\,\\\"id\\\":\\\"2012934115\\\"\\,\
	    \\\"params\\\":{\\\"CuTemperature\\\":%d\\,\\\"CurrentHumidity\\\":%d}\\,\\\"version\\\":\\\"1.0.0\\\"}\",1,0\r\n",
		PublishMessageTopPost,
		(int)ht->Temperature,
		(int)ht->Humidity
	);
	Usart2_SendString(mqtt_message);
}
/*********************************************************************************************************
* �� �� �� : TaskEsp12Msg_Handle
* ����˵�� : esp12�¼�������
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : 10ms��ˢ�¼��
*********************************************************************************************************/ 
void TaskEsp12Msg_Handle(void)
{
	switch(state)
	{
		case AT_AT:	
			//LCD_ShowFlashString(0, 220, "������������  ", LCD_RED, LCD_WHITE);
			RevampState("AT\r\n", WAIT, AT_AT, ECHO, AT_OK, WAIT_1000, WAIT_100);
			break;
		case ECHO:	
			RevampState("ATE0\r\n", WAIT, ECHO, AT_CWMODE1, AT_OK, WAIT_1000, WAIT_100);
			break;
		case AT_CWMODE1:
			RevampState("AT+CWMODE=1\r\n", WAIT, AT_CWMODE1, AT_CIFSR, AT_OK, WAIT_1000, WAIT_100);
			break;
		case AT_CIFSR:
			RevampState("AT+CIFSR\r\n", WAIT, AT_CIFSR, AT_CWJAP, AT_IP, WAIT_1000, WAIT_100);
			break;
		case AT_CWJAP:
			lv_bar_set_value(guider_ui.screen_barState, 20, LV_ANIM_ON);
			memset(mqtt_message, 0, sizeof(mqtt_message));
			sprintf((char *)mqtt_message, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwsd);
			RevampState((char *)mqtt_message, WAIT, AT_CWJAP, AT_MQTTCLEAN, AT_OK, WAIT_10000, WAIT_200);
			break;
		case AT_MQTTCLEAN:	
			if(before == AT_CWMODE2)	//����Ǵ�AT_CWMODE2״̬�����ģ���ô��һ��״̬��AT_CIPMUX
			{
				RevampState("AT+MQTTCLEAN=0\r\n", WAIT, AT_MQTTCLEAN, AT_CIPMUX, AT_OK|AT_ERR, WAIT_1000, WAIT_200);
			}
			else RevampState("AT+MQTTCLEAN=0\r\n", WAIT, AT_MQTTCLEAN, AT_CIPSNTPCFG, AT_OK|AT_ERR, WAIT_1000, WAIT_200);
			break;
		case AT_CIPSNTPCFG:
			RevampState("AT+CIPSNTPCFG=1,8,\"ntp1.alliyun.com\"\r\n", WAIT, AT_CIPSNTPCFG, AT_MQTTUSERCFG, AT_OK, WAIT_1000, WAIT_200);
			break;
		case AT_MQTTUSERCFG:{
			lv_bar_set_value(guider_ui.screen_barState, 40, LV_ANIM_ON);
			unsigned char PassWord[50] = {0};
			CalculateSha1(PassWord);
			memset(mqtt_message, 0, sizeof(mqtt_message));
			sprintf((char *)mqtt_message, "AT+MQTTUSERCFG=0,1,\"NULL\",\"%s\",\"%s\",0,0,\"\"\r\n", UserName, PassWord);
			RevampState((char *)mqtt_message, WAIT, AT_MQTTUSERCFG, AT_MQTTCLIENTID, AT_OK, WAIT_1000, WAIT_100);
		}	break;
		case AT_MQTTCLIENTID:
			memset(mqtt_message, 0, sizeof(mqtt_message));
			sprintf((char *)mqtt_message, "AT+MQTTCLIENTID=0,\"%s\"\r\n", ClientId);
			RevampState((char *)mqtt_message, WAIT, AT_MQTTCLIENTID, AT_MQTTCONN, AT_OK, WAIT_1000, WAIT_100);
			break;
		case AT_MQTTCONN:
			lv_bar_set_value(guider_ui.screen_barState, 60, LV_ANIM_ON);
			memset(mqtt_message, 0, sizeof(mqtt_message));
			sprintf((char *)mqtt_message, "AT+MQTTCONN=0,\"%s\",1883,1\r\n", IP);
			RevampState((char *)mqtt_message, WAIT, AT_MQTTCONN, AT_MQTTSUB, AT_OK, WAIT_1000, WAIT_100);
			break;
		case AT_MQTTSUB:
			memset(mqtt_message, 0, sizeof(mqtt_message));
			sprintf((char *)mqtt_message, "AT+MQTTSUB=0,\"%s\",1\r\n", PublishMessageTopSet);
			RevampState((char *)mqtt_message, WAIT, AT_MQTTSUB, FINISH, AT_OK, WAIT_1000, WAIT_100);
			break;
		case AT_CWMODE2:
			//LCD_ShowFlashString(0, 220, "����������     ", LCD_RED, LCD_WHITE);
			memset(ssid, 0, MESSAGE_MAX/2);			//������ػ�����Ϣ
			memset(pwsd, 0, MESSAGE_MAX/2);
			memset(mqtt_message, 0, sizeof(mqtt_message));
			W25QXX_EraseSector(WIFI_SECTOR_NUM);
			RevampState("AT+CWMODE=2\r\n", WAIT, AT_CWMODE2, AT_MQTTCLEAN, AT_OK, WAIT_1000, WAIT_100);
			break;
		case AT_CIPMUX:
			RevampState("AT+CIPMUX=1\r\n", WAIT, AT_CIPMUX, AT_CWSAP, AT_OK, WAIT_1000, WAIT_100);
			break;
		case AT_CWSAP:{
			lv_bar_set_value(guider_ui.screen_barState, 80, LV_ANIM_ON);
			unsigned char buf[12] = {0};
//			char i = 0;
//			for(i=0; i<12; i++)
//				buf[i] = *(unsigned char *)(UNIQUE_ID_ADDRESS + i);
			memcpy(buf, (unsigned char *)UNIQUE_ID_ADDRESS, 12);
			memset(mqtt_message, 0, sizeof(mqtt_message));
			sprintf((char *)mqtt_message, "AT+CWSAP=\"Smart-%05d\",\"12345678\",1,4\r\n", CRC16_Compute(buf, 12));
			RevampState((char *)mqtt_message, WAIT, AT_CWSAP, AT_CIPSERVER, AT_OK, WAIT_1000, WAIT_100);
		}	break;
		case AT_CIPSERVER:
			RevampState("AT+CIPSERVER=1,8086\r\n", WAIT, AT_CIPSERVER, WAIT_CONF, AT_OK, WAIT_1000, WAIT_100);
			break;
		case WAIT:
			response = MeasuringResponse(wishack);
			switch(response)
			{
				case AT_OK:
					if(--interval == 0)	//���һ����ʱ�����ת����һ��״̬
					{
						#if WIFI_DEGBUG
							printf("***%d->%d***\r\n", before, next);
						#endif
						count = TRY_COUNT;			//�ط�����
						JumpState(before, next);	//��ת����һ��״̬
					}	
					break;
				case AT_ERR:
					if(--interval == 0)	//���һ����ʱ�����ת����һ��״̬
					{
						#if WIFI_DEGBUG
							printf("###%d->%d###\r\n", before, before);
						#endif
						JumpState(before, before);	//��ת����һ��״̬			
						if(--count == 0)			//error����������һ�������ᴥ����λ״̬
						{
							RevampState("AT+RST\r\n", AT_RST, AT_ERR, AT_RST, AT_OK, WAIT_4000, WAIT_100);
							count = TRY_COUNT;
						}
					}
					break;
				case AT_IPOK:
					#if WIFI_DEGBUG
						printf("�Ѿ���ȡ��ip\r\n");
					#endif
					RevampState(NULL, AT_MQTTCLEAN, AT_IPOK, AT_MQTTCLEAN, AT_OK, WAIT_1000, WAIT_100);
					break;
				case AT_IPERR:
					#if WIFI_DEGBUG
						printf("δ��ȡ��ip\r\n");
					#endif
					
					if((CheckFlashOrBuffer(ssid, pwsd, true)==true) || (ssid[0]!=0 || pwsd[0]!=0))	//����Ѿ������˺����룬�ȳ�������
					{
						printf("--%s %s--\r\n", ssid, pwsd);
						RevampState(NULL, AT_CWJAP, AT_CWJAP, AT_CWJAP, AT_OK, WAIT_1000, WAIT_100);
					}
					else								//�������staģʽ���ȴ��û�����
					{
						printf("�ȴ��û�����wifi��Ϣ\r\n");
						Set_Esp12ConnectionStatus(THREE);
						RevampState(NULL, AT_CWMODE2, AT_IPERR, AT_CWMODE2, AT_OK, WAIT_1000, WAIT_100);
					}
					break;
				default:
					if(--timeout == 0)			//��������涨ʱ��û�еõ�ϣ����Ӧ�����ת����һ��״̬
					{
						#if WIFI_DEGBUG
							printf("***timeout->%d***\r\n", before);
						#endif
						JumpState(before, before);
						if(--count == 0)		//��ʱ����������һ�������ᴥ����λ״̬
						{
							RevampState("AT+RST\r\n", AT_RST, AT_IPERR, AT_IPERR, AT_OK, WAIT_4000, WAIT_100);
							count = TRY_COUNT;
						}
					}
					break;
			}
			break;
		case AT_RST:	//4S�������λ״̬���ص��ʼ��״̬
			if(--timeout == 0)	
				RevampState("AT\r\n", WAIT, AT_AT, AT_AT, AT_OK, WAIT_1000, WAIT_100);
			break;
		case WAIT_CONF:
			if(DownData_Handle() == WAIT_CONF)
			{
				printf("--%s %s--\r\n", ssid, pwsd);
				Set_Esp12ConnectionStatus(false);
				RevampState(NULL, AT_AT, AT_AT, AT_AT, AT_OK, WAIT_1000, WAIT_100);
			}
			break;
		case FINISH:	//������Ӻ���һЩ׼�������������ת����״̬
			Set_Esp12ConnectionStatus(true);
			LED1 = 1;
			lv_bar_set_value(guider_ui.screen_barState, 100, LV_ANIM_ON);
			memset(mqtt_message, 0, sizeof(mqtt_message));
			timeout = sprintf((char *)mqtt_message, "ssid:%s pwsd:%s", ssid, pwsd);
			mqtt_message[timeout] = 0xaa;
			W25QXX_EraseSector(WIFI_SECTOR_NUM);
			W25QXX_WritePage(mqtt_message, WIFI_FLASH_ADDR, MESSAGE_MAX);	//���ӳɹ������flash�е���Ϣ
			printf("Platform connection successful\r\n");
//			LCD_ShowFlashString(0, 220, "�������ӳɹ�  ", LCD_RED, LCD_WHITE);
			RevampState(NULL, FREE, FINISH, FREE, AT_OK, WAIT_1000, WAIT_100);
			break;
		case FREE:	
			if(DownData_Handle() == DISCONNECT)			//����ʱ�ص������״̬
				RevampState("AT\r\n", WAIT, AT_AT, AT_AT, AT_OK, WAIT_1000, WAIT_100);
			break;
	}
}
/*********************************************************************************************************
* �� �� �� : DownData_Handle
* ����˵�� : �����·�����
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ��⵽�Ͽ����ӻᴥ���������ƣ���⵽�û������˺��������������sta
*********************************************************************************************************/ 
static unsigned char DownData_Handle(void)
{
	unsigned char temp = 0;
	
	if(WifiUsart.RecFlag==true && (Get_Esp12ConnectionStatus()==true || Get_Esp12ConnectionStatus()==THREE))
	{
		WifiUsart.RecFlag = false;
		WifiUsart.RecLen = 0;
		//printf("%s\r\n", WifiUsart.RxBuff);
		if(!strstr((char *)Usart2.RxBuff, "AT+MQTTPUB=0"))	//�˳��ϱ����Ե�����
		{
			if(!JsonString_Parsing(Usart2.RxBuff, &json))
			{
				JsonString_Dispose(&json);
			}
			
			if(strstr((char *)WifiUsart.RxBuff, "WIFI DISCONNECT") || strstr((char *)WifiUsart.RxBuff, "+MQTTDISCONNECTED"))	
			{
				Set_Esp12ConnectionStatus(false);
				temp = DISCONNECT;
			}
			else if(strstr((char *)WifiUsart.RxBuff, "ssid"))
			{
				if(CheckFlashOrBuffer(ssid, pwsd, false) == true)
				{
					printf("�յ�����Ϣ����ssid\r\n");
					temp = WAIT_CONF;
				}
			}
		}
		memset(WifiUsart.RxBuff, 0, sizeof(WifiUsart.RxBuff));
	}
	return temp;
}
/*********************************************************************************************************
* �� �� �� : CheckFlashOrBuffer
* ����˵�� : ����ڲ�flash���ߴ��ڽ��ջ��������Ƿ�����˺�����
* ��    �� : ssid�˺Ŵ洢����pwsd����洢����mode��true����ڲ�flash��false��鴮�ڽ��ջ�����
* �� �� ֵ : true���ڣ�false������
* ��    ע : ��
*********************************************************************************************************/ 
unsigned char CheckFlashOrBuffer(unsigned char *ssid, unsigned char *pwsd, char mode)
{
	unsigned char cnt = 0;
	unsigned char *sp = NULL;
	memset(mqtt_message, 0, sizeof(mqtt_message));
	if(mode == true)
	{
		W25QXX_ReadDatas(mqtt_message, WIFI_FLASH_ADDR, MESSAGE_MAX);
	}
	else 
	{
		strcpy((char *)mqtt_message, (char *)WifiUsart.RxBuff);
	}
	if(strstr((char *)mqtt_message, "ssid") != NULL)
	{
		sp = (unsigned char *)(strstr((char *)mqtt_message, ":") + 1);
		if(mode == false)
		{
			sp = (unsigned char *)(strstr((char *)sp, ":") + 1);
		}
		for(cnt=0; cnt<MESSAGE_MAX; cnt++)
		{
			if(*sp == ' ')
				break;
			*ssid++ = *sp++;
		}
		sp = (unsigned char *)(strstr((char *)sp, ":") + 1);
		for(cnt=0; cnt<MESSAGE_MAX; cnt++)
		{
			if(*sp == 0xaa)	
				break;
			*pwsd++ = *sp++;
		}
	}else return false;
	return true;
}
/*********************************************************************************************************
* �� �� �� : MeasuringResponse
* ����˵�� : ���Ӧ��
* ��    �� : ack��ϣ����Ӧ������
* �� �� ֵ : ��NULL_Ϊ��Ӧ��
* ��    ע : ��
*********************************************************************************************************/ 
static unsigned char MeasuringResponse(unsigned char ack)
{
	switch(ack)
	{
		case AT_OK:
			if(strstr((char *)WifiUsart.RxBuff, "OK"))			return AT_OK;
			else if(strstr((char *)WifiUsart.RxBuff, "ERROR"))	return AT_ERR;	
			break;
		case AT_OK|AT_ERR:
			return AT_OK;
			break;
		case AT_IP:
			if(strstr((char *)WifiUsart.RxBuff, "192.168"))		return AT_IPOK;
			if(strstr((char *)WifiUsart.RxBuff, "0.0.0.0"))		return AT_IPERR;
			break;
		case DISCONNECT:
			if(strstr((char *)WifiUsart.RxBuff, "WIFI DISCONNECT"))			return DISCONNECT;
			if(strstr((char *)WifiUsart.RxBuff, "+MQTTDISCONNECTED"))		return DISCONNECT;
			break;
	}
	return NULL_;
}
/*********************************************************************************************************
* �� �� �� : RevampState
* ����˵�� : �޸�״̬
* ��    �� : cmd����״̬Ҫ���͵����sta����������������ת��ʲô״̬��bef��ʲô״̬���������,
			 nex���ȴ���ɺ���һ��״̬��to����ʱʱ�䣬inte������һ��״̬�ļ��ʱ��
* �� �� ֵ : ��
* ��    ע : ���cmdΪNULL����ô��ʾֻ��ת״̬������ָ��
*********************************************************************************************************/ 
void RevampState(char *cmd, unsigned char sta, unsigned char bef, unsigned char nex, unsigned char ack, unsigned short to, unsigned short inte)
{
	memset(WifiUsart.RxBuff, 0, sizeof(WifiUsart.RxBuff));
	WifiUsart.RecLen = 0;
	if(cmd != NULL)
		Usart2_SendString((unsigned char *)cmd);	
	state = sta;		//�����������ȴ�Ӧ��״̬
	before = bef;		//��¼��һ��״̬
	next = nex;			//��һ��Ҫִ�е�״̬
	wishack	  = ack;	//ϣ����Ӧ��
	timeout = to;		//��ʱʱ��Ϊ100ms
	interval = inte;	//����һ��״̬�ļ��ʱ��Ϊ100ms
	response = UNKNOWN;	//�����һ���յ���Ӧ��
}
/*********************************************************************************************************
* �� �� �� : JumpState
* ����˵�� : ״̬��ת
* ��    �� : sta����ǰ״̬��mode��true��һ��״̬��false��һ��״̬
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
static void JumpState(unsigned char sta, unsigned char offset)
{
	if(sta == AT_MQTTSUB)	state = FINISH;
	else					state = offset;
}
/*********************************************************************************************************
* �� �� �� : mstrcat
* ����˵�� : �ַ�������
* ��    �� : s1��Ŀ�꣬ s2��Դ
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
static void mstrcat(char *s1, const char *s2)
{
	if(*s1 != NULL)
		while(*++s1);
	while((*s1++ = *s2++));
}	
/*********************************************************************************************************
* �� �� �� : CalculateSha1
* ����˵�� : ����sha1�ܳ�
* ��    �� : password���ܳ״�Ż�����
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
static void CalculateSha1(unsigned char *password)
{
	unsigned char temp[3] = {0};
	unsigned char cnt = 0;
	hmac_sha1((unsigned char *)DeviceSecret,32,(unsigned char *)Encryption,46,mqtt_message);
	memset(temp, 0, sizeof(temp));
	for(cnt=0;cnt<20;cnt++)
	{
		sprintf((char *)temp,"%02X",mqtt_message[cnt]);
		mstrcat((char *)password,(char *)temp);
	}
}

unsigned char Get_Esp12ConnectionStatus(void)
{
	return ConnectFlag;
}
void Set_Esp12ConnectionStatus(unsigned char value)
{
	ConnectFlag = value;
}
/*******************************************************
*��������CRC16_Compute
*��  �ܣ�У�����
*��  ������ҪУ�������ָ�룬��ҪУ������ݳ���
*��  �أ�У����
*��  ע����
********************************************************/
static unsigned short CRC16_Compute(unsigned char *buffer, unsigned short len)
{
		extern const unsigned short crc16_list[256];//У�������
    unsigned short crc_code = 0xFFFF;//У�����ֵ
    unsigned int i = 0; //CRCУ�������ֵ
    while (len--)
    {
        i = (crc_code&0xff) ^ ((*buffer) & 0xff); //����CRC
        crc_code = ((crc_code>>8) & 0xff) ^ crc16_list[i];
			buffer++;
    }
    return crc_code ;	//����У����  (���ֽ�+���ֽ�)
}
/*crcУ���*/
const unsigned short crc16_list[256] =
{
	0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,   
	0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,   
	0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,   
	0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,   
	0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,   
	0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,   
	0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,   
	0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,   
	0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,   
	0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,   
	0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,   
	0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,   
	0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,   
	0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,   
	0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,   
	0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,   
	0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,  
	0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,   
	0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,   
	0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,   
	0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,   
	0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,   
	0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,   
	0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,   
	0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,   
	0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,   
	0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,   
	0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,   
	0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,   
	0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,   
	0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,   
	0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040 
};



