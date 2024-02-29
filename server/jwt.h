#ifndef __JWT_H__
#define __JWT_H__

#include <l8w8jwt/encode.h>
#include <l8w8jwt/decode.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

char *createToken(const char *username);
bool verifyToken(const char *username, const char *jwt);
int cpToken(const char *username , char *token);
#endif
