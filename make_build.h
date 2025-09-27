#ifndef BUILD_H
#define BUILD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include "make_parser.h"
#include "dependency_graph.h"

// 执行命令
int execute_command(const char *cmd);
// 执行目标（带时间戳检查）
int execute_build_with_timestamp_check(ParserState *state, const char *target_name, DependencyGraph *graph);
//获取文件最后修改时间
int get_file_mod_time(const char *filename,time_t *mod_time);
//检查目标是否需要重建
int should_rebuild_target(const char *target,const char dependencies[][MAX_FILE_NAME_LEN],int dep_count);
//根据时间戳检查结果执行构建
int build_target_if_needed(Rule *rule);

#endif