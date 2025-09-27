#ifndef MAKEFILE_PARSER_H
#define MAKEFILE_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "variable.h"
#include "dependency_graph.h"
#define MAX_LINE_LEN 1024    // 文件名最长32个字符+1个结束符
#define MAX_RULES 30         // 最大规则数量
#define MAX_DEPEND_NUM 30    // 每个目标最大依赖数量
#define MAX_COMMAND_NUM 30   // 每个目标最大命令数量

// 命令结构体
typedef struct
{
    char cmd[MAX_LINE_LEN]; // 存储命令内容
} Command;

// 规则结构体
typedef struct
{
    char target[MAX_FILE_NAME_LEN];                       // 目标名称
    char dependencies[MAX_DEPEND_NUM][MAX_FILE_NAME_LEN]; // 依赖列表
    int dep_count;                                        // 依赖数量
    Command commands[MAX_COMMAND_NUM];                    // 命令列表
    int cmd_count;                                        // 命令数量
    int line_num;                                         // 定义所在行号
} Rule;

// 解析器状态结构体
typedef struct
{
    Rule rules[MAX_RULES]; // 规则数组
    int rule_count;        // 已解析的规则数量
} ParserState;

// 初始化解析器状态
void init_parser(ParserState *state);
// 检查目标是否定义
int is_target_defined(ParserState *state, const char *target);
// 检查文件是否存在
int is_file_exists(const char *filename);
//// 解析Makefile并检查规则
void parse_makefile(const char *filename,ParserState *state,DependencyGraph *graph,VariableTable *variable_table);
// 检查依赖是否有效
void check_dependencies(ParserState *state);

#endif