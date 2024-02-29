#ifndef __PWD_H__
#define __PWD_H__

#include "multi-user_dir_stack.h"
#include "sql.h"


int pwd(MYSQL *conn, dirStackType *dirStk, char *str);


#endif
