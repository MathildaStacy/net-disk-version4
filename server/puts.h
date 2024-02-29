#ifndef _PUTS_H_
#define _PUTS_H_
#include <mysql/mysql.h>
#include "pwd.h"
#include "multi-user_dir_stack.h"
int msgtrans(char *msg,int sockfd);

int msgrecv(char* msg , int sockfd);

int sha1file(const char * filename , char *sha1buf);

int putsfile(char* name , int netfd,long offset);
int getfile(int sockfd,int opfd ,long offset);

int commandPuts_C(char* filename,int sockfd);
int commandPuts_S(MYSQL * conn,dirStackType*virtual_path,int sockfd);

#endif











