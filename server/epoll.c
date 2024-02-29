#include "threadPool.h"
int epollAdd(int epfd, int fd){
    struct epoll_event events;
    events.events = EPOLLIN;
    events.data.fd = fd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&events);
    return 0;
}
int epollDel(int epfd, int fd){
    epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL);
    return 0;
}

