#include "esp8266.h"
#include "usart2.h"
#include "systick.h"
#include "led.h"
#include "hmacsha1.h"
#include "ADC.h"
static unsigned char ConnectFlag = false;		//true�������ӣ�false��δ����
static void CalculateSha1(unsigned char *password);
static void mstrcat(char *s1, const char *s2);



void ESP12_AP_Init(void)
{
	ESP12_Send_Cmd("AT\r\n","OK",10,3);//����ָ��AT
	ESP12_Send_Cmd("AT+CWMODE=2\r\n","OK",30,3);   //����WIFI APģʽ
	ESP12_Send_Cmd("AT+CWSAP=\"WWW888\",\"0123456789\",11,4\r\n","OK",30,3);  //����wifi�˺�������
	ESP12_Send_Cmd("AT+RST\r\n","ready",800,3); //����
	ESP12_Send_Cmd("AT+CIPMUX=1\r\n","OK",50,3); //�����������
	ESP12_Send_Cmd("AT+CIPSERVER=1,5000\r\n","OK",50,3); //�������������ö˿ں�
}



void ESP12_STA_Init(void)
{
	char AT_CMD[150]={0};
	u8 PassWord[50] = {0};  //��ŵ��ǹ�ϣ�������Կ
	
	RST:
	ESP12_Send_Cmd("AT\r\n","OK",100,3);//����ָ��AT;
	//����WIFI STA
	ESP12_Send_Cmd("AT+CWMODE=1\r\n","OK",300,3);
	//����ʱ��  NTSP������  ���ڵ����ͻ�����������ϵͳ��ʱ�䣬�ﵽͬ��ʱ���Ŀ��	
	ESP12_Send_Cmd("AT+CIPSNTPCFG=1,8,\"ntp1.alliyun.com\"\r\n","OK",100,3);
	//����wifi �˺�&����
	sprintf(AT_CMD,"AT+CWJAP=\"%s\",\"%s\"\r\n",SSID,PWD);
	if(ESP12_Send_Cmd(AT_CMD,"OK",3000,3)==0)
	{
		printf("WIFI���ƻ������д�,��λ����\r\n");
		ESP12_Send_Cmd("AT+RST\r\n","ready",1000,1);  //wifi���Ӳ��ϣ�����
		goto RST;
	}
	
	memset(AT_CMD,0,sizeof(AT_CMD));  //��0���飬����
	CalculateSha1(PassWord);	//�����ϣ     
//	printf("haxi=%s\r\n",PassWord);
	sprintf(AT_CMD,"AT+MQTTUSERCFG=0,1,\"NULL\",\"%s\",\"%s\",0,0,\"\"\r\n", UserName, PassWord);
	ESP12_Send_Cmd(AT_CMD,"OK",2000,3);
	
	//�������ӿͻ���ID
	memset(AT_CMD,0,sizeof(AT_CMD));  //��0���飬���ã�memset����������һ���ڴ��д��ĳ��ֵ
	sprintf(AT_CMD,"AT+MQTTCLIENTID=0,\"%s\"\r\n",ClientId);
	ESP12_Send_Cmd(AT_CMD,"OK",1000,3);
	
	//���ӵ�MQTT����������ƽ̨��
	memset(AT_CMD,0,sizeof(AT_CMD));
	sprintf(AT_CMD,"AT+MQTTCONN=0,\"%s\",1883,1\r\n",IP);
	if(ESP12_Send_Cmd(AT_CMD,"OK",1000,3)==0)
	{
		printf("����aliyuʧ��,��λSTM32����\r\n");
		ESP12_Send_Cmd("AT+RST\r\n","ready",1000,2);  //wifi���Ӳ��ϣ�����  1000��ʱ1S    2���Ӵ���
		__set_FAULTMASK(1); //STM32���������λ
		NVIC_SystemReset();
	}
	
	//��������
	memset(AT_CMD,0,sizeof(AT_CMD));
	sprintf(AT_CMD, "AT+MQTTSUB=0,\"%s\",1\r\n", PublishMessageTopSet);
	ESP12_Send_Cmd(AT_CMD,"OK",1000,3);
	printf("����aliyu�ɹ�\r\n");
	Publish_MQTT_message(publish_mqtt_message,4); //��������
	
}    




//������1.ָ�2.��Ҫ��Ӧ��3.�ȴ�Ӧ����ʱ��while 4.�ط�����
bool ESP12_Send_Cmd(char *cmd,char *ack,u16 waitms,u8 cnt)
{	
	SendUsart2Data((u8*)cmd);//1.����ATָ��
	printf("%s\r\n",cmd); //����1���
	while(cnt--)
	{
		delay_ms(waitms);
		//2.�����жϽ���wifiӦ��
		if(Usart2.RecFlag)
		{
			Usart2.RecFlag = 0;
			Usart2.FlagLen = 0;
			if(strstr((char*)Usart2.RxBuff,ack)!=NULL)
			{
				printf("ok\r\n");
				return 1;
			}
			memset(Usart2.RxBuff,0,sizeof(Usart2.RxBuff));
		}
	}
	Usart2.RecFlag = 0;
	Usart2.FlagLen = 0;
	return 0;
	//3.��ʱЩ��ʱ��ȴ��жϽ���Ӧ���ж�Ӧ��  �ظ���Ӧ�����ַ��� ����"ok" strcmp  strstr
	/*if(strstr(Rx_buf,"ok")!=NULL)
	{
			//�ɹ����õ���Ҫ�Ļظ�
	}
	else
	{
		//ʧ�ܣ��ٷ�3-5�����ظ��жϡ�
	//����3-5��֮����ʧ�ܣ�Ӧ�ø�λһ��ģ�� RST
	}
	*/
}


u8 ESP12_Send_TO_Client(u8 id,char * data)
{
	u8 send_buf[20]={0};
	sprintf((char*)send_buf,"AT+CIPSEND=%d,%d\r\n",id,strlen(data));
/*	SendUsart2Data(send_buf);
	delay_ms(20);
	if(strstr((char*)Usart2.RxBuff,">")!=NULL)
	{
			SendUsart2Data(data);
	}
	*/
	if(ESP12_Send_Cmd((char*)send_buf,">",20,3))
	{
		SendUsart2Data((u8*)data);
		return 1;
	}
	return 0;	
}




/***************************Τ��************************************/
//Data_TypeDef DATA;
JSON json;  //��Ž�������ļ�ֵ��
/**********************************************************************
*��������JsonString_Parsing
*��  �ܣ����������Ʒ������·�������json��ʽ
*��  ����str��ָ�򴮿�2���յİ������·����� json�����������ݱ���Ľṹ��
*��  �أ���
*��  ע��JsonString_Parsing(Usart2.RxBuff, &json);
+MQTTSUBRECV:0,"/sys/a1prIyDGvtv/ST01/thing/service/property/set",99,{"method":"thing.service.property.set",
"id":"937812926","params":{"LEDSwitch":0},"version":"1.0.0"}
**********************************************************************/
void JsonString_Parsing(char *str, JSON *json)
{
	char str1[100]={0};//LEDSwitch":0},"version":"1.0.0"}
	char str2[50]={0};

	strcpy(str1,strstr(str,"params")+10);  //"params"ƫ��10����ַ��ʼ����
	//strcspn�������ַ��� str1 ��ͷ�����м����ַ��������ַ��� str2 �е��ַ�
	strncpy(str2,str1,strcspn(str1,"}"));  //����strcspn(str1,"}"���ص��ַ�����
	//��ʱstr2��ð�ź������������Ҫ��ֵ
	strcpy(json->values,strstr(str2,":")+1);	
	//�������˫����ǰ����Ǽ�
	strncpy(json->key,str2,strcspn(str1,"\""));
}

/**********************************************************************
*��������JsonString_Dispose
*��  �ܣ�ʹ�ý���������ݽ��п���
*��  ����json�����������ݱ���Ľṹ��
*��  �أ���
*��  ע��JsonString_Dispose(&json);;
**********************************************************************/
void JsonString_Dispose(JSON *json)
{
		u16 i=0;
    u8 state = 0;
    if(strstr(json->key, "LEDSwitch"))
		{
		//�������·����������ַ���תΪ������,atoi(json->values)
//		DATA.LEDSwitch = atoi(json->values); 
		state = atoi(json->values)? 0 : 1;   
		if(state==0)
		{
			LED1= 0;
			printf("����\r\n");
		}
		else
		{
			LED1= 1;
			printf("�ص�\r\n");
		}	
	}
	
	
	
	
	
	
	
	
	
	
	
	
//	else if(strstr(json->key, "lightingFunction"))
//	{
//		//�������·����������ַ���תΪ������,atoi(json->values)
//		DATA.LEDSwitch = atoi(json->values); 
//		state = DATA.LEDSwitch? 0 : 1;  
//		LED2 = state;
////		LED3 = state;	     			
//	}
//    else if(strstr(json->key, "motorDirection"))
//	{
//		DATA.MotorDirection = atoi(json->values);
//		state = DATA.MotorDirection? 0 : 1;
//		for(i=0;i<800;i++)
//		{
//			//�����������
//		}
//		//�������ֹͣ
//		if(state==0)
//		{
//			
//		}
//		else
//		{
//			
//		}
//	}
//	//ֱ�����
//	 else if(strstr(json->key, "KitchenVentilator_MotorStall"))
//     {
//		//�������·����������ַ���תΪ������,atoi(json->values)
//		DATA.KitchenVentilator_MotorStall = atoi(json->values);
//		motor_contorl(DATA.KitchenVentilator_MotorStall);//����ֱ�������������
//	 }
//	else if(strstr(json->key, "RGBColor"))
//	{
//		 RGBDisplay(json->values);//����RGB    
//	}
}







/*********************************************************************************************************
* �� �� �� : mstrcat
* ����˵�� : �ַ�������
* ��    �� : s1��Ŀ�꣬ s2��Դ
* �� �� ֵ : ��
* ��    ע : �� s1[100]="123"
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
	unsigned char digest[30]={0};
	unsigned char cnt = 0;
	hmac_sha1((unsigned char *)DeviceSecret,32,(unsigned char *)Encryption,46,digest);
	memset(temp, 0, sizeof(temp));
	for(cnt=0;cnt<20;cnt++)
	{
		sprintf((char *)temp,"%02X",digest[cnt]);
		mstrcat((char *)password,(char *)temp);
	}
}

unsigned char Esp12_Get_ConnectionStatus(void)
{
	return ConnectFlag;
}
void Esp12_Set_ConnectionStatus(unsigned char value)
{
	ConnectFlag = value;
}

/*********************************************************************************************************
* �� �� �� : Esp12SendCmd
* ����˵�� : ��esp12ģ�鷢��ָ��ȴ�ϣ����Ӧ��
* ��    �� : cmd�����͵����ack��ϣ����Ӧ��waittime���ȴ�Ӧ���ʱ�䣨ms��
* �� �� ֵ : 1û�еõ�ϣ����Ӧ��0�õ���ϣ����Ӧ��
* ��    ע : ��
*********************************************************************************************************/ 
unsigned char Esp12SendCmd(char *cmd, char *ack, unsigned short waittime)
{
	Usart2.FlagLen = 0;						//��ʾλ����������
	waittime /= 10;                   			//��ʱʱ��
	SendUsart2Data((unsigned char *)cmd);		//��������
    while(--waittime)	//�ȴ����ڽ�����ϻ�ʱ�˳�
    {	
		if(Usart2.RecFlag == true)
		{
			if(strstr((char *)Usart2.RxBuff, (char *)ack))
			{
				Usart2.FlagLen = 0;
				Usart2.RecFlag = false;
				//printf("%s\r\n", WifiUsart.RxBuff);
				break;
			}
			Usart2.FlagLen = 0;
			Usart2.RecFlag = false;
			//printf("%s\r\n", WifiUsart.RxBuff);
		}
		delay_ms(10);
    }
   if(waittime)	return 1;                       
   return 0; 
}

//��ʼ��
JSON_PUBLISH publish_mqtt_message[4]=        
{
	{ "\\\"temperature\\\":","0.00" },
	{ "\\\"LightLux\\\":","0" },
	{ "\\\"Humidity\\\":","0.00" },
	{ "\\\"GasConcentration\\\":","0" }
}; 
extern float len;
extern int X;
extern float hh;
extern int sum[3];
//�������� ���Ϸ��������
void Publish_MQTT_message(JSON_PUBLISH *data,u8 data_num)  
{
	char AT_CMD[384]={0};
	char params[256]={0},i,*sp;

	
	sp=params;
	
	sprintf(data[0].value,"%.2f",len);   //�Ѵ�������ֵ��ֵ��json�ṹ���value
	sprintf(data[1].value,"%d",X);//(unsigned short)filter(12,ADC_Channel_10));
	sprintf(data[2].value,"%.2f",hh);
	sprintf(data[3].value,"%d",90);
	
	//          4
	for(i=0;i<data_num;i++)
	{        // 3
		if(i<(data_num-1))
		{   
			sprintf(sp,"%s%s%s",data[i].keyname,data[i].value,"\\,");
			while(*sp!=0) {sp++;} //��ֹ����
		}
//"\\\"temperature\\\":20\\,\\\"humidity\\\":30\\,\\\"harmfulGas\\\":40\\,\\\"GasConcentration\\\":50"
		else
			sprintf(sp,"%s%s",data[i].keyname,data[i].value);
	}
//	printf("data=%s\r\n",params);
//	sprintf(AT_CMD,"AT+MQTTPUB=0,\"%s\",\"{\\\"params\\\":{\\\"temperature\\\":50\\,\\\"humidity\\\":77}}\",1,0\r\n",PublishMessageTopPost);
	sprintf(AT_CMD,"AT+MQTTPUB=0,\"%s\",\"{\\\"params\\\":{%s}}\",1,0\r\n",PublishMessageTopPost,params);
	ESP12_Send_Cmd(AT_CMD,"OK",1000,1);		
}


//RST  ���ɸ�λ��Ƭ��   �ӶϿ�����


