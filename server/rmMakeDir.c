#include "hsqdef.h"
#include<mysql/mysql.h>
#include"pwd.h"
int rm(dirStackType *stack,char *fileName,MYSQL *con){
    char usrname[128]={0};
        strcat(usrname,stack->userName);
        char dir[128]={0};
        pwd(con,stack,dir);
    MYSQL_RES *res = NULL;
    MYSQL_ROW row;
    char com[128]={0};
    char fullPath[128]={0};
   snprintf(fullPath,128,"%s/%s",dir,fileName);
    int preid;
     snprintf(com,128,"SELECT fileId,type FROM files WHERE path='%s' and user ='%s'",fullPath,usrname);
     mysql_query(con,com);
      int preId;
      char type[32]={0};
      res = mysql_use_result(con);
      while ((row = mysql_fetch_row(res))) {
         preId=atoi(row[0]);
         strcat(type,row[1]);
     }

      mysql_free_result(res);

      bzero(com,128);
         snprintf(com,sizeof(com),"UPDATE files set tomb = '1' where path='%s' and user ='%s'",fullPath,usrname);
         int ret =  mysql_query(con,com);
         if(ret ==0)
         {
             return 0;
         }
         if(strcmp(type,"dir")==0){

bzero(com,128);
snprintf(com,sizeof(com),"UPDATE files set tomb = '1' where preId ='%d' and  user ='%s'",preId,usrname);
ret =  mysql_query(con,com);
      if(ret ==0)
          return 0 ;
         }
return -1;


}
int makeDir(dirStackType *stack,char *dirName,MYSQL *con)
{  char usrname[128]={0};
    strcat(usrname,stack->userName);

    printf("stack usrname = %s,len = %ld\n",stack->userName, strlen(stack->userName));
    printf("usrname = %s,len = %ld\n",usrname,strlen(usrname));

    char dir[1024]={0};
    pwd(con,stack,dir );
    MYSQL_RES *res = NULL;
    MYSQL_ROW row;
    char com [128]={0};
    int preid;
    char fullPath[128]={0};
    snprintf(fullPath,128,"%s/%s",dir,dirName);
    printf("pwd = %s\n",dir);
    printf("fullPath=%s\n",fullPath);
    printf("makeDir 50\n");
   snprintf(com,128,"SELECT fileId,tomb FROM files  WHERE path='%s' and user ='%s'",fullPath,usrname);
int ret =  mysql_query(con,com);
 if(ret==0)
 {
     printf("judge file sccess!\n");
 }    res =  mysql_store_result(con);
      printf("numRows=%ld\n",mysql_num_rows(res));
      printf("numRows=%ld\n",mysql_num_rows(res));
      if(mysql_num_rows(res)!=0)
      { row =mysql_fetch_row(res);
          mysql_free_result(res);
         if(strcmp(row[1],"1")==0)
         {
             bzero(com,0);
             snprintf(com,128,"update  FROM files set tomb =1  WHERE path='%s' and user ='%s'",fullPath,usrname);
             mysql_query(con,com);
             return 0;
         }

          return -1;
      }
   bzero(com,128);
    snprintf(com,128,"SELECT fileId FROM files  WHERE path='%s' and user ='%s'",dir,usrname);

    printf("makeDir 57\n");

          ret =  mysql_query(con,com);
         int preId=-1;
        MYSQL_RES   *res1 = mysql_use_result(con);
    printf("makeDir 58\n");

            while ((row = mysql_fetch_row(res1))) {
              preId=atoi (row[0]);
    }

    printf("makeDir 60\n");

            char pathSql[128]={0};

            snprintf(pathSql,128,"%s/%s",dir,dirName);
           bzero(com,128);
snprintf(com,sizeof(com),"INSERT INTO files  (filename, user, preId, path, type,sha1) values ('%s','%s',%d,'%s','dir','0')",dirName,usrname,preId,pathSql);

    printf("makeDir 67\n");

mysql_free_result(res1);
  ret =  mysql_query(con,com);
           if(ret== 0)
           { return 0 ;
           }
           else
           {
            return -1;
           }
    return 0;
}
