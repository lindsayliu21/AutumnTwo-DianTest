OBJS = main.o hello.o
CC = gcc
CFLAGS = -Wall -O2
app:$(OBJS)
	$(CC) $(CFLAGS) -o app $(OBJS)
main.o:main.c
	$(CC) $(CFLAGS) -c main.c
hello.o:hello.c
	$(CC) $(CFLAGS) -c hello.c
