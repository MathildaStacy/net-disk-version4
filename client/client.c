#include "analyOrder.h"
#include "exePanClient.h"
#include "loginAndRegister.h"
#include "train.h"
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
    
    printf("-------------------------------欢迎使用本大爷的网盘-------------------------------\n");
    //循环登录////////////////////////////////////////////////////////////////////////////////////////////////
    while(1){
        
        //每次循环都初始化指令结构，来接收用户标准输入
        order_t order;
        orderInit(&order);

        //判断业务类型，选择登录或者注册/////////////////////////////////////////////////////////////////////////
        printf("请选择业务类型：\n 键盘输入 0 : 登录 \n 键盘输入 1 ：注册 \n 键盘键入 2 ：退出\n");
        int flag = 0;
        scanf("%d",&flag);
        printf("%d \n",flag);
        if(flag == 0){

            printf("----------登录----------\n");
            printf("username：");
            fflush(stdout);
            scanf("%s", order.username);
            printf("userpassword：");
            fflush(stdout);
            scanf("%s", order.password);

        }
        if(flag == 1){

            printf("----------注册----------\n");
            printf("username：");
            fflush(stdout);
            scanf("%s", order.username);
            printf("userpassword：");
            fflush(stdout);
            scanf("%s", order.password);
        }

        if(flag == 2){

            printf("下次再见！！！\n");
            break;
        }
        
        
        //执行登录业务///////////////////////////////////////////////////////////////////////////////////////////
        order.cmd = LOGIN;
        //train_t train;
        //train.size =sizeof(order_t);
        //memcpy(train.buf,&order,train.size);
        send(sockfd,&order,sizeof(order_t),SIGEV_SIGNAL);//发送空信息表示这是一个登录操作
        //选择具体业务
        if(flag == 1){
            //注册
            order.num = 1; //注册标志位
            int ret = userRegister(&order,sockfd);
            if(ret == 0){
                //注册成功
                printf("您已注册成功，感谢选择本大爷的网盘！！！\n");
            }
            else{
                printf("注册失败，用户名已被使用！！！\n");
            }
        }
        if(flag == 0){
            //登录
            order.num = 0; //登录标志位
            int ret = userLogin(&order, sockfd);
            if(ret == 0){
                //登录成功 执行网盘业务/////////////////////////////////////////////////////////////////////////
                printf("用户：%s 登录成功，欢迎使用本大爷的网盘！！！\n",order.username);
                //获取token值
                getToken(sockfd, order.token);
                printf("1\n");
                //执行网盘业务
                exePanClient(sockfd, order.username, order.token);
                close(sockfd);
                break;

            }
            printf("登录失败, 用户名或者密码错误, 宝贝！请重新登录\n");
        }
    }
    //while()循环结束，说明客户端业务已经完成，准备下线/////////////////////////////////////////////////////////
    return 0;
}
