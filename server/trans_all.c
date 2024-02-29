#include "train.h"
#include "hsqdef.h"


int recv_train(void *buf, int netfd) {
    train_t train;
    ssize_t n;

    // 首先接收size部分
    n = recv(netfd, &train.size, sizeof(train.size), 0);
    if (n == -1 || n != sizeof(train.size)) {
        return -1;  // 接收失败
    }

    // 确保size合法
    if (train.size > sizeof(train.buf)) {
        return -1;  // size超出预期，失败
    }

    // 接收buf内容
    n = recv(netfd, train.buf, train.size, 0);
    if (n == -1 || n != train.size) {
        return -1;  // 接收失败
    }

    // 将内容复制到调用者的buffer中
    memcpy(buf, train.buf, train.size);
    return 0;  // 接收成功
}

int send_train(void *buf, long size, int netfd) {
    train_t train;
    ssize_t n;

    // 准备数据结构
    train.size = size;
    memcpy(train.buf, buf, size);

    // 发送size部分
    n = send(netfd, &train.size, sizeof(train.size), 0);
    if (n == -1 || n != sizeof(train.size)) {
        return -1;  // 发送失败
    }

    // 发送buf内容
    n = send(netfd, train.buf, size, 0);
    if (n == -1 || n != size) {
        return -1;  // 发送失败
    }

    return 0;  // 发送成功
}

