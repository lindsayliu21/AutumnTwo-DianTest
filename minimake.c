#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "make_parser.h"
#include "dependency_graph.h"
#include "make_build.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usages:./minimake <target>\n");
        printf("Example:./minimake app\n");
        return 1;
    }
    if(strcmp(argv[0],"./minimake")!=0){
       printf("Usages:./minimake <target>\n");
       printf("Exmple:./minimake app \n");
       return 1;
    }
    DependencyGraph graph;
    ParserState state;
    VariableTable variable_table;
    init_graph(&graph);
    init_parser(&state);
    init_variable_table(&variable_table);
    char *filename="Makefile_cleared.mk";
    // 解析Makefile
    parse_makefile(filename, &state,&graph,&variable_table);
    
    // 检查依赖
    check_dependencies(&state);

    // 输出依赖图
    print_dependency_graph(&graph);
    
    // 先检查目标是否存在
    if (is_target_defined(&state, argv[1]) == -1) {
        printf("错误: 目标 '%s' 不存在!\n", argv[1]);
        return 1;
    }
    
    // 执行带时间戳检查的构建
    int result = execute_build_with_timestamp_check(&state, argv[1], &graph);
    
    return result;
}