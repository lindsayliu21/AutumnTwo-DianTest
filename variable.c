#include "variable.h"

void init_variable_table(VariableTable* table){
    table->var_count=0;
    memset(table->variables,0,sizeof(table->variables));
}

void set_variable(VariableTable* table, const char* name, const char* value, VariableScope scope){
    //已经存在该变量则更新
   int index=find_variable(table,name,scope);
   if(index>=0){
        strncpy(table->variables[index].value,value,MAX_VAR_VALUE-1);
        table->variables[index].value[MAX_VAR_VALUE-1]='\0';
        return;
    }
    //不存在则添加
    if(table->var_count<MAX_VARIABLES){
        strncpy(table->variables[table->var_count].name,name,MAX_VAR_NAME-1);
        table->variables[table->var_count].name[MAX_VAR_NAME-1]='\0';
        strncpy(table->variables[table->var_count].value,value,MAX_VAR_VALUE-1);
        table->variables[table->var_count].value[MAX_VAR_VALUE-1]='\0';
        table->variables[table->var_count].scope = scope; 
        table->var_count++;

    }
}

int find_variable(VariableTable* table,const char*name,VariableScope scope){
    for(int i=0;i<table->var_count;i++){
        if(strcmp(table->variables[i].name,name)==0&&table->variables[i].scope == scope){
            return i;
        }
    }
    return -1;
}

const char *get_variable(VariableTable* table,const char*name){
   return find_variable(table,name,SCOPE_MAKEFILE)>=0?table->variables[find_variable(table,name,SCOPE_MAKEFILE)].value:NULL;
}

int  extract_variable_name(const char * str,char *var_name){
if(str[0]!='$'){
return 0;
}
//处理$（Var）
if(str[1]=='('){
   const char * end= strchr(str+2,')');
   if(!end){
       return 0;}
    size_t len=end-str-2;
    if(len>MAX_VAR_NAME){
        len=MAX_VAR_NAME-1;
    }
    strncpy(var_name,str+2,len);
    var_name[len]='\0';
    return end-str+1;//返回变量名长度
}
//处理${Var}
else if(str[1]=='{'){
    const char * end= strstr(str+2,"}");
    if(!end){
        return 0;
    }
    size_t len=end-str-2;
    if(len>MAX_VAR_NAME){
        len=MAX_VAR_NAME-1;
    }
    strncpy(var_name,str+2,len);
    var_name[len]='\0';
    return end-str+1;
}
return 0;
}

char* expand_variables(VariableTable* table, const char* input){
    return expand_variables_with_depth(table, input, 0);
}

char* expand_variables_with_depth(VariableTable* table, const char* input, int depth){
    if(!input) return NULL;
    
    // 检查递归深度，防止无限递归
    if(depth > MAX_EXPANSION_DEPTH) {
        fprintf(stderr, "Warning: Maximum variable expansion depth exceeded\n");
        return NULL;
    }
    
    size_t max_output_len = MAX_VAR_VALUE * 4; 
    char * output =(char*) malloc(max_output_len);
    if(!output) return NULL;
    output[0]='\0';
    size_t output_len = 0;
    size_t i=0;
    size_t len=strlen(input);
    while(i<len){
        char var_name[MAX_VAR_NAME];
        int var_len=extract_variable_name(input+i,var_name);
        if(var_len>0){
           const char * var_value=get_variable(table,var_name);
            if(var_value){
                //递归展开变量值中的嵌套引用
                char * expanded_value=expand_variables_with_depth(table, var_value, depth+1);
                if(expanded_value){
                    size_t expanded_len = strlen(expanded_value);
                    // 检查缓冲区是否足够，避免溢出
                    if (output_len + expanded_len < max_output_len - 1) {
                        strcat(output, expanded_value);
                        output_len += expanded_len;
                    }
                    else {
                        fprintf(stderr, "Warning: Variable expansion overflow for '%s'\n", var_name);
                    }
                    free(expanded_value);
                }
                else{     
                    // 变量未定义或递归深度超限，直接添加原始变量名
                    if(output_len + var_len < max_output_len - 1) {
                        strncat(output, input+i, var_len);
                        output_len += var_len;
                    }
                    else {
                        fprintf(stderr, "Warning: Variable expansion overflow for '%s'\n", var_name);
                    }
                }                         
                i += var_len;
                continue;
            }
            else {
                // 变量未定义，输出警告但继续处理
                fprintf(stderr, "Warning: variable '%s' is not defined!!\n", var_name);
            }
        }
        // 处理普通字符，确保不溢出
        if (output_len < max_output_len - 1) {
            output[output_len++] = input[i];
            output[output_len] = '\0'; // 实时更新终止符
        }
        i++;
    }
    return output;
}

int parse_variable_line(VariableTable* table, char* line){
if(line[0]!='\t'){
    char *var_equal=strchr(line,'=');
    if(!var_equal) return 0; 
    *var_equal='\0';
    char *var_name=line;
    char *var_value=var_equal+1;
    if(strlen(var_name)==0){
    printf("Error: Variable name cannot be empty!!\n");
    return 0;
    }
    if(strlen(var_value)==0){
    printf("Error: Variable value cannot be empty!!\n");
    return 0;
    }
    // 去除变量名末尾的空格
    char *end = var_name + strlen(var_name) - 1;
    while (end > var_name && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
    //去除变量值开头的空格
    while(*var_value!='\0'&&isspace((unsigned char)*var_value)){
    var_value++;
    }   
    // 处理变量值中的变量引用（嵌套展开）
    char *expanded_value = expand_variables(table, var_value);
    if (expanded_value) {
        set_variable(table, var_name, expanded_value,SCOPE_MAKEFILE);
        free(expanded_value);
    } else {
        set_variable(table, var_name, var_value,SCOPE_MAKEFILE);
    }  
    return 1; // 成功解析变量定义
}
else if(line[0]=='\t'){
    char *lined=expand_variables(table, line);
    free(lined);
    return 1;
}
return 0;//不含变量行，解析失败
}
