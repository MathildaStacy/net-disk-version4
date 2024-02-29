#ifndef _EXEPANCLIENT_
#define _EXEPANCLIENT_
#include "analyOrder.h"

//4网盘业务客户端, 参数增加 token
int exePanClient(int socked, char* usrname, char* token);


//打印命令行提示符
int displayCmdLine(char* bufPrintf);


//改变当前打印命令行提示符信息
//cd 操作 参数 是正确的才能执行本函数
int changeCmdline(char* bufPrintf, order_t* porder);
//1 第一个参数传入当前的命令提示符信息
//2 选择不同改变操作的标志位，根据客户标准输入的cd命令的参数来执行不同操作
//cd .. 返回上级目录，需要删减
//cd .  返回当前目录，返回当前目录，不变
//cd dir1 拼接

#endif


