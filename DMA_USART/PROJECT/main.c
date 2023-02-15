#include "stm32f4xx.h"
#include "led.h"               
#include "Beep.h"
#include "key.h"
#include "uart.h"
#include "motor.h"
#include "exti.h"
#include "systick.h"
#include "tim.h"
#include "tim7.h"
#include "tim3.h"
#include "ADC.h"
#include "DMA.h"
#include <string.h>
//void delay(int m);
void fff(USART_TypeDef* USARTx, char*string);
void tt(char*sp);
void qaz(char*str);
u32 addr=0;
int main()
{	
	Led_Init();
	Beep_Init();
	key_Init();
	Uart_Init();
	Motor_Init();
	Systick_Init(168);
	//tim6_Init(50000,84);
	//tim7_Init(50000,84);
	//tim3_Init(500,840);
	//tim2_Init(500,840);
	//exti_Init();
	ADC1_Init();
	//Delay_ms(500);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//同一组优先级别中，不同的抢占级别之间，其中一抢占级别正在做事，另外抢占级别不能打断他
	dma_Init();
	myDMA_ENABLE();
	while(1)
	{
	
				

	}
}



