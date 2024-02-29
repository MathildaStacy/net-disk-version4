#ifndef _RM_MAKE_DIR_H
#define _RM_MAKE_DIR_H
#include <mysql/mysql.h>
#include"multi-user_dir_stack.h"
#include "hsqdef.h"
int rm(dirStackType *stack,char *fileName,MYSQL *con);
int makeDir(dirStackType *stack,char *dirName,MYSQL *con);
#endif

