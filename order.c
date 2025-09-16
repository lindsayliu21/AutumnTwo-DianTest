#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#define MAX_LINE 1024
//--help命令输出
void print_help();
void process_makefile(int verbose);
int main(int argc,char *argv[]){
int verbose=0;
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
//判断是否进入调试模式（-v/--verbose)
else if((strcmp(argv[i],"-v")==0)||(strcmp(argv[i],"--verbose")==0)){
verbose=1;
break;
}
else {
// 处理无效参数
printf("Error: Invalid argument '%s'\n", argv[i]);
printf("Try 'program --help' for more information.\n");
return 1;
}
}
process_makefile(verbose);
return 0;
}


void print_help(){
printf("usage: git [-v | --version] [-h | --help] [-C <path>] [-c <name>=<value>]\n"
           "           [--exec-path[=<path>]] [--html-path] [--man-path] [--info-path]\n"
           "           [-p | --paginate | -P | --no-pager] [--no-replace-objects] [--bare]\n"
           "           [--git-dir=<path>] [--work-tree=<path>] [--namespace=<name>]\n"
           "           [--config-env=<name>=<envvar>] <command> [<args>]");
}


void process_makefile(int verbose){
FILE *infile,*outfile=NULL;
infile=fopen("./Makefile","r");
if(infile==NULL){
perror("无法打开Makefile文件。\n");
return;
}
char buf[MAX_LINE];
while(fgets(buf,sizeof(buf),infile)!=NULL){
    printf("%s",buf);
}
fclose(infile);
return;
}
