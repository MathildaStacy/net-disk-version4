#ifndef __FILE_CONTENT_TO_SHA_H__
#define __FILE_CONTENT_TO_SHA_H__


#include <stdio.h>
#include <openssl/evp.h>
#include <unistd.h> // for read()

#define SHA1_STR_LEN 41

int file_content_to_sha1(int file_fd, char *sha1, size_t n);

#endif
