#include "analyOrder.h"
#include "exePanClient.h"
#include "loginAndRegister.h"
int main(int argc, char *argv[])
{
    // 192.168.72.128:1234 是服务端的ip端口
    ARGS_CHECK(argc,3);
    //与服务端建立连接
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
    int ret = connect(sockfd,(struct sockaddr *)&serverAddr,sizeof(serverAddr));
    ERROR_CHECK(ret,-1,"connect");

    //执行登录业务
    //服务端先接收客户端发送来的信息（空），来判断是否是登录，注册，注销业务
    //用户登录后返回一个用户名和令牌 username and token;
    printf("--------NetDisk--------\n");
    char username[64] = {0}; 
    char token[512] = {0};
    order_t order;
    orderInit(&order);
    order.cmd =LOGIN;
    //
    send(sockfd,&order,sizeof(order_t),0);//发送空信息表示这是一个登录操作
    //注册
    userRegister(username,sockfd);
    //登陆
    userLogin(username, sockfd);
    //获取token值
    getToken(sockfd, token);
    printf("Login successfully, username = %s\n",username);

    //执行网盘业务
    exePanClient(sockfd, username, token);
    printf("----------------------client.c\n");
    close(sockfd);
    return 0;
}





















