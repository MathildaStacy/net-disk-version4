#include "fbr_gets_and_puts.h"
#include "train.h"
//#include "logger.h"
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sys/file.h>


#define ONE_MB 1024 * 1024

int recvn(int sockfd, void* buf, long size)
{
    char* p = (char*)buf;
    int curSize = 0;
    while (curSize < size) {
        int getrecv = recv(sockfd, p + curSize, size - curSize, MSG_WAITALL);
        if (getrecv == 0) {
            // 如果接收值为0，说明对方正常关闭连接
            return curSize;
        }
        curSize += getrecv;
    }
    
    return curSize;;
}

int sendn(int sockfd, const void* buf, long size)
{
    const char* p = (const char*)buf;
    int curSize = 0;
    while (curSize < size) {
        int sent = send(sockfd, p + curSize, size - curSize, 0);
        if (sent < 0) {
            // 出错处理
            perror("send error");
            return -1;
        }
        curSize += sent;
    }
    
    return 0;
}

// 介绍一个新的辅助函数，用于读取和更新接收进度的记录文件
long updateOffsetRecord(const char *filename, long offset, int reset) {
    char recordFilename[300]; // 保证可以存放原文件名+后缀
    snprintf(recordFilename, sizeof(recordFilename), "%s.offset", filename);

    if (reset) {
        unlink(recordFilename);
        return 0;
    }

    if (offset == -1) {
        int fd = open(recordFilename, O_RDONLY);
        if (fd == -1) return 0;
        long storedOffset;
        if (read(fd, &storedOffset, sizeof(storedOffset)) != sizeof(storedOffset))
            storedOffset = 0;
        close(fd);
        return storedOffset;
    } else {
        int fd = open(recordFilename, O_RDWR | O_CREAT, 0666);
        if (fd == -1) return 0;
        write(fd, &offset, sizeof(offset));
        close(fd);
        return offset;
    }
}

void createAndWriteIfNotExists(const char* filePath) {
    int fd;
    struct stat buffer;

    // 检查文件是否存在
    if(stat(filePath, &buffer) != 0) {
        // 如果不存在，创建文件并写入long类型的0
        printf("filename is %s\n", filePath);
        fd = open(filePath, O_RDWR | O_CREAT, 0666); // 创建文件，如果文件不存在
        if (fd == -1) {
            perror("open");
            return; // 打开或创建文件失败
        }

        long zero = 0;
        if(write(fd, &zero, sizeof(zero)) != sizeof(zero)) {
            perror("write");
            close(fd);
            return; // 写入失败
        }

        printf("文件不存在，已创建并写入long类型的0。\n");
    } else {
        printf("文件已存在。\n");
    }

    if (fd != -1) {
        close(fd); // 关闭文件描述符
    }
}


int recvFile(int sockfd, const char* sha1) {
    char filename[256] = {0};
    long fileSize;
    off_t offset = 0;

    strcpy(filename, sha1);
    fileSize = strlen(filename);
    //向服务端用小火车发送文件名  1
    train_t file_name_train;
    memcpy(&file_name_train.size, &fileSize, sizeof(file_name_train.size));
    memcpy(file_name_train.buf, filename, file_name_train.size);
    printf("in train: file name size = %ld\n", file_name_train.size);
    sendn(sockfd, &file_name_train.size, sizeof(file_name_train.size));
    sendn(sockfd, file_name_train.buf, file_name_train.size);
    printf("1\n");

/*
    //接收服务端发送的错误信息  2
    train_t train_error_msg; 
    recvn(sockfd, &train_error_msg.size, sizeof(train_error_msg.size));
    recvn(sockfd, train_error_msg.buf, train_error_msg.size);
    int error_msg = 0;
    memcpy(&error_msg, train_error_msg.buf, train_error_msg.size);
    printf("2\n");
    if(error_msg == 1)
    {
        return -1;
    }

*/  

    //检查本地文件是否有以前的传输记录，如果没有就创建
    char fileRecord[256] = {0};
    sprintf(fileRecord, "%s.offset", filename);
    createAndWriteIfNotExists(fileRecord);

    //从服务端接收文件大小  3
    if (recvn(sockfd, &fileSize, sizeof(fileSize)) == 0) {
        printf("Failed to receive file size.\n");
        return -1;
    }
    printf("recved file size = %ld\n", fileSize);

    int fd = open(filename, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("Unable to open file");
        return -1;
    }

    //打开后对文件上锁
    if(flock(fd, LOCK_EX) == -1)
    {
        return -1;
    } 

    //向服务端发送偏移量  4  握手完毕
    offset = updateOffsetRecord(filename, -1, 0);
    printf("offset = %ld\n", offset);
    sendn(sockfd, &offset, sizeof(offset));

    if (offset >= fileSize) {
        updateOffsetRecord(filename, 0, 1);
        close(fd);
        printf("File already completely received, skipping download.\n");
        return 0;
    }

    // 调整文件大小
    ftruncate(fd, fileSize);

    // 对整个文件进行mmap映射
    void *mapped = mmap(NULL, fileSize, PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("mmap error");
        close(fd);
        return -1;
    }
    close(fd); // 映射后可以关闭文件描述符
    int recv_count = 0;
    while (offset < fileSize) {
        ssize_t toReceive = (fileSize - offset > ONE_MB) ? ONE_MB : fileSize - offset;
        ssize_t receivedBytes = recvn(sockfd, (char*)mapped + offset, toReceive);
        recv_count++;
        printf("recv once! recv size = %ld, count = %d\n", receivedBytes, recv_count);
        offset += receivedBytes;
        updateOffsetRecord(filename, offset, 0);

        // 此处不再需要手动进行数据写入操作，因为mmap映射会自动同步到文件
    }

    munmap(mapped, fileSize);
    updateOffsetRecord(filename, 0, 1); // 传输完成后，清理记录文件
    flock(fd, LOCK_UN);

    printf("File successfully received.\n");
    return 0;
}



int sendFile(int sockfd, const char *sha1)
{
    //从客户端接收文件名  1
    printf("0.0\n");
    train_t file_name_train;
    recvn(sockfd, &file_name_train.size, sizeof(file_name_train.size));
    printf("0.1 file name size = %ld\n", file_name_train.size);
    recvn(sockfd, file_name_train.buf, file_name_train.size);
    printf("0.2\n");
    char temp_filename[256] = {0};
    memcpy(temp_filename, file_name_train.buf, file_name_train.size);
    printf("file name is : %s\n", temp_filename);
    printf("1\n");

    //将文件名和用户当前目录拼接起来
    
    char filename[256] = {0};
    sprintf(filename, "%s" , temp_filename);
    


    //打开文件, 向客户端发送错误信息   2
    int fd = open(filename, O_RDONLY);

    //打开后对文件上锁
    if(flock(fd, LOCK_EX) == -1)
    {
        return -1;
    } 

/*
    train_t train_error_msg;
    train_error_msg.size = sizeof(int);
    if (fd == -1) {
        int error = 1;
        memcpy(train_error_msg.buf, &error, sizeof(int));
        sendn(sockfd, &train_error_msg.size, sizeof(train_error_msg.size));
        sendn(sockfd, train_error_msg.buf, train_error_msg.size);

        return -1;
    }
    else {
        int error = 0;
        memcpy(train_error_msg.buf, &error, sizeof(int));
       
    }
    sendn(sockfd, &train_error_msg.size, sizeof(train_error_msg.size));
    sendn(sockfd, train_error_msg.buf, train_error_msg.size);
    printf("2\n");

 */   
    
    
    // 获取文件大小，用于断点续传和决定是否使用mmap
    off_t fileSize = lseek(fd, 0, SEEK_END);
    if (fileSize == -1) {
        perror("Seek Error");
        close(fd);
        return -1;
    }
    
    // 将当前的文件大小发送给客户端，以支持断点续传  3
    printf("file size = %ld\n", fileSize);
    int ret = sendn(sockfd, &fileSize, sizeof(fileSize));
    if(ret != 0)
    {
        printf("send file size failed\n");
    }
    
    off_t offset = 0;
    // 接收客户端发回的当前偏移量  4  握手完毕
    recvn(sockfd, &offset, sizeof(offset));
    printf("file offset = %ld\n", offset);
    
    lseek(fd, offset, SEEK_SET); // 跳转到断点位置
    
    int recv_size = sendfile(sockfd, fd, &offset, fileSize - offset);
    printf("recv size = %d\n", recv_size);
    
    
    
    close(fd);
    flock(fd, LOCK_UN);
    return 0; // 成功完成传输
}


//int server_send(MYSQL *conn, dirStackType *dirStk, const char *file_name, int sockfd)
//{
//    int file_pre_id = 0;
//    int ret = getHead(dirStk, &file_pre_id);
//    int error_num = 0;
//
//    if(ret == 1)
//    {
//        file_pre_id = -1;
//    }
//
//    int file_ids[1024];
//    int nums = findFilesByPreId(conn, file_pre_id, file_ids);
//
//    if(nums == 0)
//    {
//        printf("file does not exist in client download!\n");
//        error_num = 1; //数据库里找不到，文件不存在！
//    }
//
//    File file_data;
//
//    for(int i = 0; i < nums; i++)
//    {
//        bzero(&file_data, sizeof(file_data));
//        getFileDataById(conn, file_ids[i], &file_data);
//        if(strcmp(file_name, file_data.filename))
//        {
//            break;
//        }
//    }
//
//    if(file_data.tomb == 1)
//    {
//        error_num = 1; //墓碑值为-1，文件不存在！
//    }
//
//    
//
//
//    //握手:用小火车向客户端发送错误信息  1
//    train_t train_error_msg;
//    train_error_msg.size = sizeof(int);
//    if (error_num == -1) {
//        int error = 1;
//        memcpy(train_error_msg.buf, &error, sizeof(int));
//        sendn(sockfd, &train_error_msg.size, sizeof(train_error_msg.size));
//        sendn(sockfd, train_error_msg.buf, train_error_msg.size);
//
//        return -1;
//    }
//    else {
//        int error = 0;
//        memcpy(train_error_msg.buf, &error, sizeof(int));
//       
//    }
//    sendn(sockfd, &train_error_msg.size, sizeof(train_error_msg.size));
//    sendn(sockfd, train_error_msg.buf, train_error_msg.size);
//
//    char file_sha1[41];
//    bzero(file_sha1, sizeof(file_sha1));
//    strcpy(file_sha1, file_data.sha1);
//    
//    ret = sendFile(sockfd, file_sha1);
//
//    if(ret != 0)
//    {
//        printf("send file failed!\n");
//        return -1;
//    }
//
//    return 0;
//}

//下载：kehudaua
//从用户目录栈里得到当前顶部的dirID 把他作为父亲id查询文件名是否存在
//得到文件的条目后，查询它的墓碑值
//如果存在，获取它的哈希值1
//获取哈希值1后，把哈希值1传给客户端
//客户端在本地查文件存不存在，把偏移量传过来
//服务端传输文件，传输完客户端再哈希一下文件，得到哈希值2，比对哈希值1和哈希值2是否一样，把结果传给服务端
//如果不一样，客户端就把文件删了，重新下载


int client_download(int sockfd, const char *file_name)
{
    train_t train_error_msg;
    printf("fbt_get.c 400 here1\n");
    recvn(sockfd, &train_error_msg.size, sizeof(train_error_msg.size));
    printf("fbr_get.c 402 here2\n");
    recvn(sockfd, train_error_msg.buf, train_error_msg.size);
    printf("fbr_get.c 404 here3\n");
    int error_msg = 0;
    memcpy(&error_msg, train_error_msg.buf, train_error_msg.size);
    printf("fbr_get.c 407 here4\n");
    printf("2\n");
    if(error_msg == 1)
    {
        return -1;
    }
    
    printf("file name :%s\n", file_name);
    recvFile(sockfd, file_name);
}

