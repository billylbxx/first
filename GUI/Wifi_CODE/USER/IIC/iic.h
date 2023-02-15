#ifndef _IIC_H_
#define _IIC_H_

#include "io_bit.h"              

#define ACK	 0
#define	NACK 1  

#define IIC_SCL       PBout(6)     //SCLʱ������
#define IIC_SDA    	  PBout(7)     //���SDA	 
#define IIC_SDAIN     PBin(7)      //����SDA 

void IIC_Start(void);
void IIC_Stop(void);
unsigned char IIC_Wait_ACK(void);
void IIC_WriteByte(unsigned char data);
unsigned char IIC_ReadByte(unsigned char ack);

#endif
