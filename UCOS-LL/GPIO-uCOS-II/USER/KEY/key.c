#include "key.h"
#include "ucos_ii.h"


void Key_init(void)
{
   GPIO_InitTypeDef GPIO_InitStruct;//结构体变量  GPIO 
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//开PA时钟
   
   GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;//PA0   输入模式   浮空输入
   GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
   GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
   GPIO_Init(GPIOA,&GPIO_InitStruct);
}


u8 Key_Scan(void)
{
//    u32 t=15000;      //消抖时间
    static u8 flag=0;//按下标志位
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) && flag==0)//读取低低电平 第一次
    {
        flag=1; 
//        while(t--);   //消抖
        OSTimeDly(20);
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) return 1; //第二次 
	   
    }
    else if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) ) flag=0;     //释放
    return 0;
}
