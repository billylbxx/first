#ifndef __QMsg_H
#define __QMsg_H
#include "stdbool.h"
#include "io_bit.h"
#include "stm32f4xx.h"
#define MAXSIZE 11

typedef struct {
    unsigned char elem[MAXSIZE];
    unsigned char front;        //下标可取,指向队头元素
    unsigned char rear;         //下标不可取,指向元素应放入的位置
}CQueue;
extern CQueue qhead;

typedef struct {
	bool (*init)(CQueue *pQueue);
    bool (*push)(CQueue *pQueue, unsigned char val);
    bool (*pop)(CQueue *pQueue, unsigned char *rtval);
    int  (*get_length)(CQueue *pQueue);
    void (*show)(CQueue *pQueue);
    bool (*is_empty)(CQueue *pQueue);
    bool (*is_full)(CQueue *pQueue); 
    bool (*get_front)(CQueue *pQueue, unsigned char *rtval);
}__CQueue;
extern __CQueue QMsg;

#endif
