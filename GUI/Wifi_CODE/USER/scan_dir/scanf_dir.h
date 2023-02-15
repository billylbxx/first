
#ifndef __LIST__H__
#define __LIST__H__
#include "ff.h"

struct list {
    unsigned char path_file_name[_MAX_LFN + 1];
    struct list *next;
};
extern struct list *head;
FRESULT scan_dir(struct list **head, char *dir, int depth);
int list_print(struct list *);
unsigned int list_node_count(struct list * head);
int linklist_clear(struct list *head);
void scanf_dir_test(void);

#define LEN sizeof(struct list)
#endif


