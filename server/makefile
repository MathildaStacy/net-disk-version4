SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))
CC:=gcc
server:$(OBJS)
	$(CC) $^ -o $@ -lpthread -lmysqlclient -lcrypt -lcrypto -lssl -ll8w8jwt
%.o:%.c
	$(CC) -c $^ -o $@ -g
clean:
	$(RM) $(OBJS) server
rebuild:clean all

