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
unsigned char Router[]="AT+CWSAP=\"MY_wifi\",\"0123456789\",11,3\r\n";  //���ó�·���� ����ΪMY_wifi ����0123456789
unsigned char RST[]="AT+RST\r\n";
unsigned char M_Connection[]="AT+CIPMUX=1\r\n";
unsigned char SERVER[]="AT+CIPSERVER=1,8080\r\n"; //�˿ں�8080
unsigned char SEND[]="AT+CIPSEND\r\n";//AT+CIPSEND= ��������
unsigned char cx[]="AT+CIFSR\r\n";//AT+CIPSEND= ��������
unsigned char fs[]="AT+CIPSEND=0,10\r\n";//
 
 
 void wifi_init(void){
	uart1_init(115200);	    //���ڳ�ʼ��������Ϊ115200  wifiģ��������õĲ�����Ϊ115200
	
	SendUsart2Data(AT);   
	delay_ms(1000);
	
	SendUsart2Data(MODE);   
	delay_ms(1000);
	
	SendUsart2Data(Router);   //����wifiģ���·���� ��Ӧ��·�����ָ�����
	delay_ms(1000);                          //���ⲽ�ֻ����ߵ��ԾͿ����ѵ�����Ϊmy_wifi��wifi

	SendUsart2Data(RST);   //����ģ��
	delay_ms(1000);

	SendUsart2Data(M_Connection);  //����������
	delay_ms(1000);
 
	SendUsart2Data(SERVER);  //���óɷ����� �� ���ö˿ں�8080
	delay_ms(2000);                         //���ⲽ���ֻ�APP�������õ�IP���˿ںžͿ���������  wifiģ���IPһ��̶�Ϊ192.168.4.1    (����������д)
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
				printf("led1�Ѵ�\n");
			}
			if(strcmp("ledoff",p)==0){
				LED1 = 1;
			}
			
		}
	
		
	}
	
	return 0;
}
