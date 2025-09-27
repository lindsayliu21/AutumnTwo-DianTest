#include "make_parser.h"

void init_parser(ParserState *state)
{
    state->rule_count = 0;
    memset(state->rules, 0, sizeof(state->rules));
}

int is_target_defined(ParserState *state, const char *target)
{
    for (int i = 0; i < state->rule_count; i++)
    {
        if (strcmp(state->rules[i].target, target) == 0)
            return i; // 返回已定义的规则索引
    }
    return -1; // 目标未定义
}

int is_file_exists(const char *filename)
{
    struct stat statbuf;
    return (stat(filename, &statbuf) == 0); // 返回1表示文件存在
}

void parse_makefile(const char *filename,ParserState *state,DependencyGraph *graph,VariableTable *variable_table)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("failed to open file!!\n");
        return;
    }
    
    // 清空变量表，确保每个Makefile都有自己独立的变量空间
    init_variable_table(variable_table);
    
    char line[MAX_LINE_LEN];
    int line_num = 0;
    Rule *current_rule = NULL;
    while (fgets(line, sizeof(line), file))
    {
        line_num++;
        line[strcspn(line, "\n\r")] = '\0';
        // 检查是否为目标行（不以Tab开头）
        if (line[0] != '\t')
        {
            int var_lined= parse_variable_line(variable_table, line);
            if(var_lined) continue; //变量行，继续读取下一行
            // 处理目标行
            // 查找冒号分隔符
            char *colon = strchr(line, ':');
            if (colon)
            {
                // 分割目标和依赖
                *colon = '\0';
                char *target_str = line;
                char *deps_str = colon + 1;
                // 检查目标是否为空
                if (strlen(target_str) == 0)
                {
                    printf("Line%d: Invalid target defined!!\n", line_num);
                }
                
                // 检查目标是否已定义
                int existing_idx = is_target_defined(state, target_str);
                if (existing_idx != -1)
                {
                    printf("Line%d: Duplicate target definition '%s'.  ", line_num, target_str);
                    printf(" The previous definition is right here: Line%d\n", state->rules[existing_idx].line_num);
                }
                // 去除目标末尾的空格
                char *target_str_end = target_str + strlen(target_str) - 1;
                while ( target_str_end> target_str && isspace((unsigned char)*target_str_end)) target_str_end--;
                *(target_str_end + 1) = '\0';
                //将目标加入节点列表中
                find_or_add_node(graph,target_str);

                current_rule = &state->rules[state->rule_count++];
                strncpy(current_rule->target, target_str, MAX_FILE_NAME_LEN - 1);
                current_rule->target[MAX_FILE_NAME_LEN - 1] = '\0';
                current_rule->dep_count = 0;
                current_rule->cmd_count = 0;
                current_rule->line_num = line_num;

                //依赖中有嵌套变量
                char *expanded_deps = expand_variables(variable_table, deps_str);
                // 解析依赖
                if(expanded_deps){
                char *dep = strtok(expanded_deps, " ");
                while (dep)
                {
                    if (strlen(dep) > 0)
                    {
                        strncpy(current_rule->dependencies[current_rule->dep_count], dep, MAX_FILE_NAME_LEN - 1);
                        current_rule->dependencies[current_rule->dep_count][MAX_FILE_NAME_LEN - 1] = '\0';
                        current_rule->dep_count++;
                        find_or_add_node(graph,dep);
                        add_edges(graph,target_str,dep);
                    }
                    dep = strtok(NULL, " ");
                }
                free(expanded_deps);
            }
            }
        }
        // 有Tab,则为命令行
        else
        {
            // 存储命令，去除开头的Tab
            char *cmd = line + 1;
            //如果命令中含有变量
            char *expanded_cmd = expand_variables(variable_table, cmd);
            if (expanded_cmd && strlen(expanded_cmd) > 0)
            {
                strncpy(current_rule->commands[current_rule->cmd_count].cmd, expanded_cmd, MAX_LINE_LEN - 1);
                current_rule->commands[current_rule->cmd_count].cmd[MAX_LINE_LEN - 1] = '\0';
                free(expanded_cmd);
                current_rule->cmd_count++;
            }
        }
    }
    fclose(file);
    // 检查所有依赖的有效性
    check_dependencies(state);
}

void check_dependencies(ParserState *state)
{
    for (int i = 0; i < state->rule_count; i++)
    {
        Rule *rule = &state->rules[i];
        for (int j = 0; j < rule->dep_count; j++)
        {
            const char *dep = rule->dependencies[j];
            int is_valid = 0;

            if (is_target_defined(state, dep) != -1)
                is_valid = 1;
            else if (is_file_exists(dep) == 1)
                is_valid = 1;
            // 如果依赖无效，输出错误
            if (!is_valid)
            {
                printf("Line%d: Invalid dependency '%s' \n", rule->line_num, dep);
            }
        }
    }
}