#include<stdio.h>
#include<string.h>

void print_help(){
printf("usage: git [-v | --version] [-h | --help] [-C <path>] [-c <name>=<value>]\n"
           "           [--exec-path[=<path>]] [--html-path] [--man-path] [--info-path]\n"
           "           [-p | --paginate | -P | --no-pager] [--no-replace-objects] [--bare]\n"
           "           [--git-dir=<path>] [--work-tree=<path>] [--namespace=<name>]\n"
           "           [--config-env=<name>=<envvar>] <command> [<args>]");
}

int main(int argc,char *argv[]){
//处理无参数情况
if(argc==1){
printf("Error:No argument provided!\n");
printf("Try 'program --help' for more information.\n");
return 1;
}
//命令行参数解析
for(int i=1;i<argc;i++){
if(strcmp(argv[i],"--help")==0){
    print_help();
}
else {
// 处理无效参数
printf("Error: Invalid argument '%s'\n", argv[i]);
printf("Try 'program --help' for more information.\n");
return 1;
}
}
}