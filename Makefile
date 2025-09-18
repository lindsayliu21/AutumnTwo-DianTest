# test1.mk 内容
app: main.c
	gcc -o app main.c

# 重复定义app目标
app: utils.c
	gcc -o app utils.c

app1: main.c missing.c  # missing.c 不存在于当前目录
	gcc -o app1 main.c missing.c

app2: main.c lib  # lib 未定义为目标
	gcc -o app2 main.c lib