hello.o:hello.c
	gcc -c hello.c
main.o: main.c  program.o
	gcc -c main.c -o main.o
program.o:main.o hello.o
	gcc -o program main.o hello.o
app: program.o
	gcc -o app program.o
