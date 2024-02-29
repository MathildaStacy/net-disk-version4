#include "cd.h"

#include "sql.h"
#include "multi-user_dir_stack.h"
#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>


//测试
#include "pwd.h"



int cd(MYSQL *conn, dirStackType *dirStk, char *str)
{
    if(isEmpty(dirStk) && (strcmp(str, "..")) == 0)
    {
        return -1; //当前就是根目录
    }
    else if(!isEmpty(dirStk) && (strcmp(str, "..")) == 0)
    {
        int dirId = 0;
        stkPop(dirStk, &dirId);
        return 0;
    }

    int pid = 0;
    if(isEmpty(dirStk))
    {
        pid = -1;
    }
    else {
        getHead(dirStk, &pid);
    }
    
    
    printf("cd 33 : pos1\n");
    int file_id[1024];
    int n = findFilesByPreId(conn, pid, file_id);

     
    printf("cd 38 : pos2\n");
    
    for(int i = 0; i < n; i++)
    {
        

        printf("cd 44 : pos3\n");
        File file_s;
        bzero(&file_s, sizeof(file_s));
        int ret = getFileDataById(conn, file_id[i], &file_s);
        printf("file id found %d in cd.c\n", file_id[i]);
        printf("file_s.filename = |%s|, file_s.user =|%s|\n", file_s.filename, file_s.user);
        if(ret == -1)
        {
            printf("dir not found!\n");
            return -1; //目录不存在
        }
        if(strcmp(str, file_s.filename) == 0 && strcmp(file_s.user, dirStk->userName) == 0)
        {
            printf("found the dir! file\n");
            stkPush(dirStk, file_id[i]);
            return 0;
        }
        
    }

    
    return -1;
}


//int g_log_level = 0;
//
//
//int main()
//{
//    
//    dirStackType *dirStk;
//    dirStackInit(&dirStk);
//
//    strcpy(dirStk->userName, "user1");
//    
//
//
//    MYSQL *conn;
//    sqlConnect(&conn);
//    
//    
//    int x = 0;
//    getHead(dirStk, &x);
//    printf("83 head = %d\n", x);
//    
//    cd(conn, dirStk, "dir1");
//    
//    
//
//    char path[2048];
//    bzero(path, sizeof(path));
//    
//    pwd( conn,dirStk, path);
//    printf("1:%s\n", path);
//    
//    bzero(path, sizeof(path));
//    pwd( conn,dirStk, path);
//    printf("2:%s\n", path);
//
//    cd(conn, dirStk, "dir2");
//    x = 0;
//    getHead(dirStk, &x);
//    printf("88 head = %d\n", x);
//    
//    bzero(path, sizeof(path));
//    pwd( conn,dirStk, path);
//    printf("3:%s\n", path);
//
//    
//}

