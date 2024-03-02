#include "hsqdef.h"
#include "multi-user_dir_stack.h"

int ls(int netfd, MYSQL *conn, dirStackType *dirStk) {
    int ret;
    int id;
    if(isEmpty(dirStk)){
        id = -1;
    }
    getHead(dirStk, &id);
    MYSQL_RES *res;
    MYSQL_ROW row;

    char query[2000]={0};
    char lsBuf[1000]={0};

    sprintf(query,"select * from files where preId = %d and user = '%s' and tomb <> 1;", id, dirStk->userName);
    if (mysql_query(conn, query)) {
        printf("ls error: %s\n", mysql_error(conn));
        strcpy(lsBuf, "error");
        ret = send(netfd, lsBuf, sizeof(lsBuf), 0);
        ERROR_CHECK(ret, -1, "send");
        mysql_free_result(res);
        return -1;

    } else {
        res = mysql_use_result(conn);
        if (res) {
        bzero(lsBuf, sizeof(lsBuf));          // 清空lsBuf
        while ((row = mysql_fetch_row(res)) != NULL) {
            char tempBuf[100];
            sprintf(tempBuf, "%s ", row[1]);  // 暂存当前文件名及空格
            strcat(lsBuf, tempBuf);           // 将当前文件名及空格追加到 lsBuf
        }
        send(netfd, lsBuf, sizeof(lsBuf), 0);
        mysql_free_result(res);
        }
    }
    return 0;
}

