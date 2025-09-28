#include"makefile_simple.h"

void print_help(){
printf("usage: git [-v | --version] [-h | --help] [-C <path>] [-c <name>=<value>]\n"
           "           [--exec-path[=<path>]] [--html-path] [--man-path] [--info-path]\n"
           "           [-p | --paginate | -P | --no-pager] [--no-replace-objects] [--bare]\n"
           "           [--git-dir=<path>] [--work-tree=<path>] [--namespace=<name>]\n"
           "           [--config-env=<name>=<envvar>] <command> [<args>]\n");
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

void print_error(int line_num,ErrorType error){
printf("line%d: ",line_num);
switch(error){
case MISS_COLON:
    printf("Missing colon in target definition!\n");
    break;
case MISS_TAB:
    printf("Command found start without a tab!\n");
    break;
case COMMAND_BEFORE_RULE:
    printf("Command found before rule\n");
    break;
default:
    break;
}
}

int is_target_or_command_line(const char *line){
const char *ptr=line;
if(*ptr){
if(isspace((unsigned char)*ptr)) return 0;//开头有空白字符，返回0，表示为规则行
else return 1;//开头没有空白字符，返回1，表示为目标行
}

}

ErrorType check_line_error(const char *line, int rule_flag){
int is_t_or_c=is_target_or_command_line(line);
//检查目标行是否缺少冒号：
if(is_t_or_c){
const char *ptr=line;
if(strchr(ptr,':')==NULL){
if(strchr(ptr,'=')) return NO_ERROR;
  return MISS_COLON;
}
}
//检查命令行是否缺少Tab
else if(!is_t_or_c){
const char *ptr=line;
if(*ptr!='\t') return MISS_TAB;

// 检查命令是否在规则之前出现
if (rule_flag%2==0) return COMMAND_BEFORE_RULE;
}        
return NO_ERROR;

}

void rule_checker(){
FILE *rulefile=fopen("./Makefile_cleared.mk","r");
if(rulefile==NULL){
perror("Error opening Makefile_cleared.mk!!");
return;
}
char line[MAX_LINE];
int line_num = 0;
int rule_flag = 0; // 标记是否已遇到规则定义
//逐行读取并处理
printf("正在对Makefile_cleared.mk文件进行静态语法处理...\n");
while(fgets(line,sizeof(line),rulefile)){
 line_num++;
 ErrorType error=check_line_error(line,rule_flag);
 if(error!=NO_ERROR){
print_error(line_num,error);
 }
 if(is_target_or_command_line(line)) rule_flag=1;
 else rule_flag+=2;
}
fclose(rulefile);
return ;
}