#include "key.h"
#include "ucos_ii.h"


void Key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;//�ṹ�����  GPIO 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);//��PAʱ��
   
    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_DOWN;
    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;    //PA0   KEY1
    GPIO_Init(GPIOA,&GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_13;    //PC13   KEY2
    GPIO_Init(GPIOC,&GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1;    //PB1   KEY3
    GPIO_Init(GPIOB,&GPIO_InitStruct);
}


//u8 Key_Scan(void)
//{
////    u32 t=15000;      //����ʱ��
//    static u8 flag=0;//���±�־λ
//    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) && flag==0)//��ȡ�͵͵�ƽ ��һ��
//    {
//        flag=1; 
////        while(t--);   //����
//        OSTimeDly(20);
//        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) return 1; //�ڶ��� 
//	   
//    }
//    else if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) ) flag=0;     //�ͷ�
//    return 0;
//}
