#include "QMsg.h"
#include <stdio.h>
static bool InitCQueue(CQueue *pQueue);          			//初始化队列
static bool Push(CQueue *pQueue, unsigned char val);		//入队，从队尾(rear)入
static bool Pop(CQueue *pQueue, unsigned char *rtval);		//出队，从队首(front)出
static int GetCQueueLen(CQueue *pQueue);         			//获取队列长度
static void ShowQueue(CQueue *pQueue);          		 	//输出队列所有元素
static bool IsEmpty(CQueue *pQueue);             			//队列为空则返回true
static bool IsFull(CQueue *pQueue);             			//队列满则返回false
static bool GetFront(CQueue *pQueue, unsigned char *rtval);	//获取队首元素

//队列控制器对象
__CQueue QMsg = {
	.init = InitCQueue,
	.push = Push,
	.pop = Pop,
	.get_length = GetCQueueLen,
	.show = ShowQueue,
	.is_empty = IsEmpty,
	.is_full = IsFull,
	.get_front = GetFront,
};

//初始化队列
bool InitCQueue(CQueue *pQueue)             
{
    if(NULL == pQueue){
        return true;
	}
        
    pQueue->front = 0;
    pQueue->rear = 0;
    
    return false;
}

//入队，从队尾(rear)入
bool Push(CQueue *pQueue, unsigned char val)          
{
    //入队之前应该先判断队列是否已经满了
    if(IsFull(pQueue)){
        return true;
    }

    pQueue->elem[pQueue->rear] = val;               //将元素放入队列，这里也说明front是可取的
    pQueue->rear = (pQueue->rear+ 1) % MAXSIZE;    	//更新队尾位置
    
    return false;
}

//出队，从队首(front)出
bool Pop(CQueue *pQueue, unsigned char *rtval)        
{
    //出队之前应该先判断队列是否是空的
    if(IsEmpty(pQueue)){
        return true;
    }

    *rtval = pQueue->elem[pQueue->front];           //取出队头元素
    pQueue->front = (pQueue->front+ 1) % MAXSIZE;  //更新队头位置

    return false;
}

//获取队列长度
int GetCQueueLen(CQueue *pQueue)            
{
    return ((pQueue->rear - pQueue->front + MAXSIZE) % MAXSIZE);
}

//输出队列所有元素
void ShowQueue(CQueue *pQueue)              
{

    int front = pQueue->front;
    int rear = pQueue->rear;

    while (front != rear){
        printf("%2d", pQueue->elem[front++]);
    }

    printf("\r\n");
}

//队列为空则返回true
bool IsEmpty(CQueue *pQueue)                
{
    return pQueue->rear == pQueue->front;
}

//队列满则返回false
bool IsFull(CQueue *pQueue)                 
{
    return (pQueue->rear + 1)%MAXSIZE == pQueue->front;
}

//获取队首元素
bool GetFront(CQueue *pQueue, unsigned char *rtval)   
{
    if (!IsEmpty(pQueue)){
        *rtval = pQueue->elem[pQueue->front];   //将队头元素通过*rtval返回
        return true;
    }

    return false;
}


