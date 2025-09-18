#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include <sys/stat.h>

#define MAX_LINE_LEN 1024 // 文件名最长32个字符+1个结束符
#define MAX_FILE_NAME_LEN 33 // 最大规则数量
#define MAX_RULES 30 // 每个目标最大依赖数量
#define MAX_DEPEND_NUM 30 // 每个目标最大依赖数量
#define MAX_COMMAND_NUM 30 // 每个目标最大命令数量

// 命令结构体
typedef struct {
char cmd[MAX_LINE_LEN];  // 存储命令内容
} Command;

// 规则结构体
typedef struct {
    char target[MAX_FILE_NAME_LEN];       // 目标名称
    char dependencies[MAX_DEPEND_NUM][MAX_FILE_NAME_LEN];  // 依赖列表
    int dep_count;                        // 依赖数量
    Command commands[MAX_COMMAND_NUM];    // 命令列表
    int cmd_count;                        // 命令数量
    int line_num;                         // 定义所在行号
} Rule;

// 解析器状态结构体
typedef struct {
    Rule rules[MAX_RULES];  // 规则数组
    int rule_count;         // 已解析的规则数量
} ParserState;

//初始化解析器状态
void init_parser(ParserState *state);
//检查目标是否定义
int is_target_defined(ParserState *state, const char *target);
//检查文件是否存在
int is_file_exists(const char *filename);
//// 解析Makefile并检查规则
void parse_makefile(const char *filename);
//检查依赖是否有效
void check_dependencies(ParserState *state);

int main(int argc,char *argv[]){
if (argc != 2) {
    printf("用法: %s <makefile>\n", argv[0]);
    return 1;
    }
parse_makefile(argv[1]);
    return 0;
}


void init_parser(ParserState *state) {
    state->rule_count = 0;
    memset(state->rules, 0, sizeof(state->rules));
}

int is_target_defined(ParserState *state, const char *target){
for(int i=0;i<state->rule_count;i++){
    if(strcmp(state->rules[i].target,target)==0)
    return i;//返回已定义的规则索引
}
return -1;//目标未定义
}

int is_file_exists(const char *filename){
    struct stat statbuf;
    return(stat(filename,&statbuf)==0);//返回1表示文件存在
}

void check_dependencies(ParserState *state){
    for(int i=0;i<state->rule_count;i++){
    Rule *rule=&state->rules[i];
    for(int j=0;j<rule->dep_count;j++){
    const char *dep=rule->dependencies[j];
    int is_valid=0;

    if(is_target_defined(state,dep)!=-1) is_valid=1;
    else if(is_file_exists(dep)==1) is_valid=1;
    // 如果依赖无效，输出错误
    if (!is_valid) {
    printf("Line%d: Invalid dependency '%s' \n", rule->line_num, dep);
    }

    }
}
}

void parse_makefile(const char *filename){
FILE * file=fopen(filename,"r");
if(!file){
    perror("failed to open file!!\n");
}
ParserState state;
init_parser(&state);
    
char line[MAX_LINE_LEN];
int line_num = 0;
Rule *current_rule = NULL;
while(fgets(line,sizeof(line),file)){
    line_num++;
    line[strcspn(line,"\n\r")]='\0';
// 检查是否为目标行（不以Tab开头）
if (line[0] != '\t') {
// 查找冒号分隔符
char *colon = strchr(line, ':');
if (colon) {
// 分割目标和依赖
*colon = '\0';
char *target_str = line;
char *deps_str = colon + 1;
// 检查目标是否为空
if (strlen(target_str) == 0) {
printf("Line%d: Invalid target defined!!\n", line_num);

}
// 检查目标是否已定义
int existing_idx = is_target_defined(&state, target_str);
if (existing_idx != -1) {
printf("Line%d: Duplicate target definition '%s'.  ",line_num, target_str);
printf(" The previous definition is right here: Line%d\n", state.rules[existing_idx].line_num);

}
current_rule = &state.rules[state.rule_count++];
strncpy(current_rule->target, target_str, MAX_FILE_NAME_LEN - 1);
current_rule->target[MAX_FILE_NAME_LEN - 1] = '\0';
current_rule->dep_count = 0;
current_rule->cmd_count = 0;
current_rule->line_num = line_num;

//解析依赖
char *dep=strtok(deps_str," ");
while(dep){
if(strlen(dep)>0){
   strncpy(current_rule->dependencies[current_rule->dep_count],dep,MAX_FILE_NAME_LEN-1);
   current_rule->dependencies[current_rule->dep_count][MAX_FILE_NAME_LEN-1]='\0';
   current_rule->dep_count++;
}
dep = strtok(NULL, " ");
}

}
}
//有Tab,则为命令行
else{
//存储命令，去除开头的Tab
char *cmd=line+1;
if(strlen(cmd)>0){
strncpy(current_rule->commands[current_rule->cmd_count].cmd,cmd,MAX_LINE_LEN-1);
current_rule->commands[current_rule->cmd_count].cmd[MAX_LINE_LEN-1]='\0';
current_rule->cmd_count++;
}
}

}
fclose(file);
// 检查所有依赖的有效性
check_dependencies(&state);
}
