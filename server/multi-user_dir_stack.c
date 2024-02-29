#include "multi-user_dir_stack.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int dirStackInit(dirStackType **dirStk) {
    *dirStk = (dirStackType *)calloc(1, sizeof(dirStackType));
    if (*dirStk == NULL) {
        return 1;
    }

    (*dirStk)->stk = (stackType *)calloc(1, sizeof(stackType));
    if ((*dirStk)->stk == NULL) {
        free(*dirStk);  // 释放已分配的dirStackType指针
        return 1;
    }

    return 0;
}

int isEmpty(dirStackType *dirStk) {
    return dirStk->stk->stkSize == 0;
}

//不成功返回1，成功返回0，本函数会动态内存分配克隆一个ele字符串的副本
int stkPush(dirStackType *dirStk, int ele) {
    stackNodeT *new_node = (stackNodeT *)calloc(1, sizeof(stackNodeT));
    if(new_node == NULL) {
        return 1;
    }

    new_node->fileId = ele; // 使用strdup克隆一个字符串副本
   

    if(dirStk->stk->stkSize == 0) {
        dirStk->stk->head = new_node;
        dirStk->stk->tail = new_node;
    } else {
        new_node->next = dirStk->stk->head;
        dirStk->stk->head = new_node;
    }
    dirStk->stk->stkSize++;

    return 0;
}

int stkPop(dirStackType *dirStk, int* ele) {
    if(isEmpty(dirStk)) {
        return 1;
    }

    stackNodeT *node_to_pop = dirStk->stk->head;
    dirStk->stk->head = dirStk->stk->head->next;
    dirStk->stk->stkSize--;

    if(dirStk->stk->stkSize == 0) {
        dirStk->stk->tail = NULL;
    }

    *ele = node_to_pop->fileId; // Assign the string to the output parameter
    free(node_to_pop);

    return 0; 
}

int getHead(dirStackType *dirStk, int* ele) {
    if (isEmpty(dirStk)) {
        return 1;
    }
    *ele = dirStk->stk->head->fileId;
    return 0;
}

int getTail(dirStackType *dirStk, int * ele) {
    if (isEmpty(dirStk)) {
        return 1;
    }
    *ele = dirStk->stk->tail->fileId;
    return 0;
}

void freeStack(dirStackType *dirStk) {
    stackNodeT* current = dirStk->stk->head;
    while(current != NULL) {
        stackNodeT* next = current->next;
        
        free(current);
        current = next;
    }
    free(dirStk->stk);
    free(dirStk);
}


//通过用户名找到用户的当前目录栈
int findUserStackByUserName(MultiUserStack_t *multi_user_stack, const char *userName, dirStackType **pointer_to_dirstk_pointer)
{
    for(int i = 0; i < MAX_USERS_NUM; i++)
    {
        printf("102 %d\n", i);
        if(multi_user_stack->UsersStack[i] != NULL  && strcmp(multi_user_stack->UsersStack[i]->userName, userName) == 0)
        {
            *pointer_to_dirstk_pointer = multi_user_stack->UsersStack[i];
            return 0; //成功找到
        }
    }

    return -1; //未成功找到
}

//向用户目录栈中找到一个位置并插入
int insertUserStack(MultiUserStack_t *multi_user_stack, dirStackType *user_dir_stk)
{
    for(int i = 0; i < MAX_USERS_NUM; i++)
    {
        if(multi_user_stack->UsersStack[i] == NULL)
        {
            multi_user_stack->UsersStack[i] = user_dir_stk;
            return 0; //成功插入并退出
        }
    }

    return -1; 
}

//根据用户名删除用户的当前目录栈
int deleteUserStackByStackName(MultiUserStack_t *multi_user_stack, const char *userName)
{
    for(int i = 0; i  < MAX_USERS_NUM; i++)
    {
        if(multi_user_stack->UsersStack[i] != NULL && strcmp(multi_user_stack->UsersStack[i]->userName, userName) == 0)
        {
            freeStack(multi_user_stack->UsersStack[i]);
            multi_user_stack->UsersStack[i] = NULL;
            return 0; //成功找到并删除
        }
    }

    return -1; //未找到
}

int initMultiUserStack(MultiUserStack_t *multi_user_stack)
{
    for(int i = 0; i < MAX_USERS_NUM; i++)
    {
        multi_user_stack->UsersStack[i] = NULL;
    }

    return 0;
}


/*
int main()
{
    

    MultiUserStack_t multi_user_stk;
    initMultiUserStack(&multi_user_stk);

    for(int i = 0; i < 100; i++)
    {
        dirStackType* t;
        dirStackInit(&t);
        snprintf(t->userName, 30, "%d", i);
        insertUserStack(&multi_user_stk, t);
    }

    dirStackType *t1;
    findUserStackByUserName(&multi_user_stk, "5", &t1);
    stkPush(t1, 1);
    stkPush(t1, 2);

    dirStackType *t2;
    findUserStackByUserName(&multi_user_stk, "8", &t2);
    stkPush(t2, 3);
    stkPush(t2, 4);

    dirStackType *t3;
    findUserStackByUserName(&multi_user_stk, "6", &t3);
    stkPush(t3, 7);
    stkPush(t3, 8);

    printf("183\n");
    dirStackType *t4;
    int ret = findUserStackByUserName(&multi_user_stk, "101", &t4);
    if(ret == -1)
    {
        printf("187 not found!\n");
    }

    dirStackType *t5;
    findUserStackByUserName(&multi_user_stk, "6", &t5);
    int ele1;
    stkPop(t5, &ele1);
    printf("ele1: %d\n", ele1);
    stkPop(t5, &ele1);
    printf("ele1: %d\n", ele1);

    
}

*/
