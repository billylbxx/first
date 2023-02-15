#ifndef _IIC_H_
#define _IIC_H_

#include "io_bit.h"              

#define WM_IIC_SCL       PBout(8)     //SCLʱ������
#define WM_IIC_SDA    	 PBout(9)     //���SDA	 
#define WM_IIC_SDAIN     PBin(9)      //����SDA 
#define WM_IIC_SDA_IN()  {GPIOB->MODER &= ~(3<<2*9);} //SDA����Ϊ����
#define WM_IIC_SDA_OUT() {GPIOB->MODER &= ~(3<<2*9);  GPIOB->MODER |= (1<<2*9);} //SDA����Ϊͨ�����

/*IIC����*/
void WM_IIC_Init(void);
void WM_IIC_Start(void);
void WM_IIC_Stop(void);
unsigned char WM_IIC_Wait_ACK(void);
void WM_IIC_WriteByte(unsigned char data);
unsigned char WM_IIC_ReadByte(unsigned char ack);

#endif
