#include "worker.h"
#include "taskQueue.h"
#include "threadPool.h"
#include "analyOrder.h"
#include "chooseCmd.h"
#include "hsqdef.h"
#include "multi-user_dir_stack.h"
#include "cd.h"
#include "pwd.h"
#include "puts.h"
#include "fbr_gets_and_puts.h"
#include "rmMakeDir.h"
#include "loginAndRegister.h"
#include "logger.h"
#include "russian_roulette.h"
int exitPipe[2];
void handler(int signum){
    printf("signum = %d\n", signum);
    write(exitPipe[1],"1",1);
}
int main(int argc, char *argv[])
{
    g_log_level = 0;
    initWheel();
    initHashMap();

    // ./server 192.168.72.128 1234 4
    ARGS_CHECK(argc,4);
    LOG_DEBUG("here1");

    pipe(exitPipe);
    LOG_DEBUG("here");
    if(fork()!= 0){
        close(exitPipe[0]);
        signal(SIGUSR1,handler);
        wait(NULL);
        printf("Parent is going to exit!\n");
        exit(0);
    }
    LOG_DEBUG("here");
    //
    threadPool_t threadPool;
    threadPoolInit(&threadPool, atoi(argv[3]));

    LOG_DEBUG("here");
    //建立数据库连接//////////////////////////////////////////////////////////////////////////////
    //连接数据库，每个线程建立自己与数据库的连接,线程不安全,上锁
    pthread_mutex_lock(&threadPool.mutexMysql);
    LOG_DEBUG("here");
    MYSQL* conn = mysql_init(NULL);
    LOG_DEBUG("here");
    char *host = "localhost";
    char *user = "root";
    char *password = "123456";
    char *database = "netdisk";
    LOG_DEBUG("here");
    MYSQL *ret = mysql_real_connect(conn,host,user,password,database,0,NULL,0);
    LOG_DEBUG("here");
    pthread_mutex_unlock(&threadPool.mutexMysql);
    LOG_DEBUG("here");
    if(ret == NULL){
        //如果数据库没有连接成功，就断开与客户端的连接
        printf("Error: %s\n", mysql_error(conn));
        return 0;
    }
    printf("数据库连接成功\n");


    LOG_DEBUG("here");
    //
    close(exitPipe[1]);
    makeWorker(&threadPool);

    LOG_DEBUG("here");
    //
    int sockfd;
    tcpInit(argv[1],argv[2],&sockfd);

    LOG_DEBUG("here");
    //
    int epfd = epoll_create(1);
    LOG_DEBUG("here");
    epollAdd(epfd,sockfd);
    LOG_DEBUG("here");
    //epollAdd(epfd,exitPipe[0]);

    //短命令移植到main中执行，长命令分配给子线程做，也就是fork之后的子进程
    //////////////////////////////////////////////////////////////////////////////////////////////////

    LOG_DEBUG("here");
    //初始化多用户目录栈
    MultiUserStack_t multiUserStack;
    initMultiUserStack(&multiUserStack);

    LOG_DEBUG("here");
    while(1){

        ElementID kickout_arr[1024] = {0};
        LOG_DEBUG("here");
        struct epoll_event readySet[1024];
        int readyNum = epoll_wait(epfd,readySet,1024,1000);
        LOG_DEBUG("here");
        for(int i = 0; i < readyNum; ++i){

            //1 每当客户端连接过来时，分配一个netfd,并加入监听集合///////////////////////////////
            LOG_DEBUG("here");
            if(readySet[i].data.fd == sockfd){

                LOG_DEBUG("here");
                int netfd = accept(sockfd,NULL,NULL);
                //加入时间轮
                ElementID ele = {netfd};
                insertElement(ele);
                LOG_DEBUG("here");
                epollAdd(epfd,netfd);
            }

            //2 退出逻辑，监听到来自父进程的信号，执行优雅退出//////////////////////////////////////////
            //else if(readySet[i].data.fd == exitPipe[0]){
            //    // 线程池知道信号来了
            //    printf("threadPool is going to exit!\n");
            //    //for(int j = 0; j < threadPool.tidArr.workerNum; ++j){
            //    //    pthread_cancel(threadPool.tidArr.arr[j]);
            //    //    
            //    //}
            //    pthread_mutex_lock(&threadPool.mutex);
            //    threadPool.exitFlag = 1;
            //    pthread_cond_broadcast(&threadPool.cond);
            //    pthread_mutex_unlock(&threadPool.mutex);
            //    for(int j = 0; j < threadPool.tidArr.workerNum; ++j){
            //        pthread_join(threadPool.tidArr.arr[j],NULL);
            //    }
            //    printf("main thread is going to exit!\n");
            //    exit(0);
            //}

            //3 监听到netfds集合中有就绪的netfd,执行网盘相关业务，登录，发送指令，退出//////////////////
            else{
                //先接受客户端输入
                LOG_DEBUG("here");
                order_t order;
                memset(&order,0,sizeof(order));
                LOG_DEBUG("here");
                int ret_recvOrder = recv(readySet[i].data.fd, &order,sizeof(order_t),MSG_WAITALL);
                LOG_DEBUG("cmd = %d\n",order.cmd);
                LOG_DEBUG("here");

                // 3.0 关闭netfd的逻辑，判断客户端是否退出的逻辑
                if(ret_recvOrder == 0){     
                    //客户端主动断开连接或者异常断开
                    close(readySet[i].data.fd);//关闭netfd
                    LOG_DEBUG("here");
                    epollDel(epfd, readySet[i].data.fd);//移除监听集合
                    LOG_DEBUG("here");
                    continue;//进入下一个就绪fd的操作
                }
                int netfd = readySet[i].data.fd;
                // 3.1 判断长短命令，选择执行分支///////////////////////////////////////////////////////////////////
                //
                //短命令分支
                //////////////////////////////////////////////////////////////////////////////////////
                if(chooseCmd(&order)==1){
                    //短命令
                    //选择短命令分支执行对应操作
                    
                    //相应的netfd更新时间轮/////////////////////////////////////////////////////////
                    //
                    ElementID ele ={netfd};
                    insertElement(ele);

                    LOG_DEBUG("here");
                    switch(order.cmd){

                    case LS:
                        {
                            LOG_DEBUG("here");
                            dirStackType* dirstack;
                            findUserStackByUserName(&multiUserStack, order.username,&dirstack);
                            LOG_DEBUG("here");
                            ls(netfd, conn, dirstack);
                            LOG_DEBUG("here");
                            break;
                        }
                    case CD:
                        {   
                            dirStackType* dirstack;
                            LOG_DEBUG("here");
                            findUserStackByUserName(&multiUserStack, order.username,&dirstack);
                            LOG_DEBUG("here");
                            int ret = cd(conn,dirstack,order.parameters[0]);
                            LOG_DEBUG("here");
                            send(netfd, &ret, sizeof(int),0);
                            LOG_DEBUG("here");
                            break;
                        }
                    case RM:case RMDIR:
                        {
                            dirStackType* dirstack;
                            LOG_DEBUG("here");
                            findUserStackByUserName(&multiUserStack, order.username,&dirstack);
                            LOG_DEBUG("here");
                            int ret = rm(dirstack, order.parameters[0],conn);
                            LOG_DEBUG("here");
                            send(netfd, &ret, sizeof(int),0);
                            LOG_DEBUG("here");
                            break;
                        }
                    case PWD:
                        {
                            LOG_DEBUG("here");
                            dirStackType* dirstack;
                            LOG_DEBUG("here");
                            findUserStackByUserName(&multiUserStack, order.username,&dirstack);
                            LOG_DEBUG("here");
                            char buf[1024] = {0};
                            pwd(conn, dirstack, buf);
                            LOG_DEBUG("here");
                            send(netfd, buf, 1024, 0);
                            LOG_DEBUG("here");
                            break;
                        }

                    case MKDIR:
                        {   
                            LOG_DEBUG("here");
                            dirStackType* dirstack;
                            findUserStackByUserName(&multiUserStack, order.username,&dirstack);
                            LOG_DEBUG("here");
                            int ret = makeDir(dirstack, order.parameters[0],conn);
                            LOG_DEBUG("here");
                            printf("makeDir over\n");
                            send(netfd, &ret, sizeof(int),0);
                            LOG_DEBUG("here");
                            break;
                        }
                    case LOGIN:
                        {   
                            LOG_DEBUG("here");
                            dirStackType* dirstack;

                            dirStackInit(&dirstack);
                            LOG_DEBUG("here");
                            // 3.2 执行登录业务模块////////////////////////////////////////////////
                            char username[64] = {0};
                            LOG_DEBUG("here");
                            NetDiskInterface(netfd, conn, username);
                            deleteUserStackByStackName(&multiUserStack, username);
                            LOG_DEBUG("here");
                            strcpy(dirstack->userName,username);
                            LOG_DEBUG("here");
                            //int ret = findUserStackByUserName(&multiUserStack, username, &dirstack);
                            //if(ret == 0)
                            //{
                            //}

                            insertUserStack(&multiUserStack, dirstack);
                            LOG_DEBUG("here");
                            break;
                        }

                    case QUIT:
                        {
                            // 3.2 客户端主动退出,移除监听集合//////////////////////////////////////////////////
                            LOG_DEBUG("here");
                            close(netfd);//关闭netfd
                            LOG_DEBUG("here");
                            epollDel(epfd, readySet[i].data.fd);//移除监听
                            LOG_DEBUG("here");
                            break;
                        }

                    default:
                        break;

                    }

                }//短命令结束/////////////////////////////////////////////////////////////////

                //长命令分支
                ///////////////////////////////////////////////////////////////////////////////////
                LOG_DEBUG("here");
                if(chooseCmd(&order)==0){

                    //把netfd移除监听集合///////////////////关键一步//////////////////////////
                    epollDel(epfd,netfd);
                    //把netfd移除时间轮//////////////////////////////////////////////////////
                    removeElementById(netfd);

                    LOG_DEBUG("here");

                    dirStackType* dirstack;                    
                    findUserStackByUserName(&multiUserStack, order.username,&dirstack);
                    LOG_DEBUG("here");
                    //验证token///////////////////////////////////////////////////////////////
                    //验证操作
                    int flag = getCommand(netfd, &order);
                    LOG_DEBUG("here");
                    if(flag == -1){
                        //身份认证失败/////////////////////////////////////
                        close(netfd);
                        LOG_DEBUG("here");
                        epollDel(epfd, readySet[i].data.fd);
                        LOG_DEBUG("here");
                        continue;
                    }
                    //身份认证成功,发送标志位给客户端
                    LOG_DEBUG("here");
                    send(netfd, &flag, sizeof(int),0);
                    //长命令
                    //
                    //堆上分配一片空间存储指令结构体，子线程任务完成后 释放
                    LOG_DEBUG("here");
                    order_t* porder = (order_t* )malloc(sizeof(int)*2+10*32+64+512);
                    LOG_DEBUG("here");
                    memset(porder,0,sizeof(int)*2+10*32+64+512);
                    LOG_DEBUG("here");
                    memcpy(porder,&order,sizeof(int)*2+10*32+64+512);
                    LOG_DEBUG("here");
                    //
                    printf("I got 1 task!\n");
                    // 分配任务
                    pthread_mutex_lock(&threadPool.mutex);
                    LOG_DEBUG("here");
                    enQueue(&threadPool.taskQueue,netfd,porder,dirstack);
                    LOG_DEBUG("main_netfd = %d",netfd);
                    printf("I am master, I send a netfd  = %d\n", netfd);
                    LOG_DEBUG("here");
                    pthread_cond_broadcast(&threadPool.cond);
                    LOG_DEBUG("here");
                    pthread_mutex_unlock(&threadPool.mutex);
                    LOG_DEBUG("here");

                }
            }


        }
        //更新时间轮,踢人集合////////////////////////////////////////////////////////////////////////////////////
        int nums = updateTimeWheel(kickout_arr,1024);
        for(int i = 0; i < nums; ++i){
            //超时踢人/////////////////////////////////////////////////////////////////////////////////
            printf("kick out  netfd = %d\n", kickout_arr[i].id);
            close(kickout_arr[i].id);
            
        }
    }
    return 0;
}

