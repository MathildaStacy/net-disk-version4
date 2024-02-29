#include "loginAndRegister.h"
#include "jwt.h"
#include "logger.h"
#include "train.h"
#include "fbr_gets_and_puts.h"
#include "trans_all.h"
//int recvn(int sockfd, void* buf, long size)
//{
//    char* p = (char*)buf;
//    int curSize = 0;
//    while (curSize < size) {
//    int getrecv = recv(sockfd, p + curSize, size - curSize, MSG_WAITALL);
//    if (getrecv == 0) {
//    return curSize;
//    }
//    curSize += getrecv;
//    }
//
//    return curSize;;
//}
//
//int sendn(int sockfd, const void* buf, long size)
//{
//    const char* p = (const char*)buf;
//    int curSize = 0;
//    while (curSize < size) {
//    int sent = send(sockfd, p + curSize, size - curSize, 0);
//    if (sent < 0) {
//        perror("send error");
//        return -1;
//    }
//    curSize += sent;
//    }
//
//    return 0;
//}

//生成盐值
void generateSalt(char *salt){
    salt[0] = '$';
    salt[1] = '6';
    salt[2] = '$';
    salt[19] = '$';
    salt[20] = 0;
    srand(time(NULL));
    int flag;
    for(int i = 3;i < 19;i++){
        flag = rand() % 3;
        switch(flag){
        case 0:
            salt[i] = rand() % 26 + 'a';
            break;
        case 1:
            salt[i] = rand() % 26 + 'A';
            break;
        case 2:
            salt[i] = rand() % 10 + '0';
            break;
        }
    }
}

//注册处理
int userRegisterDeal(int netfd, MYSQL *conn) {
    char buf[4096] = {0};
    // char sendBuf[1004] = {0};
    char username[64] = {0};
    char password[32] = {0};

    // 得到用户名
    bzero(username, sizeof(username));
    recv_train(buf,netfd);
    // strcpy(username, fileData.dataBuf, strlen(fileData.dataBuf));
    printf("username = %s\n", username);

    // 查询用户名是否存在
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[2000] = {0};
    sprintf(query, "select * from users where username = '%s';", username);

    if (mysql_query(conn, query)) {
        printf("signIn error:%s\n", mysql_error(conn));
        // SEND_ERROR;
        bzero(buf, sizeof(buf));
        strcpy(buf, "errorrr");
        send_train( buf, 8,netfd);
        mysql_free_result(res);
    } else {
        res = mysql_use_result(conn);
        row = mysql_fetch_row(res);
        if (row) {
            printf("signIn error:username exist!\n");
            bzero(buf, sizeof(buf));
            strcpy(buf, "errorrr");
            send_train( buf, 8,netfd);
            mysql_free_result(res);
        } else {
            printf("signIn success:username don't exist!\n");
            bzero(buf, sizeof(buf));
            strcpy(buf, "success");
            send_train( buf, 8,netfd);
            mysql_free_result(res);
        }
    }

    char salt[21] = {0};
    generateSalt(salt);
    
    // 收取密码
    bzero(buf, sizeof(buf));
    recv_train(buf,netfd);
    strcpy(password, buf);

    char shadow[255] = {0};
    strcpy(shadow, crypt(password, salt));      

    // 插入数据库
    bzero(query, sizeof(query));
    sprintf(query, "insert into users (username, salt, encrypted_password, tomb) values ('%s', '%s', '%s', %d);", username, salt, shadow, 0);

    if (mysql_query(conn, query)) {
        printf("signIn error:%s\n", mysql_error(conn));
        // SEND_ERROR;
        bzero(buf, sizeof(buf));
        strcpy(buf, "errorrr");
        send_train( buf, 8,netfd);
    } else {
        printf("signIn success\n");
        // SEND_SUCCESS;
        bzero(buf, sizeof(buf));
        strcpy(buf, "success");
        send_train( buf, 8,netfd);
    }
    return 0;
}

//登陆处理
int userLoginDeal(int netfd, char *username, MYSQL *conn) {
    char buf[4096] = {0};

    // 接收用户名
    // GET_FILEDATA;
    recv_train(buf,netfd);
    bzero(username, 64);//2r
    strncpy(username, buf, 64);

    // 查询用户名是否存在
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[2000] = {0};
    bzero(query, sizeof(query));
    
    sprintf(query, "select * from users where username = '%s';", username);
    if (mysql_query(conn, query)) {
        printf("LogIn error:%s\n", mysql_error(conn));
        // SEND_ERROR;
        bzero(buf, sizeof(buf));
        strcpy(buf, "errorrr");
        send_train( buf, 8,netfd);//3s
        mysql_free_result(res);
        return 1;
    } else {
        res = mysql_use_result(conn);
        row = mysql_fetch_row(res);
        if (row) {
            
            printf("LogIn success:username exist!\n");

            bzero(buf, sizeof(buf));
            strcpy(buf, "success");
            send_train( buf, 8,netfd);//3s

            //传回salt
            // bzero(&fileData, sizeof(fileData));
            // strcpy(fileData.dataBuf, row[2]);
            // SEND_FILEDATA;
            bzero(buf, sizeof(buf));
            strcpy(buf, row[2]);
            send_train( buf, 21,netfd);//4s  发yz

            //传回shadow
            // bzero(&fileData, sizeof(fileData));
            bzero(buf, sizeof(buf));
            strcpy(buf, row[3]);
            // SEND_FILEDATA;
            send_train( buf, 255,netfd);//5s发秘闻
            mysql_free_result(res);
        } else {
            // SEND_ERROR;
            bzero(buf, sizeof(buf));
            strcpy(buf, "errorrr");
            send_train( buf, 8,netfd);
            mysql_free_result(res);
            return 1;
        }
    }

    // GET_FILEDATA;
    bzero(buf, sizeof(buf));
    recv_train(buf,netfd);//
    if (strcmp(buf, "errorrr") == 0) {
        printf("LogIn error: Password error!\n");
    }
    return 0;
}


// 生成Token
char *createToken(const char *username) {
    char *jwt;
    size_t jwt_length;

    struct l8w8jwt_encoding_params params;
    l8w8jwt_encoding_params_init(&params);

    params.alg = L8W8JWT_ALG_HS512;

    params.sub = "Netdisk";
    params.iss = "Netdisk";
    params.aud = "Administrator";

    params.iat = 0;
    params.exp = 0x7fffffff; // 永久生效

    params.secret_key = (unsigned char *)username;
    params.secret_key_length = strlen(username);

    params.out = &jwt;
    params.out_length = &jwt_length;

    int r = l8w8jwt_encode(&params);

    if (r != L8W8JWT_SUCCESS) {
        fprintf(stderr, "Failed to encode JWT\n");
        return NULL;
    }

    char *jwt_copy = (char *)malloc(jwt_length + 1);
    if (jwt_copy == NULL) {
        fprintf(stderr, "Failed to allocate memory for jwt_copy\n");
        l8w8jwt_free(jwt);
        return NULL;
    }

    strcpy(jwt_copy, jwt);
    l8w8jwt_free(jwt);
    return jwt_copy;
}

//验证token
bool verifyToken(const char *username, const char *jwt){
    struct l8w8jwt_decoding_params params;
    l8w8jwt_decoding_params_init(&params);

    params.alg = L8W8JWT_ALG_HS512;
    params.jwt = (char*)jwt;
    params.jwt_length = strlen(params.jwt);

    params.verification_key = (unsigned char*)username;
    params.verification_key_length = strlen((char*)params.verification_key);

    params.validate_iss = "Netdisk"; 
    params.validate_sub = "Netdisk";

    params.validate_exp = 0x7fffffff;
    params.exp_tolerance_seconds = 0;

    params.validate_iat = 0;
    params.iat_tolerance_seconds = 0;

    enum l8w8jwt_validation_result validation_result;

    int decode_result = l8w8jwt_decode(&params, &validation_result, NULL, NULL);

    if (decode_result == L8W8JWT_SUCCESS && validation_result == L8W8JWT_VALID) {
        printf("Token validation successful!\n");
        return true;
    }
    else{
        printf("Token validation failed!\n");
        return false;
    }
}

//获取token
int cpToken(const char *username , char *token) {
    char *jwt = createToken(username);
    if (jwt != NULL) {
        strcpy(token, jwt);
        free(jwt); // 释放分配的内存
    }
    return 0;
}
//获取结构体，验证token，校验成功返回0，失败返回-1
int getCommand(int netfd, order_t *cmd){
    LOG_DEBUG("here");
    //int ret = recv(netfd, cmd, sizeof(order_t), 0);
    LOG_DEBUG("here");
    //验证成功
    if(verifyToken(cmd->username, cmd->token)){
        return 0;
    }else{
        return -1;
    }
}


int NetDiskInterface(int netfd, char *username, MYSQL *conn){
    
    //从客户端接收消息
    char buf[4096] = {0};
    // memset(&fileData, 0, sizeof(fileData));
    // int ret;
    char command[10] = {0};
    // GET_FILEDATA
    recv_train(buf,netfd);//1r
    printf("login 318 buf = %s\n", buf);
    strcpy(command, buf);
    char token[512] = {0};
    if (strcmp(command, "Register") == 0) {
        userRegisterDeal(netfd, conn);
    }else if (strcmp(command, "Loginnnn") == 0){
        int ret = userLoginDeal(netfd, username, conn);
        if(ret == 0){
            cpToken(username, token);
            printf("token = %s\n", token);
            send(netfd, token, sizeof(token), 0);
        }
    }else{
        printf("Invail command!\n");
    }
    return 0;  
}




// int main(void) {
//    struct sockaddr_in serv_addr;
//    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
//    if(sockfd == -1){
//        perror("socket");
//        return -1;
//    }

//    memset(&serv_addr, 0 , sizeof(serv_addr));
//    serv_addr.sin_family = AF_INET;
//    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//    serv_addr.sin_port = htons(PORT);

//    int reuse = 1;
//    int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
//    if(ret == -1){
//        perror("setsockopt");
//        return -1;
//    }

//    bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
//    listen(sockfd, 50);

//    MYSQL *conn = mysql_init(NULL);
//    if (conn == NULL) {
//        fprintf(stderr, "mysql_init() failed\n");
//        return -1;
//    }

//    if (mysql_real_connect(conn, "localhost", "root", "010123", "netdisk", 0, NULL, 0) == NULL) {
//        fprintf(stderr, "mysql_real_connect() failed: %s\n", mysql_error(conn));
//        mysql_close(conn);
//        return -1;
//    }
//    printf("Mysql connect successfully!\n");

// //    while(1){
// //        int netfd = accept(sockfd, NULL, NULL);

// //        char username[40] = {0};
// //        NetDiskInterface(netfd, conn, username);
// //        //登陆成功
// //        printf("Login success, username = %s\n", username);
// //        close(netfd);
// //    }
//     int netfd = accept(sockfd, NULL, NULL);
//     // char username[64] = {0};
//     NetDiskInterface(netfd, conn);
//     // sleep(10);
//     mysql_close(conn);
//     close(netfd);
//     close(sockfd);
//    return 0;
// }

