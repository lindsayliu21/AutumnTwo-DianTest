hello.o:hello.c
	gcc -c hello.c
main.o: main.c
	gcc -c main.c -o main.o
program:main.o hello.o
	gcc main.o hello.o -o program
