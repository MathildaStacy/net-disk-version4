#ifndef _ANALYRODER_
#define _ANALYRODER_

enum{
    //cd ls rm pwd mkdir rmdir quit login 短命令
    //puts gets 长命令
    CD,//0
    LS,//1
    PUTS,//2
    GETS,//3
    RM,//4
    PWD,//5
    MKDIR,//6
    RMDIR,//7
    QUIT,//8
    LOGIN,//9
};
typedef struct order_s{
    
    int cmd;//命令//在服务段先用一个函数来判断是长命令还是短命令还是登录/注册/注销
    char parameters[10][32];
    int num; //参数的最大数量
    char username[64];//
    char password[32];
    char token[512];//令牌
}order_t;

//解析指令
//1、功能 拆分用户输入，判断是否合法，并将命令按格式输入到结构体中
int analyOrder(char* order, order_t* porder);
//解析命令
int analyCmd(char* cmd, order_t* porder);
//初始化结构体
int orderInit(order_t* porder);//参数数量
#endif
