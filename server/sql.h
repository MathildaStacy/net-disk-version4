#ifndef __SQL_H__
#define __SQL_H__
//#include "head.h"
//#include "ser_cli.h"
#include <mysql/mysql.h>
#define STR_LEN 10

typedef struct node
{
    char path[30];
    struct node *next; 
}node;
typedef struct {
    char filename[30];
    int filesize;
    char filetype[20];
    char md5sum[50];
}File_info;

typedef struct {
int fileId; // 文件id
char filename[128]; // 文件名
char user[30]; // 文件所属用户id
int pre_id; // 上一级目录id
// 从根目录开始的绝对路径：根据你的需要选择是否实现
char absPath[200]; // 绝对路径（可选）
char type[20]; // 文件类型
char sha1[41]; // 文件的sha1值
int tomb; // 墓碑值
} File;

int sqlConnect(MYSQL **conn);//建立mysql连接
void addUser(MYSQL *conn ,char *name,char *salt,char *password);//注册成功，向数据库中插入新用户
int findUserByName(MYSQL *conn, char *name,char* salt, char* password);//通过用户名，查询盐值
void addFile(MYSQL *conn, File file_s);
int getFileDataById(MYSQL *conn, int fileId, File *file_s);//获取文件详细信息
char* getFilename(MYSQL *conn, int fileId);
int findFilesByPreId(MYSQL *conn, int preId, int *fileIds);     //更新
int dbFindFileBySha1(MYSQL *conn, const char* sha1, File* file); //更新
//int dbFindFileByDirId(int directoryId, File *file);
int getFileIdByPath(MYSQL *conn,const char *path);
int getPreIdByFilename(MYSQL *conn,const char *path, const char * filename);
int deleteFileById(MYSQL *conn, int fileId);
int recoverFileById(MYSQL *conn, int fileId);
void operationLog(MYSQL *conn, const char * uname, const char *action, const char *result);

void get_salt(char *str);
int math_user(MYSQL *conn,char *name,char *password,char *token);
int math_token(MYSQL *conn,char *name,char *token);
void ls_func(MYSQL *conn,char*name,int code,char *buf);
//int operate_func(MYSQL *conn,Train_t *ptrain,QUR_msg *pqq_msg,char *name,int *pcode);
int find_pre_code(MYSQL *conn,char*path,int pcode);
int find_later_code(MYSQL *conn,int cur_code,char *filename,char *name);
int find_later_file(MYSQL *conn,int cur_code,char *filename,char *name);
//int cd_func(MYSQL *conn,Train_t *ptrain,QUR_msg *pqq_msg,char *name,int *pcode);
int deleteFile(MYSQL *conn, int uid,int fileid);
//int find_file_info(MYSQL *conn,File_info*,char*name,int code);
//int math_uoload(MYSQL *conn,File_info *pfile_info,char*name,int code);
//void add_file(int code,char *name,File_info *pf);
void loginLog(const char *action,const char *name,const char *ip,const char *result);//登录&注册
//int get_mqbuf(MYSQL *conn,MQ_buf *pf);


#endif

