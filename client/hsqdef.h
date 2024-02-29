#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include<ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <crypt.h>
#include <shadow.h>
#include <mysql/mysql.h>
#define ARGS_CHECK(argc,num) {if(argc!=num){fprintf(stderr,"args error!\n");return -1;}}
#define ERROR_CHECK(ret,num,msg) {if(ret == num){perror(msg); return -1;}}
#define ERROR_PCHECK(ret,num) {if(ret != num){fprintf(stderr,"%s: %s",strerror(ret));return -1;}}
