#ifndef _ESP8266_H_
#define _ESP8266_H_

#include "stm32f4xx.h" 
#include "string.h"  
#include "stdio.h"  
#include "stdbool.h"
#include <stdlib.h> 

#define  SSID    "zxtplus"          //wifi�ȵ�����
#define  PWD     "123456789"       //wifi�ȵ�����
#define  IP      "203.107.45.14"   //������IP������
//#define  IP      "a1prIyDGvtv.iot-as-mqtt.cn-shanghai.aliyuncs.com"   //������IP������
#define  PORT    1883				//���Ӷ˿ںţ�MQTTĬ��1883

//��������Ԫ��
#define DeviceName 		"ZXTU"  								//�豸����
#define ProductKey 		"a1f6LVr9yHh"						//��Ʒ�ܳ�
#define DeviceSecret	"8f14dd79276faff7e3948e43670c9685"	//�豸�ܳ�
  
                    

//�����ϣ�㷨�õ�  ����
#define Client 		"clientId123deviceName"					
#define productKey  "productKey"
#define Encryption  (Client DeviceName productKey ProductKey)
//ATָ���
#define AND "&"
#define ClientId "123|securemode=3\\,signmethod=hmacsha1|"		//�ͻ���ID
#define UserName (DeviceName AND ProductKey)					//�û���
                     
//���ķ���������
#define SYS "/sys/"
#define LINK "/"
#define TOP "/thing/event/property/"
#define POST "post"
#define ESET  "set"
#define PublishMessageTopPost 	(SYS ProductKey LINK DeviceName TOP POST)
#define PublishMessageTopSet 	(SYS ProductKey LINK DeviceName TOP ESET)



struct RGB
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
};

typedef struct
{
	struct RGB Rgb;	 //�������͵�ֵ��ȡ
	char key[50];	 //������ȡ
	char values[20];//��ȡ����ֵ�������ַ���
	short value;	 //��ֵͨ��ȡ
}JSON;
//�ϴ����ݽṹ��
typedef struct
{
	char keyname[50];	 //����ȡ
	char value[20];//��ȡ����ֵ�������ַ���
}JSON_PUBLISH;


void ESP12_AP_Init(void);
bool ESP12_Send_Cmd(char *cmd,char *ack,u16 waitms,u8 cnt);
u8 ESP12_Send_TO_Client(u8 id,char * data);
void ESP12_STA_Init(void);

void JsonString_Parsing(char *str, JSON *json);
void JsonString_Dispose(JSON *json);
void Publish_MQTT_message(JSON_PUBLISH *data,u8 data_num);


extern JSON json;
extern JSON_PUBLISH publish_mqtt_message[4];



#endif


