#ifndef __RUSSIAN_ROULETTE_H__
#define __RUSSIAN_ROULETTE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


#define TIMEOUT 6  // 超时阈值
#define WHEEL_SIZE TIMEOUT  // 时间轮大小

// 新定义的 ElementID 结构体
typedef struct {
    int id; // 元素的唯一标识符
} ElementID;

// 集合中的元素，现在包含 ElementID
typedef struct SetNode {
    ElementID element;  // 修改为 ElementID 类型
    time_t timestamp;  // 元素的访问时间
    struct SetNode *next;  // 指向下一个元素的指针
} SetNode;

// 循环队列（时间轮盘）的元素
typedef struct WheelNode {
    SetNode *setHead;  // 指向集合头结点的指针
} WheelNode;

// 定位映射表的元素，使用简单数组实现哈希表
typedef struct {
    int wheelPosition;  // 元素在时间轮中的位置
    SetNode *nodePtr;  // 指向集合中元素的指针
} HashMapEntry;

WheelNode wheel[WHEEL_SIZE];  // 时间轮盘
HashMapEntry hashMap[1024];  // 映射表，应根据实际情况调整大小

int currentTimeIndex = 0;  // 当前时间轮盘指针的位置

int updateTimeWheel(ElementID *kickout, int length);

#endif