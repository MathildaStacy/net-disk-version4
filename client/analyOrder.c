#include "hsqdef.h"
#include "analyOrder.h"
#define PAR_LEN 32

int analyOrder(char* order, order_t* porder){

    //获取命令
    char* p = strtok(order,"\n ");
    if(p == NULL){
        return -1;
    }
    // printf("%s\n",p);//测试
    int ret = analyCmd(p,porder);
    // printf("%d\n",porder->cmd);
    //判断命令
    if(ret == -1){
        printf("命令输入错误!!!\n");
        return -1;
    }
    //获取参数
    for(int i = 0; i < 10; ++i){
        p = strtok(NULL,"\n ");
        //printf("%s\n",p);//测试
        if(p == NULL){
            break;
        }
        memcpy(porder->parameters[i], p, PAR_LEN);
        porder->num++;
    }
    return 0;
}

int analyCmd(char* cmd, order_t* porder){

    if(memcmp(cmd,"cd",2) == 0&&strlen(cmd)==2){porder->cmd = CD;return 0;}
    if(memcmp(cmd,"ls",2) == 0&&strlen(cmd)==2){porder->cmd = LS; return 0;}
    if(memcmp(cmd,"puts",4) == 0&&strlen(cmd)==4){porder->cmd = PUTS;return 0;}
    if(memcmp(cmd,"gets",4) == 0&&strlen(cmd)==4){porder->cmd = GETS;return 0;}
    if(memcmp(cmd,"rm",2) == 0&&strlen(cmd)==2){porder->cmd = RM;return 0;}
    if(memcmp(cmd,"pwd",3) == 0&&strlen(cmd)==3){porder->cmd = PWD;return 0;}
    if(memcmp(cmd,"mkdir",5) == 0&&strlen(cmd)==5){porder->cmd = MKDIR;return 0;}
    if(memcmp(cmd,"rmdir",5) == 0&&strlen(cmd)==5){porder->cmd = RMDIR;return 0;}
    if(memcmp(cmd,"quit",4) == 0&&strlen(cmd)==4){porder->cmd = QUIT;return 0;}
    return -1;
}
//初始化
int orderInit(order_t* porder){

    //参数上限 num, 参数长度最长 32；
    porder->cmd = -1;
    porder->num = 0;
    for(int i = 0; i < 10; ++i){
        memset(porder->parameters[i],0,32);   
    }
    memset(porder->username,0,64);
    memset(porder->password,0,32); 
    memset(porder->token,0,512);//token 置空
    return 0;
}
//int main(){
//    
//    char buf[] = "pwda  /dir/file2 dir3/dir4 dir5 dir6";
//    order_t order;
//    orderInit(&order);
//    int ret = analyOrder(buf,&order);
//    if(ret == -1){
//        exit(-1);
//    }
//    for(int i = 0; i < 10; ++i){
//        printf("%s\n",order.parameters[i]);
//    }
//    return 0;
//}





