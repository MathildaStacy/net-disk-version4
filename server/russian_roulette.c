#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "russian_roulette.h"


extern WheelNode wheel[WHEEL_SIZE];  // 时间轮盘
extern HashMapEntry hashMap[1024];  // 映射表，应根据实际情况调整大小

extern int currentTimeIndex;  // 当前时间轮盘指针的位置


// 哈希函数
unsigned int hashFunction(int id) {
    return id % 1024;
}

void initWheel();
void initHashMap();
void insertElement(ElementID eid);
void removeElement(int id);
void updateElement(ElementID eid);
int updateTimeWheel(ElementID *kickout, int length);

// 初始化时间轮
void initWheel() {
    for (int i = 0; i < WHEEL_SIZE; i++) {
        wheel[i].setHead = NULL;
    }
}

// 初始化映射表
void initHashMap() {
    for (int i = 0; i < 1024; i++) {
        hashMap[i].wheelPosition = -1;
        hashMap[i].nodePtr = NULL;
    }
}

// 插入新元素 new version 带有自动更新元素位置
void insertElement(ElementID eid) {
    unsigned int hashIndex = hashFunction(eid.id);
    SetNode *existingNode = hashMap[hashIndex].nodePtr;

    // 如果这个ID已经存在于时间轮中
    if (existingNode != NULL && existingNode->element.id == eid.id) {
        // 更新已经存在的元素不应该简单地创建一个新元素
        updateElement(eid); // 确保除了调用前的检查外，updateElement不会再调用insertElement
        return;
    }

    // 如果元素不存在，在轮盘上为其找到一个新位置
    SetNode *newNode = (SetNode*)malloc(sizeof(SetNode));
    newNode->element = eid;
    newNode->timestamp = time(NULL);
    int newPosition = (currentTimeIndex + TIMEOUT) % WHEEL_SIZE;  // 计算新位置

    // 把新节点加入时间轮相应位置的集合
    newNode->next = wheel[newPosition].setHead;
    wheel[newPosition].setHead = newNode;

    // 更新哈希映射表
    hashMap[hashIndex].wheelPosition = newPosition;
    hashMap[hashIndex].nodePtr = newNode;
}

// 移除元素
void removeElement(int id) {
    unsigned int hashIndex = hashFunction(id);
    int position = hashMap[hashIndex].wheelPosition;
    if (position >= 0) {
        SetNode **curr = &(wheel[position].setHead);
        while (*curr) {
            SetNode *entry = *curr;
            if (entry->element.id == id) {
                *curr = entry->next;
                free(entry);
                hashMap[hashIndex].wheelPosition = -1;
                hashMap[hashIndex].nodePtr = NULL;
                return;
            }
            curr = &((*curr)->next);
        }
    }
}

// 更新元素位置
void updateElement(ElementID eid) {
    removeElement(eid.id); 
    insertElement(eid);
}

// 动态更新时间轮并记录被踢出的元素
// 返回被踢出元素的数量
int updateTimeWheel(ElementID *kickout, int length) {
    int count = 0; // 被踢出元素的计数器
    currentTimeIndex = (currentTimeIndex + 1) % WHEEL_SIZE;

    // 释放当前时间段的所有元素
    SetNode *curr = wheel[currentTimeIndex].setHead;
    while (curr && count < length) {
        SetNode *temp = curr;
        curr = curr->next;

        // 记录被踢出的元素ID
        kickout[count] = temp->element; // 直接复制结构体
        count++;

        // 释放元素并更新哈希映射表
        unsigned int hashIndex = hashFunction(temp->element.id);
        hashMap[hashIndex].wheelPosition = -1;
        hashMap[hashIndex].nodePtr = NULL;

        free(temp);
    }
    wheel[currentTimeIndex].setHead = NULL;

    return count; // 返回被踢出元素的数量
}
void printWheel() {
    printf("Current wheel state:\n");
    for (int i = 0; i < WHEEL_SIZE; i++) {
        printf("[%d] -> ", i);
        SetNode *current = wheel[i].setHead;
        while (current != NULL) {
            printf("%d ", current->element.id);
            current = current->next;
        }
        printf("\n");
    }
    printf("---------\n");
}


int printKickout(ElementID *arr, int n)
{
    printf("kickout arr: ");
    for(int i = 0; i < n; i++)
    {
        printf("%d ", arr[i].id);
    }
    printf("\n");

    return 0;
}

/*
int main() {
    // 初始化时间轮和映射表
    initWheel();
    initHashMap();
    ElementID kickout_arr[1024] = { 0 };

    // 插入几个测试元素
    ElementID ele = {1};
    insertElement(ele);
    sleep(1);
    int nums = updateTimeWheel(kickout_arr, 1024);
    printKickout(kickout_arr, nums);
    printWheel();
    printf("After %d second(s):\n", 1);

    ele.id = 2;
    insertElement(ele);
    sleep(1);
    nums = updateTimeWheel(kickout_arr, 1024);
    printKickout(kickout_arr, nums);
    printWheel();
    printf("After %d second(s):\n", 2);

    ele.id = 3;
    insertElement(ele);
    sleep(1);
    nums = updateTimeWheel(kickout_arr, 1024);
    printKickout(kickout_arr, nums);
    printWheel();
    printf("After %d second(s):\n", 3);

    ele.id = 1;
    insertElement(ele);
    sleep(1);
    nums = updateTimeWheel(kickout_arr, 1024);
    printKickout(kickout_arr, nums);
    printWheel();
    printf("After %d second(s):\n", 4);

    ele.id = 4;

    int count = 5;
    while(1)
    {
        
        insertElement(ele);
        ele.id++;
        insertElement(ele);
        ele.id++;
        sleep(1);
        nums = updateTimeWheel(kickout_arr, 1024);
        printKickout(kickout_arr, nums);
        printWheel();
        printf("After %d second(s):\n", count);
        count++;
    }

    return 0;
}
*/