/*�õݹ�˼��ɨ��ָ��Ŀ¼�е��ض��ļ���������������ļ���С����ɨ��õ��Ľ�������д洢�������С�*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "malloc.h"
#include "scanf_dir.h"


int songTotalCount = 0; //����������
struct list *head;      //����ͷ
/********************************************************************
 *  Function��     linklist_clear()
 *  Description:   ɾ������
 *  Calls:         ;
 *  Called By:
 *  Input��        head����ͷ ;
 *  Output��
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
 *  Function��     scan_dir()
 *  Description:   ɨ��Ŀ¼�ļ�����ŵ����ӱ���
 *  Calls:         ;
 *  Called By:
 *  Input��        dir��Ŀ¼��
 *                 depth:��ʼɨ����ȣ�����ʱ��Ϊ0��ֻ�Ƿ���ݹ�ʹ��;
 *  Output��       head����ͷ��ַ;
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
 *  Function��     list_print()
 *  Description:   ��������ϵ�����
 *  Calls:         ;
 *  Called By:
 *  Input��        none
 *  Output��       none;
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
 *  Function��     list_node_count()
 *  Description:   ��������Ͻڵ�����
 *  Calls:         ;
 *  Called By:
 *  Input��        none
 *  Output��       none;
 *  Return :       �ڵ�����
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




//���������ͷ��ڴ�
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
 *  Function��     get_one_node()
 *  Description:   ���ȡ�������ϵĵ��ļ���;
 *  Calls:         ;
 *  Called By:
 *  Input��        none
 *  Output��       none;
 *  Return :       ����ڵ�
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
        int t = ran % iCount;//����0��iCount-1֮���һ�������
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

