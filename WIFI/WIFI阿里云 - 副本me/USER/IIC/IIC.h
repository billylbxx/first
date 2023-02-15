#ifndef __IIC_H
#define __IIC_H
#include "io_bit.h" 

#define IIC_SDA PBout(7)
#define IIC_SCL PBout(6)
#define IIC_SDAIN PBin(7)

void IIC_Start(void);
unsigned char IIC_Wait_ACK(void);
void IIC_Stop(void);
void IIC_ACK_NACK(unsigned char ack);
void IIC_write_byte(u8 CMD) ;
u8 IIC_read_byte(void);
#endif
