#ifndef __LOGINANDREGISTER_H__
#define __LOGINANDREGISTER_H__

#include "hsqdef.h"
#include "analyOrder.h"

typedef struct file_data
{
    int dataLen;
    char dataBuf[1000];
}File_Data_t;


#define SEND_SUCCESS {bzero(&fileData,sizeof(fileData));\
                bzero(sendBuf,sizeof(sendBuf));\
                fileData.dataLen=7;\
                strcpy(fileData.dataBuf,"success");\
                memcpy(sendBuf,&fileData,sizeof(fileData));\
                ret=send(netfd,sendBuf,fileData.dataLen+4,0);\
                ERROR_CHECK(ret,-1,"send");}
#define SEND_ERROR {bzero(&fileData,sizeof(fileData));\
                bzero(sendBuf,sizeof(sendBuf));\
                fileData.dataLen=5;\
                strcpy(fileData.dataBuf,"error");\
                memcpy(sendBuf,&fileData,sizeof(fileData));\
                ret=send(netfd,sendBuf,fileData.dataLen+4,0);\
                ERROR_CHECK(ret,-1,"send");}
#define SEND_FILEDATA {fileData.dataLen=strlen(fileData.dataBuf);\
                bzero(sendBuf,sizeof(sendBuf));\
                memcpy(sendBuf,&fileData,fileData.dataLen+4);\
                ret=send(netfd,sendBuf,fileData.dataLen+4,0);\
                ERROR_CHECK(ret,-1,"send");}
#define GET_FILEDATA {bzero(&fileData,sizeof(fileData));\
                ret=recv(netfd,&fileData.dataLen,4,0);\
                ERROR_CHECK(ret,-1,"recv");\
                ret=recv(netfd,fileData.dataBuf,fileData.dataLen,0);\
                ERROR_CHECK(ret,-1,"recv");}              

int SignIn_Deal(int netfd, MYSQL *conn);
int LogIn_Deal(int netfd, char *username, MYSQL *conn);
int NetDiskInterface(int netfd, MYSQL *conn, char *username);
int userRegister(char *username, int netfd);
int userLogin(char *username, int netfd);
int getCommand(int netfd, order_t* porder);

#endif
