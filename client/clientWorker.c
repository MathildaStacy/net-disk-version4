
#include "hsqdef.h"
#include "analyOrder.h"
#include "fbr_gets_and_puts.h"
#include "puts.h"
void* threadworker(void* arg){

    //1 建立与服务端的连接/////////////////////////////////////////////////////////////////
    // 192.168.189.133 2000 是服务端的ip端口
    //与服务端建立连接
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(2000);
    serverAddr.sin_addr.s_addr = inet_addr("192.168.189.133");
    int ret = connect(sockfd,(struct sockaddr *)&serverAddr,sizeof(serverAddr));
    //ERROR_CHECK(ret,-1,"connect");
    
    //2 向服务端发送命令结构体信息/////////////////////////////////////////////////////////
    order_t* porder = (order_t* )arg;
    int ret_sendCmd = send(sockfd,porder,sizeof(int)*2+10*32+64+512,0);
    int flag = 0;//验证标志位
    int ret_recvFlag = recv(sockfd, &flag, sizeof(int),0);
    if(ret_recvFlag == 0){
        //非法访问直接退出
        close(sockfd);
        free(porder);
        exit(0);
    }

    printf("i want go\n");
    //合法访问
    //3 选择具体长命令执行/////////////////////////////////////////////////////////////////
    switch(porder->cmd){

         case PUTS:
            {
                printf("filename = %s,len = %ld\n",porder->parameters[0],strlen(porder->parameters[0]));                
                int ret = commandPuts_C(porder->parameters[0],sockfd);
                if(ret == -1){
                    printf("上传失败\n");
                }
                break;
            }
        case GETS:
            {
                int ret = client_download(sockfd, porder->parameters[0]);
                
                //
                //
                printf("跳出下载\n");

                if(ret == -1){
                    printf("下载失败\n");
                }
                break;
            }

    }

    printf("1跳出下载\n");
    //执行完具体业务后,close sockfd free porder////////////////////////////////////////////////
    close(sockfd);
    free(porder);
    //exit(0);
    printf("-----------------------rthreadexit\n");
    return NULL;
//    pthread_exit(NULL);
}
