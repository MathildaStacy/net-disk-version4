#include "exePanClient.h"
#include "hsqdef.h"
#include "analyOrder.h"
#include "puts.h"
#include "fbr_gets_and_puts.h"
#include "others.h"
#include "clientWorker.h"
#include "trans_all.h"
int exePanClient(int sockfd, char*usrname, char* token){

    char bufPrintf[1024] = {0};//保存当前命令行提示符信息
    sprintf(bufPrintf, "%s%s",usrname,":~$ ");
    
    //设置子线程属性为自动释放资源
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    while(1){

        //打印命令行提示信息///////////////////////////////////////////////////////////////
        displayCmdLine(bufPrintf);

        //接收用户标准输入
        ///////////////////////////////////////////////////////////////////////////////////
        char buf[1024] = {0};
        memset(buf, 0, sizeof(buf));
        ssize_t retStdin = read(STDIN_FILENO, buf, sizeof(buf));
        ERROR_CHECK(retStdin,-1,"read");
        //拼接order结构体
        order_t order;
        orderInit(&order);//初始化指令结构体
        memcpy(order.username,usrname,64);
        memcpy(order.token,token,512);//添加token信息
        int ret = analyOrder(buf, &order);//添加命令、命令参数、参数数量信息
        if(ret == -1){
            //指令输入错误，进入下一次循环，继续等待用户输入
            continue;
        }

        //调试语句////////////////////////////////////////////////////////////////////////
        /*
        printf("cmd = %d\n",order.cmd);//打印指令枚举
        for(int i = 0; i < order.num; ++i){
            //循环打印当前参数和参数大小
            printf("parameter = %s len = %ld\n",order.parameters[i],strlen(order.parameters[i]));
        }
        */
        //

        //判断长短命令选择执行分支
        //////////////////////////////////////////////////////////////////////////////////
        //
        //短命令分支
        /////////////////////////////////////////////////////////////////////////////////
        if(chooseCmd(&order)==1){
            //短命令
            //直接向客户端发送信息
            ssize_t retSend = send(sockfd,&order,sizeof(order),0);
            //printf("retsend = %ld, order size = %ld\n",retSend, sizeof(order));
            ERROR_CHECK(retSend, -1,"send order");

            switch(order.cmd){

            case LS:
                {
                    char lsMsg[4096] = {0};
                    int ret = recv(sockfd,lsMsg,4096,0);
                    ERROR_CHECK(ret, -1, "recv lsMsg");
                    printf("%s\n",lsMsg);
                    break;
                }
            case CD:
                {
                    //接收服务端发回来的返回值，判断之后的操作
                    int ret = 0;
                    recv(sockfd, &ret, sizeof(int), 0);
                    if(ret == 0){
                        //改变当前命令提示符显示
                        changeCmdline(bufPrintf, &order);
                    }
                    break;
                }

            case RM:case RMDIR:
                {
                    int flag = 0;
                    int retRecv = recv(sockfd, &flag, sizeof(int),0);
                    ERROR_CHECK(retRecv, -1,"recv rm");
                    //判断是否删除成功
                    if(flag == -1){//不成功就打印信息,成功不做任何操作
                        printf("删除失败\n");
                    }
                    break;
                }
            case PWD:
                {
                    char vitualPath[1024] = {0};
                    recv(sockfd,vitualPath,1024,MSG_WAITALL);
                    printf("path = %s\n",vitualPath);
                    break;
                }

            case MKDIR:

                {
                    int flag = 0;
                    int retRecv = recv(sockfd, &flag, sizeof(int),0);
                    ERROR_CHECK(retRecv, -1,"recv mkdir");
                    //判断是否删除成功
                    if(flag == -1){//不成功就打印信息,成功不做任何操作
                        printf("创建目录失败\n");
                        
                    }

                    break;
                }

            case QUIT:
                break;

            default:
                break;

            }
        }

        //长命令分支
        ///////////////////////////////////////////////////////////////////////////////////
        if(chooseCmd(&order)==0){
            //长命令

            //1 堆上分配空间来存储指令结构体
            order_t* porder = (order_t* )malloc(sizeof(order_t));
            memset(porder,0,sizeof(order_t));
            memcpy(porder,&order,sizeof(order_t));

            //2 创建子线程
            pthread_t pid;
            pthread_create(&pid, NULL, threadworker,(void*)porder);




            printf("--------------------------------exePanClien.c\n");

        }
    }//while 循环结束, 网盘业务结束，退出
    pthread_attr_destroy(&attr);
    return 0;
}
    
