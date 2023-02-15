#include "key.h"
#include "ucos_ii.h"


void Key_init(void)
{
   GPIO_InitTypeDef GPIO_InitStruct;//�ṹ�����  GPIO 
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//��PAʱ��
   
   GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;//PA0   ����ģʽ   ��������
   GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
   GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
   GPIO_Init(GPIOA,&GPIO_InitStruct);
}


u8 Key_Scan(void)
{
//    u32 t=15000;      //����ʱ��
    static u8 flag=0;//���±�־λ
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) && flag==0)//��ȡ�͵͵�ƽ ��һ��
    {
        flag=1; 
//        while(t--);   //����
        OSTimeDly(20);
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) return 1; //�ڶ��� 
	   
    }
    else if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) ) flag=0;     //�ͷ�
    return 0;
}
