OBJS1 = hello.o
OBJS = main.o $(OBJS1)
CC = gcc
CFLAGS = -Wall -O2
app:$(OBJS)
	$(CC) $(CFLAGS) -o app $(OBJS)
main.o:main.c
	$(CC) $(CFLAGS) -c main.c
${OBJS1}:hello.c
	$(CC) $(CFLAGS) -c hello.c
