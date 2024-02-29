#ifndef _OTHERS_H_
#define _OTHERS_H_

#include "analyOrder.h"

//改变当前打印的命令提示符信息
int changeCmdline(char* bufPrintf, order_t* porder);

//命令行提示符函数
int displayCmdLine(char* bufPrintf);

//长短命令选择函数
//长命令返回0，短命令返回1
int chooseCmd(order_t* porder);

#endif
