#include "stm32f4xx.h"
#include  "led.h"
#include "beep.h"
#include "key.h"

void deley(int m);

int main(void)
{	 
	u8 key1_flag=1;
	u8 key2_flag=1;
	u8 m=0;
	
	Len_Init();
	Beep_Init();
	Key_Init();
	
	
	while(1)
	{
		//Èí¼þÏû¶¶
		if(KEY1)
		{
			deley(500);
			if((KEY1)&&(key1_flag))
			{
				BEEP = ~BEEP; 
				key1_flag=0;
			}
		}
		else
		{
			key1_flag=1;
		}
		
		if(!KEY2)
		{
			deley(500);
			if((!KEY2)&&(key2_flag))
			{
				m++;
			}
		}
		else
		{
			key2_flag=1;
		}
		
		switch(m)
		{
			case 1:	
				LED1=0;
				break;
			case 2:	
				LED2=0;
				break;
			case 3:	
				LED3=0;
				break;
			case 4:	
				LED1=1;
				LED2=1;
				LED3=1;
				m=0;
				break;
		}
	}
}

void deley(int m)
{
	int i,j;
	
	for(i=0;i<10000;i++)
	{
		for(j=0;j<m;j++)
		{
			;
		}
	}
}
