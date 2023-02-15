#include <stdio.h>
#include "io_bit.h"
#include "uart1.h"
#include "systick.h"
#include "led.h"
#include "uart1.h"
#include "uart1.h"
#include "usart2.h"
 void wifi_init(void);
 
unsigned char AT[]="AT\r\n";
unsigned char MODE[]="AT+CWMODE=3\r\n";
unsigned char Router[]="AT+CWSAP=\"MY_wifi\",\"0123456789\",11,3\r\n";  //配置成路由器 名字为MY_wifi 密码0123456789
unsigned char RST[]="AT+RST\r\n";
unsigned char M_Connection[]="AT+CIPMUX=1\r\n";
unsigned char SERVER[]="AT+CIPSERVER=1,8080\r\n"; //端口号8080
unsigned char SEND[]="AT+CIPSEND\r\n";//AT+CIPSEND= 发送数据
unsigned char cx[]="AT+CIFSR\r\n";//AT+CIPSEND= 发送数据
unsigned char fs[]="AT+CIPSEND=0,10\r\n";//
 
 
 void wifi_init(void){
	uart1_init(115200);	    //串口初始化波特率为115200  wifi模块出厂配置的波特率为115200
	
	SendUsart2Data(AT);   
	delay_ms(1000);
	
	SendUsart2Data(MODE);   
	delay_ms(1000);
	
	SendUsart2Data(Router);   //配置wifi模块成路由器 相应的路由名字跟密码
	delay_ms(1000);                          //到这步手机或者电脑就可以搜到名字为my_wifi的wifi

	SendUsart2Data(RST);   //重启模块
	delay_ms(1000);

	SendUsart2Data(M_Connection);  //开启多连接
	delay_ms(1000);
 
	SendUsart2Data(SERVER);  //配置成服务器 与 设置端口号8080
	delay_ms(2000);                         //到这步打开手机APP输入设置的IP跟端口号就可以连接了  wifi模块的IP一般固定为192.168.4.1    (启明欣欣编写)
//	SendUsart2Data(cx); 

}
int main(){
	char* p;
	Systick_Inti(168);
	Led_Init();
	uart1_init(115200);
	Usart2_Init(115200);
	wifi_init();
	
while(1){
		if(uart1_flag==1)//
		{
			uart1_flag=0;
//			printf("buff:%s\n",uart_buf);  
		}
		if(Usart2.RecFlag==1){
			Usart2.RecFlag=0;
//			printf("buff:%s\n",Usart2.RxBuff);
			p=strchr((char*)Usart2.RxBuff,':');
			p++;
//				printf("p=%s\n",p);//buff: :ledoff
//			  printf("len=%d\n",strlen(p));//buff: :ledoff
		
			if(strcmp("ledon\r\n",p)==0){
				LED1 = 0;
				printf("led1已打开\n");
			}
			if(strcmp("ledoff",p)==0){
				LED1 = 1;
			}
			
		}
	
		
	}
	
	return 0;
}
