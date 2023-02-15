#ifndef __ESP8266_H
#define __ESP8266_H

#include "io_bit.h"
#include "usart2.h"

/***************************************************************************/
/**********************用户需要修改的，其他不要动***************************/
//#define SSID "OPPOR9s"									//热点名称
//#define PWSD "mwy@@123321"								//热点密码
//#define SSID "vivo"									//热点名称
//#define PWSD "12345678"								//热点密码
#define MODE "TCP"										//连接方式
#define IP   "203.107.45.14"							//服务器IP或域名，203.107.45.14	深圳
#define PORT 1883										//连接端口号，MQTT默认1883

#define WifiUsart Usart2								//与esp8266通信的串口的数据结构体

#define DeviceName 		"XYD1"								//设备名称
#define ProductKey 		"a1AhmBTTXQG"						//产品密匙
#define DeviceSecret	"709dbcd747d61ceecdce52aa8af6481e"	//设备密匙
/***************************************************************************/
/***************************************************************************/

#define SYS "/sys/"
#define LINK "/"
#define TOP "/thing/event/property/"
#define POST "post"
#define SET  "set"
#define PublishMessageTopPost 	(SYS ProductKey LINK DeviceName TOP POST)
#define PublishMessageTopSet 	(SYS ProductKey LINK DeviceName TOP SET)

#define Client 		"clientId123deviceName"					
#define productKey  "productKey"
#define Encryption  (Client DeviceName productKey ProductKey)


#define AND "&"
#define ClientId "123|securemode=3\\,signmethod=hmacsha1|"		//客户端ID
#define UserName (DeviceName AND ProductKey)					//用户名
//#define PassWord "20C185291C1767917341AF3960B15A17D86CB653" 	//密码		//由代码自动生成

void Esp12_Init(void);
void PublishMessage(void *StructMessage);
void TaskEsp12Msg_Handle(void);
void PublishMessage(void *StructMessage);
unsigned char Get_Esp12ConnectionStatus(void);
void Set_Esp12ConnectionStatus(unsigned char value);
void RevampState(char *cmd, unsigned char sta, unsigned char bef, unsigned char nex, unsigned char ack, unsigned short to, unsigned short inte);
unsigned char CheckFlashOrBuffer(unsigned char *ssid, unsigned char *pwsd, char mode);

extern unsigned char ConnectFlag;

#define WIFI_DEGBUG 1
#define THREE 3
#define MESSAGE_MAX 50								//账号密码总长
#define UNIQUE_ID_ADDRESS 0x1FFF7A10	//STM32唯一ID地址

#define WIFI_SECTOR_NUM	2047					//扇区编号
#define WIFI_FLASH_ADDR 2047*4096			//账号密码存放地址

typedef enum
{
	NULL_ = 0,
	FREE,				//Detect the data issued by the platform when idle
	AT_AT,				//Tests AT startup
	ECHO,				//Configures echoing of AT commands
	AT_CWMODE1,			//Sets the Wi-Fi Mode(Station/SoftAP/Station+SoftAP)
	AT_CIFSR,			//Gets the Local IP Address
	AT_CWJAP,			//Connects to an AP
	AT_MQTTCLEAN,		//Close the MQTT Connection
	AT_CIPSNTPCFG,		//Configures the time domain and SNTP server
	AT_MQTTUSERCFG,		//Set MQTT User Config
	AT_MQTTCLIENTID,	//MQTT client configuration
	AT_MQTTCONN,		//Connect to MQTT Broker
	AT_MQTTSUB,			//Subscribe to MQTT Topic
	AT_RST,				//Restarts a module
	FINISH,				//Initialization completed
	DOWN_DATA,			//Platform Downlink Data
	
	AT_CWMODE2 = 16,	//Setting the site ap mode 
	AT_CIPMUX,			//Open multiple links
	AT_CWSAP,			//Configuration of the ESP12 SoftAP
	AT_CIPSERVER,		//Deletes/Creates TCP or SSL Server
	WAIT_CONF,			//Wait for the user to configure WiFi
	
	WAIT = 50,			//Waiting for Device Response
	AT_OK,				//Response OK
	AT_ERR,				//Response error
	AT_IP,				//Check if IP is obtained
	AT_IPOK,			//The IP has been obtained
	AT_IPERR,			//IP not obtained
	DISCONNECT,			//Hot spot disconnection
	MQTTDISCONNECTED,	//MQTT disconnected
	UNKNOWN,			//Unknown
}ESP12_STATE;

typedef enum
{
	WAIT_100 = 10,		//Waiting time unit：10ms，10*10=100ms
	WAIT_200 = 20,		//The following up
	WAIT_300 = 30,
	WAIT_400 = 40, 
	WAIT_500 = 50,
	WAIT_1000 = 100,
	WAIT_4000 = 400,
	WAIT_10000 = 1000,
	
	TRY_COUNT = 3,		//Retransmission times
}AT_TIME_OUT;

#endif

