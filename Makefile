# 目标: app，依赖: main.c utils.c
app: main.c utils.c
	gcc -o app main.c utils.c  # 注意: 行首必须是Tab字符，不是空格