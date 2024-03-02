#ifndef _FBR_GETS_AND_PUTS_H_
#define _FBR_GETS_AND_PUTS_H_

#include "multi-user_dir_stack.h"
#include <mysql/mysql.h>

int sendFile(int sockfd, const char*sha1); //服务端  参数1：客户端的netfd

int recvFile(int sockfd, const char *sha1, const char *real_file_name);

int client_download(int sockfd, const char *file_name);

int server_send(MYSQL *conn, dirStackType *dirStk, const char *file_name, int sockfd);

#endif
