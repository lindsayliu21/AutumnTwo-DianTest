#include"make_build.h"
int execute_command(const char *cmd)
{
    printf("Executing command: %s\n",cmd);
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork failed!!\n");
        return 1;
    }
    else if (pid == 0)
    {
        execl("/bin/sh", "sh", "-c", cmd, (char *)NULL);
        perror("excell failed!!\n");
        exit(1);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
        // 返回子进程的退出状态（0表示成功，非0表示失败）
        return WEXITSTATUS(status);
    }
}

int execute_build_with_timestamp_check(ParserState *state, const char *target_name, DependencyGraph *graph)
{

    //检测循环依赖
    if(has_cycle(graph)){
        printf("Error: Detected a circular dependency!!\n");
        return 1;
    }
    // 进行拓扑排序
    int *sort_result = (int*)malloc(graph->node_count * sizeof(int));
    int sort_count = topological_sort(graph, sort_result);
    //双重检测循环依赖
    if (sort_count == -1) {
        printf("Error: Detected a circular dependency!!\n");
        free(sort_result);
        return 1;
    }

    printf("\n拓扑排序结果（构建顺序）:\n");
    for (int i = 0; i < sort_count; i++) {
        printf("%s ", graph->nodes[sort_result[i]].name);
    }
    printf("\n\n");

    // 按拓扑顺序构建目标（从前往后）
    for(int i = sort_count-1; i>=0; i--){
        const char* current_target = graph->nodes[sort_result[i]].name;
        int target_id = is_target_defined(state, current_target);
        if(target_id != -1){
            Rule *rule = &state->rules[target_id];
            printf("Deal with target: %s\n", current_target);
            int result = build_target_if_needed(rule);
            if(result){
                free(sort_result);
                return result;
            }
        }
    }
    free(sort_result);
    return 0;
}

int get_file_mod_time(const char *filename,time_t *mod_time){
    struct  stat stat_buff;
    if(stat(filename,&stat_buff)==0){
        *mod_time=stat_buff.st_mtime;
        return 1;
    }
    return 0;
}

int should_rebuild_target(const char *target,const char dependencies[][MAX_FILE_NAME_LEN],int dep_count){
    time_t target_time;
    //目标文件不存在
    if(!get_file_mod_time(target,&target_time)){
        printf("Target file '%s' is not exiting，need to rebuild\n",target);
        return 1;
    }
    //目标文件存在
    //1.检查依赖是否存在
    //2.比较依赖和目标文件的修改时间
    for(int i=0;i<dep_count;i++){
        time_t dep_time;
        if(!get_file_mod_time(dependencies[i],&dep_time)){
            printf("Error: dependency file '%s' is not exiting\n", dependencies[i]);
            return -1;
        }
        // 如果依赖文件比目标文件新，需要重新构建
        if(difftime(dep_time,target_time) > 0){
            printf("Dependency file '%s' is newer than '%s' ，need to rebuild\n", dependencies[i], target);
            return 1;
        }
    }
    // 所有依赖文件都比目标文件旧，不需要重新构建
    printf("Target file '%s' is the newest one，need not to rebuild\n",target);
    return 0;
}

int build_target_if_needed(Rule *rule) {
    int rebuild=should_rebuild_target(rule->target,rule->dependencies,rule->dep_count);
    switch (rebuild)
    {
        case 1://需要重新构建
            printf("Staring to build target '%s'...\n",rule->target);
            for(int i=0;i<rule->cmd_count;i++){
                
                int result=execute_command(rule->commands[i].cmd);
                if(result){
                    printf("Failed to build target '%s' \n",rule->target);
                    return result;
                }
            }
            printf("Succeeded to build target '%s'\n",rule->target);
            break;
        case 0://不需要重新构建
            printf("Skip building the target'%s' (up to date)\n",rule->target);
            break;
        case -1://出错
            printf("Error occurred when checking the target '%s' \n",rule->target);
            return 1;
    }
    return 0;
}