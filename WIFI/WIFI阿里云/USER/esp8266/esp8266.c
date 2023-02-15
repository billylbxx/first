#include "esp8266.h"
#include "usart2.h"
#include "systick.h"
#include "led.h"
#include "hmacsha1.h"
#include "ADC.h"
static unsigned char ConnectFlag = false;		//true：已连接，false：未连接
static void CalculateSha1(unsigned char *password);
static void mstrcat(char *s1, const char *s2);



void ESP12_AP_Init(void)
{
	ESP12_Send_Cmd("AT\r\n","OK",10,3);//测试指令AT
	ESP12_Send_Cmd("AT+CWMODE=2\r\n","OK",30,3);   //配置WIFI AP模式
	ESP12_Send_Cmd("AT+CWSAP=\"WWW888\",\"0123456789\",11,4\r\n","OK",30,3);  //设置wifi账号与密码
	ESP12_Send_Cmd("AT+RST\r\n","ready",800,3); //重启
	ESP12_Send_Cmd("AT+CIPMUX=1\r\n","OK",50,3); //开启多个连接
	ESP12_Send_Cmd("AT+CIPSERVER=1,5000\r\n","OK",50,3); //开启服务器设置端口号
}



void ESP12_STA_Init(void)
{
	char AT_CMD[150]={0};
	u8 PassWord[50] = {0};  //存放的是哈希计算的密钥
	
	RST:
	ESP12_Send_Cmd("AT\r\n","OK",100,3);//测试指令AT;
	//配置WIFI STA
	ESP12_Send_Cmd("AT+CWMODE=1\r\n","OK",300,3);
	//设置时区  NTSP服务器  用于调整客户端自身所在系统的时间，达到同步时间的目的	
	ESP12_Send_Cmd("AT+CIPSNTPCFG=1,8,\"ntp1.alliyun.com\"\r\n","OK",100,3);
	//连接wifi 账号&密码
	sprintf(AT_CMD,"AT+CWJAP=\"%s\",\"%s\"\r\n",SSID,PWD);
	if(ESP12_Send_Cmd(AT_CMD,"OK",3000,3)==0)
	{
		printf("WIFI名称或密码有错,复位重启\r\n");
		ESP12_Send_Cmd("AT+RST\r\n","ready",1000,1);  //wifi连接不上，重启
		goto RST;
	}
	
	memset(AT_CMD,0,sizeof(AT_CMD));  //清0数组，备用
	CalculateSha1(PassWord);	//计算哈希     
//	printf("haxi=%s\r\n",PassWord);
	sprintf(AT_CMD,"AT+MQTTUSERCFG=0,1,\"NULL\",\"%s\",\"%s\",0,0,\"\"\r\n", UserName, PassWord);
	ESP12_Send_Cmd(AT_CMD,"OK",2000,3);
	
	//设置连接客户端ID
	memset(AT_CMD,0,sizeof(AT_CMD));  //清0数组，备用，memset函数作用在一段内存块写入某个值
	sprintf(AT_CMD,"AT+MQTTCLIENTID=0,\"%s\"\r\n",ClientId);
	ESP12_Send_Cmd(AT_CMD,"OK",1000,3);
	
	//连接到MQTT代理（阿里云平台）
	memset(AT_CMD,0,sizeof(AT_CMD));
	sprintf(AT_CMD,"AT+MQTTCONN=0,\"%s\",1883,1\r\n",IP);
	if(ESP12_Send_Cmd(AT_CMD,"OK",1000,3)==0)
	{
		printf("连接aliyu失败,复位STM32重连\r\n");
		ESP12_Send_Cmd("AT+RST\r\n","ready",1000,2);  //wifi连接不上，重启  1000延时1S    2链接次数
		__set_FAULTMASK(1); //STM32程序软件复位
		NVIC_SystemReset();
	}
	
	//订阅主题
	memset(AT_CMD,0,sizeof(AT_CMD));
	sprintf(AT_CMD, "AT+MQTTSUB=0,\"%s\",1\r\n", PublishMessageTopSet);
	ESP12_Send_Cmd(AT_CMD,"OK",1000,3);
	printf("连接aliyu成功\r\n");
	Publish_MQTT_message(publish_mqtt_message,4); //发布主题
	
}    




//参数：1.指令，2.想要的应答，3.等待应答延时。while 4.重发次数
bool ESP12_Send_Cmd(char *cmd,char *ack,u16 waitms,u8 cnt)
{	
	SendUsart2Data((u8*)cmd);//1.发送AT指令
	printf("%s\r\n",cmd); //串口1输出
	while(cnt--)
	{
		delay_ms(waitms);
		//2.串口中断接收wifi应答
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
	//3.延时些许时间等待中断接收应答，判断应答  回复的应答是字符串 例如"ok" strcmp  strstr
	/*if(strstr(Rx_buf,"ok")!=NULL)
	{
			//成功，得到想要的回复
	}
	else
	{
		//失败，再发3-5次再重复判断。
	//发送3-5次之后还是失败，应该复位一下模块 RST
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




/***************************韦工************************************/
//Data_TypeDef DATA;
JSON json;  //存放解析处理的键值对
/**********************************************************************
*函数名：JsonString_Parsing
*功  能：解析阿里云服务器下发的数据json格式
*参  数：str：指向串口2接收的阿里云下发数据 json：解析后数据保存的结构体
*返  回：无
*备  注：JsonString_Parsing(Usart2.RxBuff, &json);
+MQTTSUBRECV:0,"/sys/a1prIyDGvtv/ST01/thing/service/property/set",99,{"method":"thing.service.property.set",
"id":"937812926","params":{"LEDSwitch":0},"version":"1.0.0"}
**********************************************************************/
void JsonString_Parsing(char *str, JSON *json)
{
	char str1[100]={0};//LEDSwitch":0},"version":"1.0.0"}
	char str2[50]={0};

	strcpy(str1,strstr(str,"params")+10);  //"params"偏移10个地址开始拷贝
	//strcspn：检索字符串 str1 开头连续有几个字符都不含字符串 str2 中的字符
	strncpy(str2,str1,strcspn(str1,"}"));  //拷贝strcspn(str1,"}"返回的字符个数
	//此时str2的冒号后面就是我们需要的值
	strcpy(json->values,strstr(str2,":")+1);	
	//键在哪里？双引号前面就是键
	strncpy(json->key,str2,strcspn(str1,"\""));
}

/**********************************************************************
*函数名：JsonString_Dispose
*功  能：使用解析后的数据进行控制
*参  数：json：解析后数据保存的结构体
*返  回：无
*备  注：JsonString_Dispose(&json);;
**********************************************************************/
void JsonString_Dispose(JSON *json)
{
		u16 i=0;
    u8 state = 0;
    if(strstr(json->key, "LEDSwitch"))
		{
		//服务器下发的数据是字符串转为整型数,atoi(json->values)
//		DATA.LEDSwitch = atoi(json->values); 
		state = atoi(json->values)? 0 : 1;   
		if(state==0)
		{
			LED1= 0;
			printf("开灯\r\n");
		}
		else
		{
			LED1= 1;
			printf("关灯\r\n");
		}	
	}
	
	
	
	
	
	
	
	
	
	
	
	
//	else if(strstr(json->key, "lightingFunction"))
//	{
//		//服务器下发的数据是字符串转为整型数,atoi(json->values)
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
//			//步进电机控制
//		}
//		//步进电机停止
//		if(state==0)
//		{
//			
//		}
//		else
//		{
//			
//		}
//	}
//	//直流马达
//	 else if(strstr(json->key, "KitchenVentilator_MotorStall"))
//     {
//		//服务器下发的数据是字符串转为整型数,atoi(json->values)
//		DATA.KitchenVentilator_MotorStall = atoi(json->values);
//		motor_contorl(DATA.KitchenVentilator_MotorStall);//调用直流电机驱动代码
//	 }
//	else if(strstr(json->key, "RGBColor"))
//	{
//		 RGBDisplay(json->values);//控制RGB    
//	}
}







/*********************************************************************************************************
* 函 数 名 : mstrcat
* 功能说明 : 字符串连接
* 形    参 : s1：目标， s2：源
* 返 回 值 : 无
* 备    注 : 无 s1[100]="123"
*********************************************************************************************************/ 
static void mstrcat(char *s1, const char *s2)
{
	if(*s1 != NULL)
		while(*++s1);
	while((*s1++ = *s2++));
}	
/*********************************************************************************************************
* 函 数 名 : CalculateSha1
* 功能说明 : 计算sha1密匙
* 形    参 : password：密匙存放缓冲区
* 返 回 值 : 无
* 备    注 : 无
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
* 函 数 名 : Esp12SendCmd
* 功能说明 : 向esp12模组发送指令并等待希望的应答
* 形    参 : cmd：发送的命令，ack：希望的应答，waittime：等待应答的时间（ms）
* 返 回 值 : 1没有得到希望的应答，0得到了希望的应答
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char Esp12SendCmd(char *cmd, char *ack, unsigned short waittime)
{
	Usart2.FlagLen = 0;						//标示位、长度清零
	waittime /= 10;                   			//超时时间
	SendUsart2Data((unsigned char *)cmd);		//发送命令
    while(--waittime)	//等待串口接收完毕或超时退出
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

//初始化
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
//发布主题 ，上发多个数据
void Publish_MQTT_message(JSON_PUBLISH *data,u8 data_num)  
{
	char AT_CMD[384]={0};
	char params[256]={0},i,*sp;

	
	sp=params;
	
	sprintf(data[0].value,"%.2f",len);   //把传感器的值赋值给json结构体的value
	sprintf(data[1].value,"%d",X);//(unsigned short)filter(12,ADC_Channel_10));
	sprintf(data[2].value,"%.2f",hh);
	sprintf(data[3].value,"%d",90);
	
	//          4
	for(i=0;i<data_num;i++)
	{        // 3
		if(i<(data_num-1))
		{   
			sprintf(sp,"%s%s%s",data[i].keyname,data[i].value,"\\,");
			while(*sp!=0) {sp++;} //防止覆盖
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


//RST  换成复位单片机   加断开连接


