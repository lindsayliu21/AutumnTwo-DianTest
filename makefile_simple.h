#ifndef MAKEFILE_SIMPLE_H
#define MAKEFILE_SIMPLE_H 

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

//静态语法检查（1.3）
void rule_checker();
//错误类型枚举
typedef enum{
NO_ERROR,
MISS_COLON,
MISS_TAB,
COMMAND_BEFORE_RULE
}ErrorType;
//打印错误信息
void print_error(int line_num,ErrorType error);
// 检查一行为目标行还是命令行(检查开头是否有空格，返回：1：目标行；0：命令行)
int is_target_or_command_line(const char *line);
//检查行错误
ErrorType check_line_error(const char *line, int has_seen_rule);

#endif