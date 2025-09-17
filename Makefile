# 声明目标文件：最终生成的可执行文件名为main
#规则1：生成可执行文件main
main main.o func.o
	gcc main.o func.o -o main

#规则2：编译main.c生成main.o
main.o:main.c
    gcc -c main.c

#规则三：
tool1 tool2:common.c tool1.c tool2.c
	gcc common.c tool1.c -o tool1
	gcc common.c tool2.c -o tool2


#规则4：清理所有生成文件(这个需要单独检验)

	rm -f main main.o func.o tool1 tool2
clean: