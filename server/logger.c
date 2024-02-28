#include "logger.h"

int g_log_level;
FILE* g_log_file = NULL;
pthread_mutex_t log_mutex;

void init_log_file(const char* filepath) {
    pthread_mutex_init(&log_mutex, NULL);
    if (filepath != NULL) {
        g_log_file = fopen(filepath, "a"); // 以追加模式打开文件
        if (g_log_file == NULL) {
            // 处理错误，例如输出到stderr
            fprintf(stderr, "Unable to open log file: %s\n", filepath);
        }
    }
}

void get_current_time_str(char* buffer, int buffer_size) {
    time_t now = time(0);
    struct tm tstruct;
    localtime_r(&now, &tstruct);
    strftime(buffer, buffer_size, "%Y-%m-%d %X", &tstruct);
}

void log_message(int level, const char* file, int line, const char* func, const char* message, ...) {
    if (level < g_log_level) return;

    pthread_mutex_lock(&log_mutex);

    char time_buffer[20];
    get_current_time_str(time_buffer, sizeof(time_buffer));
    unsigned long thread_id = (unsigned long)pthread_self();

    FILE* output = (g_log_file != NULL) ? g_log_file : stdout;
    fprintf(output, "[%s][thread id:%lx] %s - %s:%d - %s(): ", time_buffer, thread_id, (level==LOG_LEVEL_DEBUG)?"DEBUG":(level==LOG_LEVEL_INFO)?"INFO":(level==LOG_LEVEL_WARN)?"WARN":"ERROR", file, line, func);
    
    va_list args;
    va_start(args, message);
    vfprintf(output, message, args);
    va_end(args);

    fprintf(output, "\n");

    pthread_mutex_unlock(&log_mutex);
}

void close_log_file() {
    if (g_log_file != NULL) {
        fclose(g_log_file);
        g_log_file = NULL;
    }
    pthread_mutex_destroy(&log_mutex);
}