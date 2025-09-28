#ifndef VARIABLE_H
#define VARIABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

//最大变量名长度
#define MAX_VAR_NAME 33
//最大变量值长度
#define MAX_VAR_VALUE 256 
//最大变量个数
#define MAX_VARIABLES 50
//变量展开最大递归深度
#define MAX_EXPANSION_DEPTH 10

// 定义变量作用域类型
typedef enum {
    SCOPE_GLOBAL,     // 全局作用域
    SCOPE_MAKEFILE    // 当前Makefile作用域
} VariableScope;

//变量结构体
typedef struct {
    char name[MAX_VAR_NAME];
    char value[MAX_VAR_VALUE];
    VariableScope scope;
} Variable;

//变量表结构体
typedef struct {
    int var_count;
    Variable variables[MAX_VARIABLES];
} VariableTable;

//初始化变量表
void init_variable_table(VariableTable* table);
//找到变量，返回变量索引
int find_variable(VariableTable* table,const char*name,VariableScope scope);
//添加和更新变量
void set_variable(VariableTable* table, const char* name, const char* value,VariableScope scope);
//获取变量值
const char* get_variable(VariableTable* table, const char* name);
//解析变量定义行
int parse_variable_line(VariableTable* table, char* line);
//从字符串中提取变量名（处理 $(VAR) 和 ${VAR} 格式）
int  extract_variable_name(const char* str,char *var_name);
// 展开字符串中的变量（替换 $(VAR) 或 ${VAR} 为实际值)
char* expand_variables(VariableTable* table, const char* input);
char* expand_variables_with_depth(VariableTable* table, const char* input, int depth);


#endif