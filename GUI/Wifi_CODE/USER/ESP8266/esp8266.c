#include "esp8266.h"
#include "usart2.h"
#include "systick.h"
//���ܣ� Esp8266��������
//������cmd ,��Ҫ���͵�����
//      ack ,�ڴ�����Ӧ
//����ֵ��1��������ȷ��0��ʧ��
int ESP8266_SendCmd(char* cmd,char * ack,int waitTime)
{
	u8 res = 0;
	Usart2_SendString((unsigned char*)cmd);
	while(1)
	{
		if(Usart2.RecFlag)
		{
			if(strstr((char*)Usart2.RxBuff,ack))
			{
				memset(&Usart2,0,sizeof(Usart2));
				res = 1;
				break;
			}	
		}
		waitTime--;
		Delay_Ms(1);
		if(waitTime<=0)
		{	
			res = 0;
			break;
		}
	}
	return res;
}

void ESP8266_ConnectAlliyun(void) //clean
{
	ESP8266_SendCmd("AT+MQTTCLEAN=0\r\n","OK",500); //���֮ǰ�Ѿ����ӣ���ô�ȹص�
	ESP8266_SendCmd("AT+RST\r\n","ready",2000);  //�ȸ�λ
	if(ESP8266_SendCmd("AT\r\n","OK",500))
	{
		printf("AT OK\r\n");
	}
	if(ESP8266_SendCmd("AT+CWMODE=1\r\n","OK",500)) //�豸STAģʽ
	{
		printf("AT+CWMODE=1 OK\r\n");
	}
	if(ESP8266_SendCmd("AT+CIPSNTPCFG=1,8,\"ntp1.alliyun.com\"\r\n","OK",500))
	{
		printf("����ʱ����NTP������ OK\r\n");
	}
	if(ESP8266_SendCmd("AT+CWJAP=\"iPhone\",\"12345678\"\r\n","OK",5000))
	{
		printf("�����ȵ� OK\r\n");
	}
	
	if(ESP8266_SendCmd("AT+MQTTUSERCFG=0,1,\"NULL\",\"xing&a1qF2DrnjnL\",\"84DC0F391310524622BB8C66C8095902618EC25E\",0,0,\"\"\r\n","OK",5000))
	{

		printf("MQTT���� OK\r\n");
	}
	if(ESP8266_SendCmd("AT+MQTTCLIENTID=0,\"xyd-xing|securemode=3\\, \
		 signmethod=hmacsha1|\"\r\n","OK",5000))
	{
		printf("MQTT�ͻ���ID���� OK\r\n");
	}
	if(ESP8266_SendCmd("AT+MQTTCONN=0,\"a1qF2DrnjnL.iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883,1\r\n","OK",5000))
	{
		printf("MQTT ���� OK\r\n");
	}
	                                  //sys/a1qF2DrnjnL/xing/thing/service/property/set
	if(ESP8266_SendCmd("AT+MQTTSUB=0,\"/sys/a1qF2DrnjnL/xing/thing/service/property/set\",1\r\n","OK",5000))
	{
		printf("MQTT ���� OK\r\n");
	}
}

int ESP8266_Pub(void)	/*���βο����޸ĳɽṹ��ָ��,���������޸�*/
{	
 char mqtt_message[100]= {0};
	sprintf((char *)mqtt_message,	
		"AT+MQTTPUB=0,\"/sys/a1qF2DrnjnL/xing/thing/event/property/post\",\"{\\\"method\\\":\\\"thing.service.property.set\\\"\\,\\\"id\\\":\\\"735662328\\\"\\,\
	    \\\"params\\\":{\\\"CuTemperature\\\":66\\,\\\"CurrentHumidity\\\":88}\\,\\\"version\\\":\\\"1.0.0\\\"}\",1,0\r\n");
	return ESP8266_SendCmd(mqtt_message,"OK",4000);
}

