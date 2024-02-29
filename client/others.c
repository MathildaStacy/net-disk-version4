#include "others.h"
#include "hsqdef.h"

//命令行提示符函数
int displayCmdLine(char* bufPrintf){

    printf("\033[32m%s\033[0m",bufPrintf);
    fflush(stdout);
    return 0;
}
//改变当前打印的命令提示符信息
int changeCmdline(char* bufPrintf, order_t* porder){

    if(memcmp(porder->parameters[0],"..",2)==0&&strlen(porder->parameters[0])==2){
        //cd ..
        int lastIndex = strlen(bufPrintf)-1;
        int i;
        for(i = lastIndex; i > 0; --i){
            if(bufPrintf[i] == '/'){
                memset(bufPrintf+i,0,strlen(bufPrintf)-i);
                break;
            }
        }
        bufPrintf[i] = '$';
        bufPrintf[i+1] = ' ';
    }
    else if(memcmp(porder->parameters[0],".",1)==0&&strlen(porder->parameters[0])==1);
    else{
        //cd dir1
        printf("%ld\n",strlen(bufPrintf));
        bufPrintf[strlen(bufPrintf)-2] = '/';
        bufPrintf[strlen(bufPrintf)-1] = 0;
        sprintf(bufPrintf, "%s%s%s",bufPrintf,porder->parameters[0],"$ ");
    }
    return 0;
}

//长短命令选择函数
//长命令返回0，短命令返回1,命令错误返回-1
int chooseCmd(order_t* porder){

    switch(porder->cmd){
    //短命令
    case LS: case CD: case RM : case PWD :case MKDIR : case RMDIR :case  QUIT: case LOGIN: 
        {
            return 1;
        }
    //长命令
    case PUTS:case GETS:
        {

            return 0;
        }
    }
    //错误命令，在进入选择长短命令前已经拦截
    return -1;
}





