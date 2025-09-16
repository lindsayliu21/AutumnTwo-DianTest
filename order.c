#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#define MAX_LINE 1024
//--help命令输出(1.1)
void print_help();

//预处理与文件读取（1.2）
void process_makefile(int verbose);
//去除尾部空格
void trim_trailing_spaces(char *line);
//去除注释(#后的内容)包括去除注释后的空行和行尾空格
void remove_comments(char *line);
//过滤空行
int is_blank_line(const char *line);
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
//进入调试模式
if(verbose==1){
outfile=fopen("./Makefile_cleared.mk","w");
if(outfile==NULL){
perror("无法创建输出文件Minimake_claered.mk");
fclose(infile);
return;
}
}
// 逐行读取并处理
printf("正在处理Makefile...\n");
char line[MAX_LINE];
while(fgets(line,sizeof(line),infile)!=NULL){
// 去除注释
remove_comments(line);
// 去除行尾空格
trim_trailing_spaces(line);
// 过滤空行
if (is_blank_line(line)) {
continue;
}
// 输出清理后的行
if (verbose) {
fputs(line, outfile);
fputs("\n", outfile);  // 重新添加换行符
} 
else {
printf("%s\n", line);  // 非调试模式下打印到控制台
}    
}
// 检查读取错误
if (ferror(infile)) {
perror("读取文件时发生错误");
}
fclose(infile);
if (verbose && outfile != NULL) {
fclose(outfile);
printf("处理完成，清理后的内容已保存到Minimake_claered.mk\n");
} 
else if (!verbose) {
printf("处理完成\n");
}    
}

int is_blank_line(const char *line){
if(line==NULL||*line=='\0'){
    return 1;
}
while(*line!='\0'){
if(!isspace((unsigned char )*line)){
return 0;
}
line++;
}
return 1;
}

void trim_trailing_spaces(char *line){
if(line==NULL||*line=='\0') return;
int len=strlen(line);
while(len>0&&isspace((unsigned char)line[len-1])){
    len--;
}
line[len]='\0';
}
void remove_comments(char *line){
if(line==NULL||*line=='\0') return;
char *firstcomment=strchr(line,'#');
if(firstcomment!=NULL){
*firstcomment='\0';// 在#处截断字符串
}
}