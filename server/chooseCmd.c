#include "chooseCmd.h"

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

