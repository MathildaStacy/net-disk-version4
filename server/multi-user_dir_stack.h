#ifndef __MULTI_USER_DIR_STACK_H__
#define __MULTI_USER_DIR_STACK_H__

#define MAX_USERS_NUM 1024

typedef struct stackNodeS {
    int fileId;
    struct stackNodeS* next;
} stackNodeT;

typedef struct stackS {
    stackNodeT *head;
    stackNodeT *tail;
    int stkSize;
} stackType;

typedef struct dirStackS {
    stackType *stk;
    char userName[30];
} dirStackType;

typedef struct MultiUserStack_s {
    dirStackType *UsersStack[MAX_USERS_NUM];
} MultiUserStack_t;

//通过用户名找到用户的当前目录栈
int findUserStackByUserName(MultiUserStack_t *multi_user_stack, const char *userName, dirStackType **pointer_to_dirstk_pointer);

//向用户目录栈中找到一个位置并插入
int insertUserStack(MultiUserStack_t *multi_user_stack, dirStackType *user_dir_stk);

//根据用户名删除用户的当前目录栈
int deleteUserStackByStackName(MultiUserStack_t *multi_user_stack, const char *userName);

int initMultiUserStack(MultiUserStack_t *multi_user_stack);



// 初始化栈
int dirStackInit(dirStackType **dirStk);
// 检查栈是否为空
int isEmpty(dirStackType *dirStk);
// 入栈操作
int stkPush(dirStackType *dirStk, const int ele);
// 出栈操作
int stkPop(dirStackType *dirStk, int* ele);
// 获取栈顶元素
int getHead(dirStackType *dirStk, int* ele);
// 获取栈底元素
int getTail(dirStackType *dirStk, int* ele);
// 释放栈
void freeStack(dirStackType *dirStk);

#endif