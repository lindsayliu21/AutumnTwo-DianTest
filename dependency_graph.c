#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#define MAX_LINE_LEN 1024
#define MAX_NAME_LEN 33
#define MAX_NODES 50
#define MAX_EDGES 100

// 图中节点结构体
typedef struct {
    char name[MAX_NAME_LEN];  // 节点名称（目标或依赖）
    int in_degree;            // 入度
    int out_degree;           // 出度
} Node;

// 图中边结构体
typedef struct {
    int from;  // 起始节点索引
    int to;    // 目标节点索引
} Edge;

// 依赖图结构体
typedef struct {
    Node nodes[MAX_NODES];    // 节点数组
    Edge edges[MAX_EDGES];    // 边数组
    int node_count;           // 节点数量
    int edge_count;           // 边数量
} DependencyGraph;

// 规则结构体
typedef struct {
    char target[MAX_NAME_LEN];
    char dependencies[MAX_NODES][MAX_NAME_LEN];
    int dep_count;
} Rule;

// 解析器状态结构体
typedef struct {
    Rule rules[MAX_NODES];
    int rule_count;
} ParserState;

// 函数声明
void init_graph(DependencyGraph *graph);
void init_parser(ParserState *state);
int find_or_add_node(DependencyGraph *graph, const char *name);
void add_edge(DependencyGraph *graph, const char *from, const char *to);
void parse_makefile(const char *filename, DependencyGraph *graph, ParserState *state);
void print_dependency_graph(DependencyGraph *graph);
int is_file_exists(const char *filename);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("用法: %s <makefile>\n", argv[0]);
        printf("示例: %s Makefile\n", argv[0]);
        return 1;
    }
    
    DependencyGraph graph;
    ParserState state;
    
    init_graph(&graph);
    init_parser(&state);
    
    // 解析Makefile并构建依赖图
    parse_makefile(argv[1], &graph, &state);
    
    // 输出依赖图
    print_dependency_graph(&graph);
    
    return 0;
}

// 初始化图
void init_graph(DependencyGraph *graph) {
    graph->node_count = 0;
    graph->edge_count = 0;
    memset(graph->nodes, 0, sizeof(graph->nodes));
    memset(graph->edges, 0, sizeof(graph->edges));
}

// 初始化解析器状态
void init_parser(ParserState *state) {
    state->rule_count = 0;
    memset(state->rules, 0, sizeof(state->rules));
}

// 查找节点，如果不存在则添加
int find_or_add_node(DependencyGraph *graph, const char *name) {
    // 查找节点是否已存在
    for (int i = 0; i < graph->node_count; i++) {
        if (strcmp(graph->nodes[i].name, name) == 0) {
            return i;
        }
    }
    
    // 如果节点不存在且图未满，则添加新节点
    if (graph->node_count < MAX_NODES) {
        strncpy(graph->nodes[graph->node_count].name, name, MAX_NAME_LEN - 1);
        graph->nodes[graph->node_count].name[MAX_NAME_LEN - 1] = '\0';
        graph->nodes[graph->node_count].in_degree = 0;
        graph->nodes[graph->node_count].out_degree = 0;
        return graph->node_count++;
    }
    
    return -1; // 图已满
}

// 添加边（依赖关系）
void add_edge(DependencyGraph *graph, const char *from, const char *to) {
    int from_idx = find_or_add_node(graph, from);
    const int to_idx=find_or_add_node(graph,to);
    
    if (from_idx == -1 || to_idx == -1) {
        printf("错误: 无法添加边 %s -> %s，节点数量已达上限\n", from, to);
        return;
    }
    
    // 更新节点的度数
    graph->nodes[from_idx].out_degree++;
    graph->nodes[to_idx].in_degree++;
    
    // 添加边
    if (graph->edge_count < MAX_EDGES) {
        graph->edges[graph->edge_count].from = from_idx;
        graph->edges[graph->edge_count].to = to_idx;
        graph->edge_count++;
    } else {
        printf("错误: 边数量已达上限\n");
    }
}

// 检查文件是否存在
int is_file_exists(const char *filename) {
    struct stat statbuf;
    return (stat(filename, &statbuf) == 0);
}

// 解析Makefile并构建依赖图
void parse_makefile(const char *filename, DependencyGraph *graph, ParserState *state) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("无法打开文件");
        return;
    }
    
    char line[MAX_LINE_LEN];
    Rule *current_rule = NULL;
    
    while (fgets(line, sizeof(line), file)) {
        // 移除换行符
        line[strcspn(line, "\n\r")] = '\0';
        
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
                    printf("警告: 发现空目标\n");
                    continue;
                }
                
                // 添加目标节点
                find_or_add_node(graph, target_str);
                
                // 保存规则信息用于后续处理
                if (state->rule_count < MAX_NODES) {
                    current_rule = &state->rules[state->rule_count];
                    strncpy(current_rule->target, target_str, MAX_NAME_LEN - 1);
                    current_rule->target[MAX_NAME_LEN - 1] = '\0';
                    current_rule->dep_count = 0;
                    state->rule_count++;
                } else {
                    printf("警告: 规则数量已达上限\n");
                    current_rule = NULL;
                    continue;
                }
                
                // 解析依赖
                char *dep = strtok(deps_str, " ");
                while (dep && current_rule) {
                    if (strlen(dep) > 0) {
                        // 保存依赖信息
                        if (current_rule->dep_count < MAX_NODES) {
                            strncpy(current_rule->dependencies[current_rule->dep_count], dep, MAX_NAME_LEN - 1);
                            current_rule->dependencies[current_rule->dep_count][MAX_NAME_LEN - 1] = '\0';
                            current_rule->dep_count++;
                            
                            // 添加依赖节点和边
                            find_or_add_node(graph, dep);
                            add_edge(graph, target_str, dep);
                        }
                    }
                    dep = strtok(NULL, " ");
                }
            }
        }
    }
    
    fclose(file);
}

// 打印依赖图
void print_dependency_graph(DependencyGraph *graph) {
    printf("\n========== Makefile 依赖图 ==========\n");
    
    // 打印所有节点
    printf("\n节点列表 (共 %d 个):\n", graph->node_count);
    printf("序号\t节点名称\t\t入度\t出度\n");
    printf("----------------------------------------\n");
    for (int i = 0; i < graph->node_count; i++) {
        printf("%d\t%-20s\t%d\t%d\n", 
               i, 
               graph->nodes[i].name, 
               graph->nodes[i].in_degree, 
               graph->nodes[i].out_degree);
    }
    
    // 打印所有边
    printf("\n依赖关系 (共 %d 条):\n", graph->edge_count);
    printf("序号\t起始节点 -> 目标节点\n");
    printf("----------------------------------------\n");
    for (int i = 0; i < graph->edge_count; i++) {
        int from = graph->edges[i].from;
        int to = graph->edges[i].to;
        printf("%d\t%s -> %s\n", 
               i, 
               graph->nodes[from].name, 
               graph->nodes[to].name);
    }
    
    // 打印邻接表形式
    printf("\n邻接表表示:\n");
    printf("----------------------------------------\n");
    for (int i = 0; i < graph->node_count; i++) {
        printf("%s: ", graph->nodes[i].name);
        int first = 1;
        for (int j = 0; j < graph->edge_count; j++) {
            if (graph->edges[j].from == i) {
                if (!first) printf(", ");
                printf("%s", graph->nodes[graph->edges[j].to].name);
                first = 0;
            }
        }
        printf("\n");
    }
    
    printf("\n=====================================\n");
}