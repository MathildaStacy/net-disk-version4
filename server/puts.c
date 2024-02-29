#include "puts.h"
#include "hsqdef.h"
#include "train.h"
#include "pwd.h"
#include "sql.h"
//#include "logger.h"
#include <openssl/sha.h>
#include <sys/file.h>
#include <mysql/mysql.h>
#include "file_content_to_sha1.h"
#define BUFSIZE 4096

//传输字符串
 int msgtrans(char *msg,int sockfd)
{
    train_t msgTrain;
    msgTrain.size = strlen(msg)+1;
    int rret =  send(sockfd,&msgTrain.size,sizeof(msgTrain.size),MSG_NOSIGNAL);
    if(rret == -1)
    {
        printf("transmgs erro\n ");
        return 1;
    }
    memcpy(msgTrain.buf,msg,msgTrain.size);
    rret = send(sockfd,msgTrain.buf,msgTrain.size,0);
    if(rret == -1)
    {
        printf("transmgs erro \n");
        return 1;
    }
    return 0;

}

//结收字符串
 int msgrecv( char* msg , int sockfd)
{
   train_t msgTrain;
   int rret = recv(sockfd,&msgTrain.size,sizeof(msgTrain.size),0);
   if(rret == 0)
   {
       printf("get msg erro\n");
       return -1;
   }
   rret =  recv(sockfd,msgTrain.buf,msgTrain.size,0);
   if(rret == 0)
   {
       printf("get msg erro\n");
       return -1;
   }
   memset(msg,0,sizeof(msg));
   memcpy(msg,msgTrain.buf,msgTrain.size);
   return 0;
}

 //使用了的话编译记得链接-lcrypto库-lssl
 //sha1buf大小为char[41]
 //如过返回值为-1则说明没有这个文件,将sha1buf设置为0
 int sha1file(const char* filename, char *sha1buf)
{
    int opfd = open(filename,O_RDWR);
    if (opfd == -1)
    {
        printf("file is not exist\n");
        memset(sha1buf,0,sizeof(sha1buf));
        return -1;
    }

    else
    {
     int  rret = file_content_to_sha1(opfd,sha1buf,41); 
     if(rret == -1)
     {
         printf("sha1 is erro\n");
         return -1;
     }
    }
    close(opfd);
    return 0;
}

//此函数为文件传输  with offset
 int putsfile(char* filename , int netfd, long offset)
{
    printf("let puts\n");
    int opfd = open(filename,O_RDWR); 
    struct stat file_stat ;
    int rret  = fstat(opfd ,&file_stat);
    if(rret == -1)
    {
        printf("putsfile rret erro\n");
        exit(-1);
    }
    long filesize = file_stat.st_size;
    lseek(opfd,offset,SEEK_SET);
    //trans filesize ,let getpart konw how long they need get
    char filesize_buf[1024];
    memcpy(filesize_buf,&filesize,sizeof(filesize));
    msgtrans(filesize_buf,netfd);
    rret = sendfile(netfd , opfd , 0 , filesize-offset);
    ERROR_CHECK(rret, -1 ,"sendfile");
    close(opfd);
    printf("puts over\n");
    return 0;
}

//写入打开的文件描述符,使用打开的文件描述符可以不用在函数内部打开文件文件描述符
//方便创建文件和传输
 int getfile(int sockfd,int opfd ,off_t offset)
{
    //get filesize
    char filesizebuf[1024];
    msgrecv(filesizebuf,sockfd);
    long filesize ;
    memcpy(&filesize,filesizebuf,sizeof(filesize));
    
    //得到文件大小，接收剩余大小
    //------------------------------------------------------------
    char buf [1];
    int cursize = offset;
    while(cursize<filesize)
    {
        
        int bufsize =sizeof(buf);
        int getrecv;
        memset(buf,0,bufsize);
        if(filesize - cursize > bufsize )
        {
          getrecv = recv(sockfd,buf,bufsize,MSG_WAITALL);
          if(getrecv==0)
          {
              printf("read to write erro\n");
              break;
          }
          cursize += getrecv;
          write(opfd,buf,getrecv);
        }
        else 
        {
          getrecv = recv(sockfd,buf,filesize-cursize,MSG_WAITALL);
          if(getrecv==0)
          {
              printf("read to write erro\n");
              break;
          }
          cursize += getrecv;
          write(opfd,buf,getrecv);
        }
        //-----------------------------------------------------------------
    }
    printf("get over\n");
    return 0;
}
int commandPuts_S(MYSQL * conn,dirStackType * virtual_path,int sockfd)
{
    //接收哈希
    char hashbuf[41];
    msgrecv(hashbuf,sockfd);
    if(strlen(hashbuf)<3)
    {
        printf("client need check\n");
        return -1;
    }
    char filename[128];
    msgrecv(filename,sockfd);
    //查询有无此文件,存在返回1，不在返回0
    int isExist = 0;
    off_t offset =0;
    char offsetbuf[1024];
    //用于得到父目录
    char virtual_filepath[1024]; 
    int pwdret = pwd(conn,virtual_path,virtual_filepath);
    if(pwdret==-1)
    {
        printf("pwd wrong!\n");
        return  -1;
    }
    File isExist_file;
    //使用dbret判断是否存在，-1不存在，0存在，-2异常
    int dbret = dbFindFileBySha1(conn,hashbuf,&isExist_file);
    printf("preid == %d\n    tomb ==%d\n   filename =%s\n   sha1=%s \n",isExist_file.pre_id,
                                                        isExist_file.tomb,
                                                        isExist_file.filename,
                                                        isExist_file.sha1);
    if(dbret == 0)
    {
        msgtrans("1",sockfd);
        printf("file is exist\n");
        isExist=1;
    }
    else if (dbret == -1)
    {
        isExist = 0;
    }
    else if(dbret == -2)
    {
        printf("dbFindFileBySha1 erro\n");
        return -1;
    }
    //如果文件存在
    if(isExist == 1)
    {
        //此时的isexist文件内有目录下的信息

        
                    
           //得到preid对比 
           int preid;
           int getheadret =  getHead(virtual_path,&preid);
           if(getheadret == 1)
           {
               preid = -1;
           }

           //结合做判断
           //路径相同且墓碑值为0,直接返回
           printf("preid == %d\n    tomb ==%d\n   filename =%s\n   sha1=%s \n",isExist_file.pre_id,
                                                        isExist_file.tomb,
                                                        isExist_file.filename,
                                                        isExist_file.sha1);
           if((preid == isExist_file.pre_id)&&(isExist_file.tomb==0))
           {
               printf("there is same file\n");
               return 0;
           }
           //路径相同且墓碑值为1，将墓碑置为0
           if((preid==isExist_file.pre_id)&&(isExist_file.tomb==1))
           {
              int recoverret =  recoverFileById(conn,isExist_file.fileId);
              if(recoverret==-1)
              {
                  printf("recover erro\n");
                  return -1;
              }
               printf("ok ,recovered successfully\n"); 
               //置零
            
               return 0;
           }



           File writefile;
           strcpy(writefile.filename,filename);
           char user[30];
           strcpy(user,virtual_path->userName);
           strcpy(writefile.user,user); 
           writefile.pre_id=preid;
            printf("插入文件preid = %d\n",preid);
            

           char fullpath[1024];
           sprintf(fullpath,"%s/%s",virtual_filepath,filename);
           strcpy(writefile.absPath,fullpath);
           strcpy(writefile.type,"f");
           strcpy(writefile.sha1,hashbuf);
           writefile.tomb=0;
           printf("插入文件：writefile.preid = %d\n",writefile.pre_id);
           addFile(conn,writefile);

           return 0;
    }

    //如果不存在
    if(isExist == 0)
    {
        msgtrans("0",sockfd);
        //注册表内没有说明可能没有或是未传输完
       int opfd =open(hashbuf,O_RDWR);
       //情况1：打开失败说明没有此文件
       if(opfd == -1)
       {
           //此时发送文件0
           memcpy(offsetbuf,&offset,sizeof(offset));
           int rret =  msgtrans(offsetbuf,sockfd);
           if(rret == 1)
           {
               printf("trans filesize erro\n");
               close(opfd);
               return -1;
           }
           //重新创建文件
           int newfd = open(hashbuf,O_RDWR|O_CREAT,0660);
           //发送完offset后开始接收
           getfile(sockfd,newfd,0);
           close(newfd);
       }
       //情况2：打开成功说明之前有文件
       else
       {
           if (flock(opfd, LOCK_EX) < 0) 
           { // 对文件加排它锁
               perror("flock");
               exit(EXIT_FAILURE);
           }
            struct stat file_stat;
            int rret = fstat(opfd,&file_stat);
            if(rret == -1)
            {
                printf("fstat erro\n");
                return -1;
            }
            offset = file_stat.st_size;
           memcpy(offsetbuf,&offset,sizeof(offset));
           rret =  msgtrans(offsetbuf,sockfd);
           if(rret == 1)
           {
               printf("trans filesize erro\n");
               return -1;
           }
       //发送完offset后开始接收
           getfile(sockfd,opfd,offset);
           if (flock(opfd, LOCK_UN) < 0) 
           { // 解锁文件
               perror("flock");
               exit(EXIT_FAILURE);
           } 
       }
       //最后一次对比hash是否正确
       char jugehash[41];
       sha1file(hashbuf,jugehash);
       if(strcmp(hashbuf,jugehash)==0)
       {
           //1 is ok;
           char checkbuf[]="1";
           msgtrans(checkbuf,sockfd);
           printf("get ok\n");
           //写入文件目录表
           File writefile;
           strcpy(writefile.filename,filename);
           char user[30];
           strcpy(user,virtual_path->userName);
           strcpy(writefile.user,user); 

           int preid;
           int getheadret =  getHead(virtual_path,&preid);
           if(getheadret == 1)
           {
               preid = -1;
           }
           writefile.pre_id=preid;
            printf("插入文件preid = %d\n",preid);
            

           char fullpath[1024];
           sprintf(fullpath,"%s/%s",virtual_filepath,filename);
           strcpy(writefile.absPath,fullpath);
           strcpy(writefile.type,"f");
           strcpy(writefile.sha1,hashbuf);
           writefile.tomb=0;
           printf("插入文件：writefile.preid = %d\n",writefile.pre_id);
           addFile(conn,writefile);

       }
       else
       {
           char checkbuf[]="0";
           msgtrans(checkbuf,sockfd);
           printf("get erro\n");
           if (unlink(hashbuf) != 0) {
               perror("Failed to delete file");
               exit(EXIT_FAILURE);
           } 

       }
       close(opfd);

    }
    return 0;
}

int commandPuts_C(char* filename,int sockfd)
{ 
    //发送哈希
    char hashbuf[41];
    sha1file(filename,hashbuf);
    msgtrans(hashbuf,sockfd);
    msgtrans(filename,sockfd);
    
    //接收此时文件大小
    char offsetbuf[1024];
    msgrecv(offsetbuf,sockfd);
    off_t offset ;
    memcpy(&offset,offsetbuf,sizeof(off_t));
    //通过文件偏移量传输文件
    putsfile(filename,sockfd,offset);
    
    //check get msg 1 is ok ,0 is erro
    char checkbuf[3];
    msgrecv(checkbuf,sockfd);
    if(strcmp(checkbuf,"1")==0)
    {
        printf("OK!\n");
    }
    else
    {
        printf("erro\n");
    }
    return 0;
}
