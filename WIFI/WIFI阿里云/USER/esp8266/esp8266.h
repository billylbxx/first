#ifndef _ESP8266_H_
#define _ESP8266_H_

#include "stm32f4xx.h" 
#include "string.h"  
#include "stdio.h"  
#include "stdbool.h"
#include <stdlib.h> 

#define  SSID    "zxtplus"          //wifi热点名称
#define  PWD     "123456789"       //wifi热点密码
#define  IP      "203.107.45.14"   //服务器IP或域名
//#define  IP      "a1prIyDGvtv.iot-as-mqtt.cn-shanghai.aliyuncs.com"   //服务器IP或域名
#define  PORT    1883				//连接端口号，MQTT默认1883

//阿里云三元组
#define DeviceName 		"ZXTU"  								//设备名称
#define ProductKey 		"a1f6LVr9yHh"						//产品密匙
#define DeviceSecret	"8f14dd79276faff7e3948e43670c9685"	//设备密匙
  
                    

//计算哈希算法用到  加密
#define Client 		"clientId123deviceName"					
#define productKey  "productKey"
#define Encryption  (Client DeviceName productKey ProductKey)
//AT指令的
#define AND "&"
#define ClientId "123|securemode=3\\,signmethod=hmacsha1|"		//客户端ID
#define UserName (DeviceName AND ProductKey)					//用户名
                     
//订阅发布的主题
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
	struct RGB Rgb;	 //数组类型的值读取
	char key[50];	 //键名读取
	char values[20];//读取到的值，类型字符串
	short value;	 //普通值读取
}JSON;
//上传数据结构体
typedef struct
{
	char keyname[50];	 //键读取
	char value[20];//读取到的值，类型字符串
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


