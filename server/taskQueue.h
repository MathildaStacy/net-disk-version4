#ifndef __TASKQUEUE__
#define __TASKQUEUE__
#include "analyOrder.h"
#include "multi-user_dir_stack.h"
typedef struct node_s {
    int netfd;
    order_t* porder;
    dirStackType* dirstack;
    struct node_s* pNext;
} node_t;
typedef struct taskQueue_s {
    node_t * pFront;
    node_t * pRear;
    int queueSize;
} taskQueue_t;
int taskQueueInit(taskQueue_t * pqueue);
int enQueue(taskQueue_t *pqueue, int netfd, order_t* porder,dirStackType* dirstack);
int deQueue(taskQueue_t *pqueue);
#endif
