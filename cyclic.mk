# 包含循环依赖的Makefile示例
# 用于测试循环依赖检测功能

hello.o:hello.c
	gcc -c hello.c 

main.o: main.c  program.o
	
program.o: