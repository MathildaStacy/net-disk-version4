#include "file_content_to_sha1.h"
#include <fcntl.h>
#include <stdio.h>

// 将字节数据转化为十六进制字符串
void bytes_to_hex_string(const unsigned char *in, size_t len, char *out, size_t out_len) {
    if (out_len < (len * 2 + 1)) return; // 检查输出缓冲区是否足够大
    for (size_t i = 0; i < len; i++) {
        sprintf(out + (i * 2), "%02x", in[i]);
    }
    out[len * 2] = '\0'; // 确保输出的字符串被终结
}

int file_content_to_sha1(int file_fd, char *sha1, size_t n) {
    // 检查提供的空间是否足够容纳SHA-1哈希值的十六进制字符串形式
    if (n < 41) return -1;

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) return -1;

    const EVP_MD *md = EVP_sha1();
    if (!EVP_DigestInit_ex(ctx, md, NULL)) {
        EVP_MD_CTX_free(ctx);
        return -1;
    }

    unsigned char buffer[1024]; // 读取文件的缓冲区
    int read_bytes = 0;

    while ((read_bytes = read(file_fd, buffer, sizeof(buffer))) > 0) {
        if (!EVP_DigestUpdate(ctx, buffer, read_bytes)) {
            EVP_MD_CTX_free(ctx);
            return -1;
        }
    }

    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len;

    if (!EVP_DigestFinal_ex(ctx, md_value, &md_len)) {
        EVP_MD_CTX_free(ctx);
        return -1;
    }

    EVP_MD_CTX_free(ctx);

    // 将二进制的哈希值转换为十六进制字符串
    bytes_to_hex_string(md_value, md_len, sha1, n);

    return 0; // 成功返回
}
/*
int main()
{

    int fd = open("user_dir_stack.c", O_RDONLY);

    char str[41];

    file_content_to_sha1(fd, str, 41);

    printf("sha1: %s\n", str);
    return 0;
}
*/ 
