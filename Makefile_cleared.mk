app: main.c
	gcc -o app main.c
app: utils.c
	gcc -o app utils.c
app1: main.c missing.c
	gcc -o app1 main.c missing.c
app2: main.c lib
	gcc -o app2 main.c lib
