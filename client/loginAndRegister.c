#include "loginAndRegister.h"
#include "train.h"
#include "trans_all.h"
#include "fbr_gets_and_puts.h"
#define BUF_SIZE 4096
#define PORT 9190

//int recvn(int sockfd, void* buf, long size)
//{
//    char* p = (char*)buf;
//    int curSize = 0;
//    while (curSize < size) {
//    int getrecv = recv(sockfd, p + curSize, size - curSize, MSG_WAITALL);
//    if (getrecv == 0) {
//    return curSize;
//    }
//    curSize += getrecv;
//    }
//
//    return curSize;;
//}
//
//int sendn(int sockfd, const void* buf, long size)
//{
//    const char* p = (const char*)buf;
//    int curSize = 0;
//    while (curSize < size) {
//    int sent = send(sockfd, p + curSize, size - curSize, 0);
//    if (sent < 0) {
//        perror("send error");
//        return -1;
//    }
//    curSize += sent;
//    }
//
//    return 0;
//}
//用户注册
int userRegister(order_t *order, int netfd) {
    char buf[4096] = {0};
    // bzero(&fileData, sizeof(fileData));
    char sendBuf[1004] = {0};
    int ret;

    strcpy(buf, "Register");
    // printf("fileData.dataBuf = %s\n", fileData.dataBuf);
    // SEND_FILEDATA;
    send_train(buf, 9,netfd);

    // 发送用户名
    bzero(buf, sizeof(buf));
    strcpy(buf, order->username);
    send_train(buf, 64,netfd);
    // 得到用户名是否存在的情况
    bzero(buf, sizeof(buf));
    recv_train(buf,netfd);
    if (strcmp(buf, "errorrr") == 0) {
        printf("signIn error: username exist!\n");
        return -1;
    }

    // 发送密码
    // bzero(&fileData, sizeof(fileData));
    // strcpy(fileData.dataBuf, order->password);
    bzero(buf, sizeof(buf));
    strcpy(buf, order->password);
send_train(buf, 32,netfd);
    // GET_FILEDATA;
    bzero(buf, sizeof(buf));
    recv_train(buf ,netfd);
    if (strcmp(buf, "errorrr") == 0) {
        printf("Register failed\n");
        return -1;
    } else {
        printf("Register success!\n");
        printf("Please enter a return!\n");
        getchar();
    }
    return 0;
}

//登陆
int userLogin(order_t *order, int netfd) {
    char buf[4096] = {0};

    strcpy(buf, "Loginnnn");
    // SEND_FILEDATA;
send_train(buf, 9,netfd);//1s
    bzero(buf, sizeof(buf));
    strcpy(buf, order->username);
    // SEND_FILEDATA;
send_train(buf, 64,netfd);//2s
    // 得到用户名是否存在的情况
    // GET_FILEDATA;
    bzero(buf, sizeof(buf));
    recv_train(buf,netfd);//3r
    if (strcmp(buf, "error") == 0) {
        printf("signIn error:username don't exist!\n");
        return -1;
    }

    // strcpy(fileData.dataBuf, order->password);
    // SEND_FILEDATA;
    bzero(buf, sizeof(buf));
    strcpy(buf, order->password);
    send_train(buf, 32,netfd);//4s   duibuq 发密码
    // 得到salt
    char salt[21] = {0};
    // GET_FILEDATA;
    bzero(buf, sizeof(buf));
    recv_train(buf,netfd);//5r 收yz

    strncpy(salt, buf, 20);
    salt[20]= '\0';

    char shadow[255] = {0};
    shadow[0] = '\0';
    strcpy(shadow, crypt(order->password, salt));
    // 得到shadow
    char shadow_server[255];
    // GET_FILEDATA;
    bzero(buf, sizeof(buf));
    recv_train(buf,netfd);//收秘闻
    strncpy(shadow_server, buf, 255);

    if (strcmp(shadow, shadow_server) == 0) {
        // strcpy(fileData.dataBuf, "success");
        // SEND_FILEDATA
        bzero(buf, sizeof(buf));
        strcpy(buf, "success");
        send_train(buf, 8,netfd);
        return 0;

    } else {
        printf("Password error\n");
        // bzero(&fileData, sizeof(fileData));
        // strcpy(fileData.dataBuf, "error");
        // SEND_FILEDATA;
        bzero(buf, sizeof(buf));
        strcpy(buf, "errorrr");
        send_train(buf, 8,netfd);
        return -1;
    }
}


////用户注册
//int userRegister(order_t *order, int netfd) {
//    File_Data_t fileData;
//    bzero(&fileData, sizeof(fileData));
//    char sendBuf[1004] = {0};
//    int ret;
//
//    strcpy(fileData.dataBuf, "Register");
//    printf("fileData.dataBuf = %s\n", fileData.dataBuf);
//    SEND_FILEDATA;
//    bzero(&fileData, sizeof(fileData));
//
//    // 发送用户名
//    strcpy(fileData.dataBuf, order->username);
//    printf("fileData.dataBuf = %s\n", fileData.dataBuf);
//    SEND_FILEDATA;
//
//    // 得到用户名是否存在的情况
//    GET_FILEDATA;
//    if (strcmp(fileData.dataBuf, "error") == 0) {
//        printf("signIn error: username exist!\n");
//        return -1;
//    }
//
//    // 发送密码
//    bzero(&fileData, sizeof(fileData));
//    strcpy(fileData.dataBuf, order->password);
//    SEND_FILEDATA;
//
//    GET_FILEDATA;
//    if (strcmp(fileData.dataBuf, "error") == 0) {
//        printf("Register failed\n");
//        return -1;
//    } else {
//        printf("Register success!\n");
//        printf("Please enter a return!\n");
//        getchar();
//    }
//    return 0;
//}
//
////登陆
//int userLogin(order_t *order, int netfd) {
//    File_Data_t fileData;
//    bzero(&fileData, sizeof(fileData));
//    char sendBuf[1004] = {0};
//    char buf[1000] = {0};
//    int ret;
//
//    strcpy(fileData.dataBuf, "Login");
//    SEND_FILEDATA;
//    bzero(&fileData, sizeof(fileData));
//
//    strcpy(fileData.dataBuf, order->username);
//    SEND_FILEDATA;
//
//    // 得到用户名是否存在的情况
//    GET_FILEDATA;
//    if (strcmp(fileData.dataBuf, "error") == 0) {
//        printf("signIn error:username don't exist!\n");
//        return -1;
//    }
//
//    bzero(&fileData, sizeof(fileData));
//    strcpy(fileData.dataBuf, order->password);
//    SEND_FILEDATA;
//    
//    // 得到salt
//    char salt[21] = {0};
//    GET_FILEDATA;
//    strncpy(salt, fileData.dataBuf, 20);
//    salt[20]= '\0';
//
//    char shadow[255] = {0};
//    shadow[0] = '\0';
//    strcpy(shadow, crypt(order->password, salt));
//    // 得到shadow
//    char shadow_server[255];
//    GET_FILEDATA;
//    strncpy(shadow_server, fileData.dataBuf, 255);
//
//    if (strcmp(shadow, shadow_server) == 0) {
//        bzero(&fileData, sizeof(fileData));
//        strcpy(fileData.dataBuf, "success");
//        SEND_FILEDATA;
//        return 0;
//
//    } else {
//        printf("Password error\n");
//        bzero(&fileData, sizeof(fileData));
//        strcpy(fileData.dataBuf, "error");
//        SEND_FILEDATA;
//        return -1;
//    }
//}


//获取token
int getToken(int sockfd, char *token){
    char buf[512] = {0};
    int ret = recv(sockfd, buf, 512, MSG_WAITALL);
    if(ret == -1){
        perror("recv");
        return -1;
    }
    strcpy(token, buf);
    return 0;
}
// int main(void) {
//    struct sockaddr_in serv_addr;
//    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
//    if(sockfd == -1){
//        perror("socket");
//        return -1;
//    }

//    memset(&serv_addr, 0 , sizeof(serv_addr));
//    serv_addr.sin_family = AF_INET;
//    serv_addr.sin_addr.s_addr = inet_addr("192.168.10.130");
//    serv_addr.sin_port = htons(PORT);

//    int ret = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
//    if(ret == -1){
//        perror("connect");
//        return -1;
//    }
//    order_t order;
//    bzero(&order, sizeof(order_t));
//    strcpy(order.username, "test0");
//    strcpy(order.password, "123456");
//    printf("------------NetDisk------------\n");
//     userRegister(&order, sockfd);
//    //userLogin(&order,sockfd);
//     int flag;
    
//     // while(1){
//     //     printf("op:");
//     //     scanf("%d", flag);
//     //     switch(flag){
//     //         case 1:
//     //             userRegister(&order, sockfd);
//     //             break;
//     //         case 2:
//     //             userLogin(&order,sockfd);
//     //             break;
//     //     }
//     // }
//    printf("Login success, username = %s\n", order.username);
//    sleep(10);
//    close(sockfd);
//    return 0;
// }
