#ifndef DEPENDENCY_GRAPH_H
#define DEPENDENCY_GRAPH_H
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAX_FILE_NAME_LEN 33 // 最大规则数量
#define MAX_NODES 50
#define MAX_EDGES 100

//节点结构体
typedef struct{
    char name[MAX_FILE_NAME_LEN];
    int in_degree;
    int out_degree;
}Node;

//边结构体
typedef struct {
    int from;
    int to;
}Edge;

//依赖图结构体
typedef struct{
    Node nodes[MAX_NODES];
    Edge edges[MAX_EDGES];
    int node_count;
    int edge_count;
}DependencyGraph;

//初始化依赖图
void init_graph(DependencyGraph *graph);
//找到和增加节点（目标、依赖、文件）
int find_or_add_node(DependencyGraph *graph, const char *name);
//加边（增加了节点之后加边）
void add_edges(DependencyGraph *graph, const char *from,const char *to);
//打印依赖图，各个目标/依赖的入度（目标）、出度（依赖）
void print_dependency_graph(DependencyGraph *graph);
//拓扑排序
int topological_sort(DependencyGraph *graph, int *result);
//检查循环依赖
int has_cycle(DependencyGraph *graph);
int has_cycle_dfs(DependencyGraph *graph,int node,int *visited,int *stack);

#endif