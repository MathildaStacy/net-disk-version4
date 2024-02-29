SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))
CC:=gcc
client:$(OBJS)
	$(CC) $^ -o $@ -lpthread -lmysqlclient -lcrypt -lcrypto -lssl
%.o:%.c
	$(CC) -c $^ -o $@ -g
clean:
	$(RM) $(OBJS) client
rebuild:clean all

