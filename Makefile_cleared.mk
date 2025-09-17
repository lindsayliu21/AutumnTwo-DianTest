main main.o func.o
	gcc main.o func.o -o main
main.o:main.c
    gcc -c main.c
tool1 tool2:common.c tool1.c tool2.c
	gcc common.c tool1.c -o tool1
	gcc common.c tool2.c -o tool2
	rm -f main main.o func.o tool1 tool2
clean:
