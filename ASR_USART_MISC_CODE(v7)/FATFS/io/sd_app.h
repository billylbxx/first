#ifndef __SD_APP_H__
#define __SD_APP_H__

#include "io_bit.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

void SD_Init(void);
u8 mf_getfree(u8 *drv,u32 *total,u32 *free);
u16 f_getflietnum(char *path,u8 ftpye);


#endif

