#ifndef __LOGGER_H
#define __LOGGER_H

#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <stdarg.h>

#define LOG_LEVEL_DEBUG 0  //日志级别，如果设置级别为0则所有级别大于等于0的级别的日志都可以打印
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_ERROR 3

extern int g_log_level;
extern FILE* g_log_file;
extern pthread_mutex_t log_mutex; //保证如果往文件输出的话是线程安全的

void init_log_file(const char* filepath);

void get_current_time_str(char* buffer, int buffer_size);

void log_message(int level, const char* file, int line, const char* func, const char* message, ...);
void close_log_file();

#define LOG_DEBUG(message, ...) log_message(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __FUNCTION__, message, ##__VA_ARGS__)
#define LOG_INFO(message, ...)  log_message(LOG_LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, message, ##__VA_ARGS__)
#define LOG_WARN(message, ...)  log_message(LOG_LEVEL_WARN, __FILE__, __LINE__, __FUNCTION__, message, ##__VA_ARGS__)
#define LOG_ERROR(message, ...) log_message(LOG_LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, message, ##__VA_ARGS__)

#endif