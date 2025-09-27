#变量定义
OBJS = main.o hello.o
CC = gcc
CFLAGS = -Wall

#1、最终目标：依赖OBJS（中间目标）
app:$(OBJS)
	$(CC) $(CFLAGS) -o app $(OBJS)
#中间目标1：依赖源文件main.c
main.o:main.c
	$(CC) $(CFLAGS) -c main.c
#中间目标2：依赖源文件hello.c
hello.o:hello.c
	$(CC) $(CFLAGS) -c hello.c













