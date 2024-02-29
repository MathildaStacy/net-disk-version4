#ifndef __CD_H__
#define __CD_H__

#include "multi-user_dir_stack.h"
#include "sql.h"


int ls(int netfd, MYSQL *conn, dirStackType *dirStk);

int cd(MYSQL *conn, dirStackType *dirStk, char *str);

#endif
