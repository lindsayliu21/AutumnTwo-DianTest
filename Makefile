# test1.mk 内容
app: main.c
	gcc -o app main.c

# 重复定义app目标
app: utils.c
	gcc -o app utils.c