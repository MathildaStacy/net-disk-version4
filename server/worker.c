#include "worker.h"
#include "taskQueue.h"
#include "threadPool.h"
#include "loginAndRegister.h"
#include "multi-user_dir_stack.h"
#include "puts.h"
#include "fbr_gets_and_puts.h"
#include "logger.h"
#include "pwd.h"
int tidArrInit(tidArr_t * ptidArr, int workerNum){
    // 申请内存 存储每个子线程的tid
    ptidArr->arr = (pthread_t *)calloc(workerNum,sizeof(pthread_t));
    ptidArr->workerNum = workerNum;
    return 0;
}
void unlock(void *arg){
    threadPool_t * pthreadPool = (threadPool_t *)arg;
    printf("unlock!\n");
    pthread_mutex_unlock(&pthreadPool->mutex);
}
void * threadFunc(void *arg){
    LOG_DEBUG("here");
    threadPool_t * pthreadPool = (threadPool_t *)arg;
    LOG_DEBUG("here");
    while(1){
        // 1 获取任务队列队首元素/////////////////////////////////////////////////////////////////
        //一个循环代表了一个用户
        LOG_DEBUG("here");
        pthread_mutex_lock(&pthreadPool->mutex);
        int netfd;
        LOG_DEBUG("here");
        order_t* porder;//命令结构体指针
        dirStackType* dirstack;
        //pthread_cleanup_push(unlock,pthreadPool);
        while(pthreadPool->exitFlag == 0 && pthreadPool->taskQueue.queueSize <= 0){
            LOG_DEBUG("here");
            pthread_cond_wait(&pthreadPool->cond, &pthreadPool->mutex);
        }
        if(pthreadPool->exitFlag == 1){
            LOG_DEBUG("here");
            printf("I am child, I am going to exit!\n");
            LOG_DEBUG("here");
            pthread_mutex_unlock(&pthreadPool->mutex);
            LOG_DEBUG("here");
            pthread_exit(NULL);
        }
        //获取netfd 和 命令结构体指针
        LOG_DEBUG("here");
        netfd = pthreadPool->taskQueue.pFront->netfd;
        LOG_DEBUG("here");
        porder = pthreadPool->taskQueue.pFront->porder;//order结构体 堆上
        LOG_DEBUG("here");
        dirstack = pthreadPool->taskQueue.pFront->dirstack;
        LOG_DEBUG("here");
        printf("I am worker, I got a netfd = %d\n", netfd);
        LOG_DEBUG("here");
        deQueue(&pthreadPool->taskQueue);
        LOG_DEBUG("here");
        pthread_mutex_unlock(&pthreadPool->mutex);
        LOG_DEBUG("here");

        // 2 连接数据库，每个线程建立自己与数据库的连接,线程不安全,上锁///////////////////////////
        pthread_mutex_lock(&pthreadPool->mutexMysql);
        LOG_DEBUG("here");
        MYSQL* conn = mysql_init(NULL);
        LOG_DEBUG("here");
        char *host = "localhost";
        char *user = "root";
        char *password = "123456";
        char *database = "netdisk";
        LOG_DEBUG("filename =|%s|,len =%ld",porder->parameters[0],strlen(porder->parameters[0]));
        MYSQL *ret = mysql_real_connect(conn,host,user,password,database,0,NULL,0);
        LOG_DEBUG("here");
        pthread_mutex_unlock(&pthreadPool->mutexMysql);
        LOG_DEBUG("here");
        if(ret == NULL){
            //如果数据库没有连接成功，就断开与客户端的连接
            printf("Error: %s\n", mysql_error(conn));
            close(netfd);
            continue;
        }
        printf("数据库连接成功\n");

        // 3 执行上传下载业务分支//////////////////////////////////////////////////////////////////
        switch(porder->cmd){

        case PUTS:
            {
                LOG_DEBUG("here");
                char pwdStr[50] = {0};
                pwd(conn, dirstack, pwdStr);
                int head_file_id = 0;
                getHead(dirstack, &head_file_id);
                LOG_DEBUG("dirstack.user = |%s|, pwd = |%s|, head_file_id = %d", dirstack->userName, pwdStr, head_file_id);
                LOG_DEBUG("tread_netfd: %d", netfd);
                //传输
                commandPuts_S(conn, dirstack, netfd);
                LOG_DEBUG("here");
                break;
            }
        case GETS:
            {

                LOG_DEBUG("here");
                server_send(conn,dirstack,porder->parameters[0],netfd);
                LOG_DEBUG("here");
                break;
            }
        }

        // 4 执行结束,释放指令结构体////////////////////////////////////////////////////////////////
        LOG_DEBUG("here");
        printf("I am worker, netfd = %d, over\n",netfd);
        //子线程结束业务，用户退出，断开和用户的连接，然后继续等待下一个用户的连接
        mysql_close(conn);
        LOG_DEBUG("here");
        free(porder);
        LOG_DEBUG("here");
        close(netfd);
    }
}
