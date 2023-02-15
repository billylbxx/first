/*用递归思想扫描指定目录中的特定文件，并计数求出总文件大小。对扫描得到的结果，进行存储到链表中。*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "malloc.h"
#include "scanf_dir.h"


int songTotalCount = 0; //计量歌曲数
struct list *head;      //链表头
/********************************************************************
 *  Function：     linklist_clear()
 *  Description:   删除链表
 *  Calls:         ;
 *  Called By:
 *  Input：        head链表头 ;
 *  Output：
 *  Return :       0
 *********************************************************************/
int linklist_clear(struct list *head)
{
    struct list * p = head;
    if(head) {
        while (head->next) {
            p = head->next;
            head->next = p->next;
            MemIn_Free(p);
            p = NULL;
        }
    }
    return 0;
}

/********************************************************************
 *  Function：     scan_dir()
 *  Description:   扫描目录文件，存放到链接表上
 *  Calls:         ;
 *  Called By:
 *  Input：        dir：目录；
 *                 depth:起始扫描深度，调用时候为0，只是方便递归使用;
 *  Output：       head链表头地址;
 *  Return :       0
 *********************************************************************/
FRESULT scan_dir(struct list **head, char *dir, int depth)
{
	struct list *p1, *p2;
	FRESULT res;
	FILINFO fno;
	DIR dp;
	int i;
	char *fn; /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
	static char lfn[_MAX_LFN + 1];
	fno.lfname = lfn;
	fno.lfsize = sizeof lfn;
#endif
	res = f_opendir(&dp, dir); /* Open the directory */
	if (res == FR_OK) {
		i = strlen(dir);
		for (;;) {
			res = f_readdir(&dp, &fno); /* Read a directory item */
			if (res != FR_OK || fno.fname[0] == 0) break; /* Break on error or end of dir */
			if (fno.fname[0] == '.') continue; /* Ignore dot entry */
	#if _USE_LFN
		fn = *fno.lfname ? fno.lfname : fno.fname;
	#else
		fn = fno.fname;
	#endif
		if (fno.fattrib & AM_DIR) { /* It is a directory */
			sprintf(&dir[i], "/%s", fn);
			res = scan_dir(head,dir,depth + 4);
			if (res != FR_OK) break;
			dir[i] = 0;
		}else { /* It is a file. */
			p1 = MemIn_Malloc(LEN);
			if(p1 == NULL) {
					printf("malloc err\r\n");
					return FR_INT_ERR;
			}			
			sprintf((char * )p1->path_file_name,"%s/%s",dir,fn);
			p1->next = NULL;
			songTotalCount++;
			if(songTotalCount == 1) {
					*head = p2 = p1;
					p2->next = NULL;
			}else{
					p2->next  = p1;
			}
			p1->next  = NULL;
			p2 = p1;
		}
	}
 }
	return res;
}

/********************************************************************
 *  Function：     list_print()
 *  Description:   输出链表上的内容
 *  Calls:         ;
 *  Called By:
 *  Input：        none
 *  Output：       none;
 *  Return :       0
 *********************************************************************/
int list_print(struct list * head)
{
    struct list *temp;
    temp = head;

   // printf("......here1");
    if(head != NULL) {
        do {
            printf("%s\r\n", temp->path_file_name);
            temp = temp->next;
        } while(temp != NULL);
    }
    return 0;
}


/********************************************************************
 *  Function：     list_node_count()
 *  Description:   输出链表上节点数量
 *  Calls:         ;
 *  Called By:
 *  Input：        none
 *  Output：       none;
 *  Return :       节点数量
 *********************************************************************/
unsigned int list_node_count(struct list * head)
{
    struct list *temp;
    unsigned int c = 0;
    temp = head;

    if(head != NULL) {
        do {
            c++;
            temp = temp->next;
        } while(temp != NULL);
    }

    return c;
}




//销毁链表，释放内存
int list_destroy(struct list * head)
{
    struct list *temp;
    temp = head;

    //printf("......here1");
    if(head != NULL) {
        do {
            temp = temp->next;
        } while(temp != NULL);
    }
    return 0;
}

/********************************************************************
 *  Function：     get_one_node()
 *  Description:   随机取出链表上的的文件名;
 *  Calls:         ;
 *  Called By:
 *  Input：        none
 *  Output：       none;
 *  Return :       随机节点
 *********************************************************************/
struct list* get_one_node(struct list* head)
{
    if(head == NULL) {
        return NULL;
    }
    int iCount = 1;
    struct list* tmp = head->next;
    struct list* pRet = tmp;
    while(tmp) {
        int ran;
        srand((unsigned)time(NULL));
        ran = rand();
        int t = ran % iCount;//生成0到iCount-1之间的一个随机数
        if(t == 0) {
            pRet = tmp;
        }
        iCount++;
        tmp = tmp->next;
    }
    return pRet;
}


void scanf_dir_test(void)
{
    
    printf("Scanfing [0:/MUSIC]...\r\n");
    scan_dir(&head, "0:/MUSIC", 0);
    printf("file list is:\r\n");
    list_print(head);

    printf("Scanf end\r\n");
    printf("songTotalCount:%d\r\n", songTotalCount);
    
}

