#include <stdio.h>
#include "hello.h"

int main() {
    printf("主程序开始运行...\n");
    print_hello();

    
    printf("计算结果 : %d\n", add(2, 3));
    printf("主程序运行结束。\n");

    return 0;
}