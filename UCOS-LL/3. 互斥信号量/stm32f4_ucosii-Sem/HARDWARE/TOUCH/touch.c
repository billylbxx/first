#include "touch.h"
#include "bsp_lcd.h"

TOUCHDef touch={0};

void Touch_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOB,C,F时钟

  //GPIOB1,2初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;//PB1/PB2 设置为上拉输入
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//PB0设置为推挽输出
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出模式
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//PC13设置为推挽输出
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出模式
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化	

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PF11设置推挽输出
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出模式
  GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化	

  uint8_t buf[50];
  sFLASH_ReadBuffer(buf, T_PARAM_ADDR, 50);
  if(buf[0] != ADJUSTFLAG)
  {
    adjust();
  }
  else
  {
    memcpy(&touch.A, &buf[1], 4);
    memcpy(&touch.B, &buf[5], 4);
    memcpy(&touch.C, &buf[9], 4);
    memcpy(&touch.D, &buf[13], 4);
    memcpy(&touch.E, &buf[17], 4);
    memcpy(&touch.F, &buf[21], 4);
  }
}

uint8_t Touch_ReadWrite(uint8_t data)
{
	uint8_t i,recv = 0;
	for(i = 0;i < 8; i++)
	{
		 T_SCK(0);
	   T_DIN(data & (1 << (7-i)));
		 delay_us(1);
		 T_SCK(1);
		 delay_us(1);
		
		 recv |= T_DOUT() << (7-i);
	}
	return recv;
}

uint16_t Get_AD(uint8_t xycmd)
{
	uint8_t temp1, temp2;
	T_CS(0);
  Touch_ReadWrite(xycmd);
  delay_us(6);
	
	T_SCK(0);	     	    
	delay_us(1);    	   
	T_SCK(1);		
	delay_us(1);    
	T_SCK(0);//给1个时钟，清除BUSY

	temp1 = Touch_ReadWrite(0x00);
	temp2 = Touch_ReadWrite(0x00);
	T_CS(1);
	return ((temp1<<8 | temp2) >> 4);
}

uint8_t touch_scan(void)
{
  if(T_PEN()==0)
  {
    return 1;
  }
  return 0;
}

uint16_t myabs(int16_t a)
{
  if(a >= 0)
    return a;
  else
    return -a;
}

#define ad_errvalue 10
uint8_t xy_adget(void)
{
  uint8_t i = 0;
	uint16_t xa[9];
	uint16_t ya[9];
	uint16_t xb[3];
	uint16_t yb[3];
	uint16_t mx[3];
	uint16_t my[3];
	for(i = 0;i < 9;i++)
	{
	   xa[i] = Get_AD(cmd_x);
		 ya[i] = Get_AD(cmd_y);
	}
  
	for(i = 0;i < 3;i++)
	{
	   xb[i] = (uint16_t)((xa[i*3]+xa[i*3+1]+xa[i*3+2])/3.+0.5);
	   yb[i] = (uint16_t)((ya[i*3]+ya[i*3+1]+ya[i*3+2])/3.+0.5);
	}
	
	for(i = 0;i < 3;i++)
	{
		mx[i] = myabs(xb[i]-xb[(i+1)%3]);
	  if( mx[i] >  ad_errvalue) return 0;
		my[i] = myabs(yb[i]-yb[(i+1)%3]);
	  if(my[i] >  ad_errvalue) return 0;
	}
	
  if(mx[0]<mx[1]) {if(mx[2]<mx[0]) touch.ad_xvalue=(xb[0]+xb[2])/2; 
  else touch.ad_xvalue=(xb[0]+xb[1])/2; } 
  else if(mx[2]<mx[1]) touch.ad_xvalue=(xb[0]+xb[2])/2; 
  else touch.ad_xvalue=(xb[1]+xb[2])/2;
	
	if(my[0]<my[1]) {if(my[2]<my[0]) touch.ad_yvalue=(yb[0]+yb[2])/2; 
  else touch.ad_yvalue=(yb[0]+yb[1])/2; } 
  else if(my[2]<my[1]) touch.ad_yvalue=(yb[0]+yb[2])/2; 
  else touch.ad_yvalue=(yb[1]+yb[2])/2;
	
	return 1;
}

#define XD0 50.f
#define YD0 40.f

#define XD1 240.f
#define YD1 280.f

#define XD2 60.f
#define YD2 450.f

void adjust(void)
{
	uint8_t status = 0;
	uint16_t adxvalue[3];
	uint16_t adyvalue[3];
	float K;
	while(status < 3)
	{
    switch(status)
    {
      case 0:
        LCD_Clear(WHITE);
        LCD_Draw_Circle(XD0, YD0, 5);
        break;
      case 1:
        LCD_Clear(WHITE);
        LCD_Draw_Circle(XD1, YD1, 5);
        break;
      case 2:
        LCD_Clear(WHITE);
        LCD_Draw_Circle(XD2,YD2, 5);
        break;
    }
    while(!touch_scan());
    if(xy_adget())
    {
      adxvalue[status] = touch.ad_xvalue;
      adyvalue[status] = touch.ad_yvalue;
      status++;
    }
    else
    {
      LCD_ShowString(160, 10, 320, 480, 24, "AD get err!");
    }
    while(T_PEN()==0);
	}
	LCD_Clear(WHITE);
	
  K = (adxvalue[0] - adxvalue[2] )*(adyvalue[1] - adyvalue[2] ) - (adxvalue[1] - adxvalue[2] )*(adyvalue[0] - adyvalue[2] );
  touch.A =((XD0 - XD2 )*(adyvalue[1] - adyvalue[2] ) - (XD1 - XD2 )*(adyvalue[0] - adyvalue[2] )) / K ;
  touch.B =((adxvalue[0] - adxvalue[2] )*(XD1 - XD2 ) - (XD0 - XD2 )*(adxvalue[1] - adxvalue[2] )) / K ;
  touch.C =(adyvalue[0]*(adxvalue[2]*XD1 - adxvalue[1]*XD2)+adyvalue[1]*(adxvalue[0]*XD2 - adxvalue[2]*XD0)+adyvalue[2]*(adxvalue[1]*XD0 - adxvalue[0]*XD1))/K ;
  touch.D =((YD0 - YD2 )*(adyvalue[1] - adyvalue[2] ) - (YD1 - YD2 )*(adyvalue[0] - adyvalue[2] )) / K ;
  touch.E =((adxvalue[0] - adxvalue[2] )*(YD1 - YD2 ) - (YD0 - YD2 )*(adxvalue[1] - adxvalue[2] )) / K ;
  touch.F =(adyvalue[0]*(adxvalue[2]*YD1 - adxvalue[1]*YD2)+adyvalue[1]*(adxvalue[0]*YD2 - adxvalue[2]*YD0)+adyvalue[2]*(adxvalue[1]*YD0 - adxvalue[0]*YD1))/ K;
  
  //存储校准参数
  uint8_t wbuf[50] = {ADJUSTFLAG,0};
  memcpy(&wbuf[1], &touch.A, 4);
  memcpy(&wbuf[5], &touch.B, 4);
  memcpy(&wbuf[9], &touch.C, 4);
  memcpy(&wbuf[13], &touch.D, 4);
  memcpy(&wbuf[17], &touch.E, 4);
  memcpy(&wbuf[21], &touch.F, 4);
  sFLASH_EraseSector(T_PARAM_ADDR);
  sFLASH_WriteBuffer(wbuf,T_PARAM_ADDR,50);
}

void xyposget(void)
{
  if(!touch_scan())
  {
    touch.xpos = 0xffff;
    touch.ypos = 0xffff;
  }
  else
  {
    xy_adget();

    touch.xpos = touch.A*touch.ad_xvalue+touch.B*touch.ad_yvalue+touch.C;
    touch.ypos = touch.D*touch.ad_xvalue+touch.E*touch.ad_yvalue+touch.F;
  }
}
