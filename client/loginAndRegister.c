
#include "loginAndRegister.h"

#define BUF_SIZE 4096
#define PORT 9190

//注册
int userRegister(char *username, int netfd) {
    File_Data_t fileData;
    bzero(&fileData, sizeof(fileData));
    char sendBuf[1004] = {0};
    char buf[1000] = {0};
    int ret;

    while (1) {
        printf("Do you wang to register?\n");
        printf("(Please enter y or n):");
        fflush(stdout);
        bzero(buf, sizeof(buf));
        read(STDIN_FILENO, buf, sizeof(buf));

        if (buf[0] == 'y' && buf[1] == '\n') {

            while (1) {
                printf("----------UserRegister----------\n");
                printf("Username:");
                fflush(stdout);
                bzero(buf, sizeof(buf));
                read(STDIN_FILENO, buf, sizeof(buf));
                
                bzero(username, 40);
                strcpy(username, buf);

                bzero(&fileData, sizeof(fileData));
                strcpy(fileData.dataBuf, "signY");
                SEND_FILEDATA;

                // 发送用户名
                bzero(&fileData, sizeof(fileData));
                strcpy(fileData.dataBuf, username);
                SEND_FILEDATA;

                // 得到用户名是否存在的情况
                GET_FILEDATA;
                if (strcmp(fileData.dataBuf, "error") == 0) {
                    printf("signIn error: username exist!\n");
                    continue;
                }

                // 输入密码
                while (1) {
                    fflush(stdout);
                    bzero(buf, sizeof(buf));
                    strcpy(buf, getpass("Password:"));

                    // 发送密码
                    bzero(&fileData, sizeof(fileData));
                    strcpy(fileData.dataBuf, buf);
                    SEND_FILEDATA;

                    GET_FILEDATA;
                    if (strcmp(fileData.dataBuf, "error") == 0) {
                        printf("Register failed\n");
                        break;
                    } else {
                        printf("Register success!\n");
                        getchar();
                        break;
                    }
                }
                break;
            }
        } else {
            bzero(&fileData, sizeof(fileData));
            strcpy(fileData.dataBuf, "signN");
            SEND_FILEDATA;
            break;
        }
    }
    return 0;
}

//登陆
int userLogin(char *username, int netfd) {
    fflush(stdout);
    File_Data_t fileData;
    bzero(&fileData, sizeof(fileData));
    char sendBuf[1004] = {0};
    char buf[1000] = {0};
    char password[40] = {0};
    int ret;

    while (1) {
        // 输入用户名
        printf("-----------UserLogin-----------\n");
        printf("Username:");
        fflush(stdout);
        bzero(buf, sizeof(buf));
        read(STDIN_FILENO, buf, sizeof(buf));
        
        bzero(username, 40);
        strncpy(username, buf, strlen(buf) - 1);

        bzero(&fileData, sizeof(fileData));
        strcpy(fileData.dataBuf, username);
        SEND_FILEDATA;

        // 得到用户名是否存在的情况
        fflush(stdout);
        GET_FILEDATA;
        if (strcmp(fileData.dataBuf, "error") == 0) {
            printf("signIn error:username don't exist!\n");
            continue;
        }

        // 输入密码
        while (1) {
            bzero(password, sizeof(password));
            fflush(stdout);
            bzero(buf, sizeof(buf));
            strcpy(buf, getpass("Password:"));
            
            strcpy(password, buf);
            password[strcspn(password, "\n")] = '\0';
            
            // 得到salt
            char salt[21] = {0};
            GET_FILEDATA;
            strncpy(salt, fileData.dataBuf, 20);
            salt[20]= '\0';

            char shadow[255] = {0};
            shadow[0] = '\0';
            strcpy(shadow, crypt(password, salt));
            // 得到shadow
            char shadow_server[255];
            GET_FILEDATA;
            strncpy(shadow_server, fileData.dataBuf, 255);

            if (strcmp(shadow, shadow_server) == 0) {
                bzero(&fileData, sizeof(fileData));
                strcpy(fileData.dataBuf, "success");
                SEND_FILEDATA;
                return 0;

            } else {
                printf("Password error\n");
                bzero(&fileData, sizeof(fileData));
                strcpy(fileData.dataBuf, "error");
                SEND_FILEDATA;
                break;
            }
        }
    }
    return 0;
}
//获取token
int getToken(int sockfd, char *token){
    char buf[512] = {0};
    int ret = recv(sockfd, buf, 512, 0);
    if(ret == -1){
        perror("recv");
        return -1;
    }
    strcpy(token, buf);
    return 0;
}

// int sendCommand(int sockfd, command *cmd){
//     int ret = send(sockfd, cmd, sizeof(command), 0);
//     if(ret == -1){
//         perror("send");
//         return -1;
//     }
//     return 0;
// }

//int main(int argc, char *argv[]) {
//    struct sockaddr_in serv_addr;
//    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
//    if(sockfd == -1){
//        perror("socket");
//        return -1;
//    }
//
//    memset(&serv_addr, 0 , sizeof(serv_addr));
//    serv_addr.sin_family = AF_INET;
//    serv_addr.sin_addr.s_addr = inet_addr("192.168.10.130");
//    serv_addr.sin_port = htons(PORT);
//
//    int ret = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
//    if(ret == -1){
//        perror("connect");
//        return -1;
//    }
//    
//    printf("------------NetDisk------------\n");
//    char username[40] = {0};
//    char token[512] = {0};
//    userRegister(username, sockfd);
//    userLogin(username, sockfd);
//    getToken(sockfd, token);
//    //printf("Login success, username = %s\ntoken = %s\n", username, token);
//
//    command cmd;
//    strcpy(cmd.username, username);
//    strcpy(cmd.token, token);
//
//    //sendCommand(sockfd, &cmd);
//    ret = send(sockfd, &cmd, sizeof(command), 0);
//    if(ret == -1){
//        perror("send");
//        return -1;
//    }
//    int flag;
//    ret = recv(sockfd, &flag, sizeof(flag), 0);
//    if(ret == -1){
//        close(sockfd);
//        return -1;
//    }
//    printf("flag = %d\n", flag);
//        
//    close(sockfd);
//    return 0;
//}
