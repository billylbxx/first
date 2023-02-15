#ifndef __W25QXX_H
#define __W25QXX_H
#include "io_bit.h" 
#define flash_cs PCout(7)
void w25q64_Init(void);
u16 W25Q64_readID(void);
void write_ENABLE(void);
u16 w2qQ64_write_status(u16 CMD);
void w25q64_clean_all(void);
u16 w25q64_BUSY(void);
void w25q64_write_page(u32 addr,const char *sp,u32 Length);
void W25Q64_Write_NoCheck(u8 *Buffer,u32 WriteAddress,u32 Length);
u8* W25QXX_read_date(u32 addr,int len);
#endif
