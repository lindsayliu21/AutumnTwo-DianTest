#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

// 优化，边界检查增强

#define MAX_LINE_LEN 1024    // 文件名最长32个字符+1个结束符
#define MAX_FILE_NAME_LEN 33 // 最大规则数量
#define MAX_RULES 30         // 每个目标最大依赖数量
#define MAX_DEPEND_NUM 30    // 每个目标最大依赖数量
#define MAX_COMMAND_NUM 30   // 每个目标最大命令数量
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
void parse_makefile(const char *filename,ParserState *state,DependencyGraph *graph);
// 检查依赖是否有效
void check_dependencies(ParserState *state);
// 执行命令
int execute_command(const char *cmd);
// 执行目标
int execute_target_topologically(ParserState *state, const char *target,DependencyGraph *graph);
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
int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("用法: %s <makefile> <target>\n", argv[0]);
        printf("示例: %s Makefile app\n", argv[0]);
        return 1;
    }
    DependencyGraph graph;
    ParserState state;
    init_graph(&graph);
    init_parser(&state);
    // 解析Makefile
    parse_makefile(argv[1], &state,&graph);
    
    // 检查依赖
    check_dependencies(&state);

    // 输出依赖图
    print_dependency_graph(&graph);
    
    //检测循环依赖
    if(has_cycle(&graph)){
    printf("Error: Detected a circulsr dependency!!\n");
    return 1;
    }
    // 执行指定目标
    int result = execute_target_topologically(&state, argv[2],&graph);
    
    return result;
}

void init_graph(DependencyGraph *graph){
    graph->node_count=0;
    graph->edge_count=0;
    memset(graph->nodes,0,sizeof(graph->nodes));
    memset(graph->edges,0,sizeof(graph->edges));
}
void init_parser(ParserState *state)
{
    state->rule_count = 0;
    memset(state->rules, 0, sizeof(state->rules));
}

int find_or_add_node(DependencyGraph *graph, const char *name){
    //检查节点是否已经存在
    for(int i=0;i<graph->node_count;i++){
        if(strcmp(graph->nodes[i].name,name)==0){
        return i;}
    }
    if(graph->node_count<MAX_NODES){
        strncpy(graph->nodes[graph->node_count].name,name,MAX_FILE_NAME_LEN-1);
        graph->nodes[graph->node_count].name[MAX_FILE_NAME_LEN-1]='\0';
        graph->nodes[graph->node_count].in_degree=0;
        graph->nodes[graph->node_count].out_degree=0;
        return graph->node_count++;
        
    }
    //节点数量达到上限
    return -1;
}

void add_edges(DependencyGraph *graph, const char *from,const char *to){
    const int from_idx=find_or_add_node(graph,from);
    const int to_idx=find_or_add_node(graph,to);
    if(from_idx==-1||to_idx==-1){
        printf("error:failed to add edge %s->%s!!",from,to);
        exit(1);
    }
    //更新节点的度数
    graph->nodes[from_idx].out_degree++;  // from节点的出度增加
    graph->nodes[to_idx].in_degree++;     // to节点的入度增加
    //添加边
    if(graph->edge_count<MAX_EDGES){
        graph->edges[graph->edge_count].from=from_idx;
        graph->edges[graph->edge_count].to=to_idx;
        graph->edge_count++;
    }
    else{
        printf("error:too many edges!!");
        exit(1);
    }
    
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



int has_cycle(DependencyGraph *graph){
    //给visited数组和队列数组份配空间并初始化全部赋值为0
    //标记顶点是否被访问过的数组（0=未访问，1=已访问）
    int *visited=(int *)calloc(graph->node_count,sizeof(int));
    //标记顶点是否处于 “当前递归路径” 上的数组(0=不在，1=在)
    int *stack=(int *)calloc(graph->node_count,sizeof(int ));
    //遍历所有节点
    for(int i=0;i<graph->node_count;i++){
        if(has_cycle_dfs(graph,i,visited,stack)){
            free(visited);
            free(stack);
            return 1;
        }  
    }
    free(visited);
    free(stack);
    return 0;
}
int has_cycle_dfs(DependencyGraph *graph,int node,int *visited,int *stack){
    //若顶点未访问，则标记访问过
    if(visited[node]==0){
        visited[node]=1;
        stack[node]=1;
    }
    // 遍历所有从v出发的边（即v的所有邻接顶点）
    for(int i=0;i<graph->edge_count;i++){
        if(graph->edges[i].from==node){
            int dep=graph->edges[i].to;
            //情况1：邻接顶点dep未访问过，递归检查dep
            //若发现有环，则返回1；
            if(visited[dep]==0&&has_cycle_dfs(graph,dep,visited,stack)){
                return 1;
            }
            //情况2：邻接顶点dep访问过 且 在当前递归路径上，说明存在循环依赖
            else if(stack[dep]){
                return 1;
            }
        }
    }
    stack[node]=0;
    return 0; 
}

int topological_sort(DependencyGraph *graph, int *result) {
    //创建入度数组
    int * in_degree=(int*)calloc(graph->node_count,sizeof(int));
    for(int i=0;i<graph->node_count;i++){
        in_degree[i]=graph->nodes[i].in_degree;
    }  
    //创建队列
    int *queue=(int *)malloc(graph->node_count*sizeof(int));
    int front=0;int rear=0;
    int result_id=0;
    //将入度为0的节点加入队列
    for(int i=0;i<graph->node_count;i++){
        if(in_degree[i]==0){
            queue[rear++]=i;
        }
    }
    while(front<rear){
        int v=queue[front++];
        result[result_id++]=v;  // 存储处理结果
        // 遍历所有从v出发的边
        for(int j=0;j<graph->edge_count;j++){
            if(graph->edges[j].from==v){
                int adj = graph->edges[j].to;  // 获取邻接节点
                in_degree[adj]--;  // 减少邻接节点的入度
                if(in_degree[adj]==0) {  // 如果邻接节点入度为0
                    queue[rear++]=adj;  // 将邻接节点加入队列
                }
            }
        }    
    }
    free(in_degree);
    free(queue);
    if(graph->node_count!=result_id) return -1;  // 检查处理的节点数
    return result_id;
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

void parse_makefile(const char *filename,ParserState *state,DependencyGraph *graph)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("failed to open file!!\n");
        return;
    }
    
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
                //将目标加入节点列表中
                find_or_add_node(graph,target_str);

                current_rule = &state->rules[state->rule_count++];
                strncpy(current_rule->target, target_str, MAX_FILE_NAME_LEN - 1);
                current_rule->target[MAX_FILE_NAME_LEN - 1] = '\0';
                current_rule->dep_count = 0;
                current_rule->cmd_count = 0;
                current_rule->line_num = line_num;

                // 解析依赖
                char *dep = strtok(deps_str, " ");
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
            }
        }
        // 有Tab,则为命令行
        else
        {
            // 存储命令，去除开头的Tab
            char *cmd = line + 1;
            if (strlen(cmd) > 0)
            {
                strncpy(current_rule->commands[current_rule->cmd_count].cmd, cmd, MAX_LINE_LEN - 1);
                current_rule->commands[current_rule->cmd_count].cmd[MAX_LINE_LEN - 1] = '\0';
                current_rule->cmd_count++;
            }
        }
    }
    fclose(file);
    // 检查所有依赖的有效性
    check_dependencies(state);
}

int execute_command(const char *cmd)
{
    printf("Executing command: %s\n",cmd);
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork failed!!");
        return 1;
    }
    else if (pid == 0)
    {
        execl("/bin/sh", "sh", "-c", cmd, (char *)NULL);
        perror("excell failed!!");
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

int execute_target_topologically(ParserState *state, const char *target,DependencyGraph *graph)
{
    int target_idx = is_target_defined(state, target);
    if (target_idx == -1)
    {
        printf("Target '%s' not defines!!\n)", target);
        return 1;
    }
    // 进行拓扑排序
    int *sort_result = (int*)malloc(graph->node_count * sizeof(int));
    int sort_count = topological_sort(graph, sort_result);

    if (sort_count == -1) {
        printf("Error: Detected a dependency!!\n");
        free(sort_result);
        return 1;
    }

    printf("\n拓扑排序结果（构建顺序）:\n");
    for (int i = 0; i < sort_count; i++) {
        printf("%s ", graph->nodes[sort_result[i]].name);
    }
    printf("\n\n");

    for(int i=sort_count-1;i>=0;i--){
        const char* target_1=graph->nodes[sort_result[i]].name;
        int target_id=is_target_defined(state,target_1);
        if(target_id!=-1){
            Rule *rule=&state->rules[target_id];
            printf("Executing %s target...\n",target_1);
            for(int j=0;j<rule->cmd_count;j++){
                Command *uncmd=&rule->commands[j];  
                int result=execute_command(uncmd->cmd);
                if(result!=0)
                    return result;
            }
        }
    }
    free(sort_result);
    return 0;
}

void print_dependency_graph(DependencyGraph *graph){
printf("\n========== Makefile 依赖图 ==========\n");
//打印所有节点
printf("节点列表（共 %d 个）：\n",graph->node_count);
printf("序号\t节点名称\t\t入度\t出度\n");
printf("----------------------------------------\n");
for(int i=0;i<graph->node_count;i++){
    Node *node=&graph->nodes[i];
    printf("%d\t%-20s\t%d\t%d\n",i,node->name,node->in_degree,node->out_degree);
}

//打印所有边
printf("\n边列表（共 %d 条）：\n",graph->edge_count);
printf("序号\t%-32s——>%-32s\n","源节点","目标节点");
for(int i=0;i<graph->edge_count;i++){
    Edge *edge=&graph->edges[i];
    printf("%d\t%s\t %s\n",i,graph->nodes[edge->from].name,graph->nodes[edge->to].name);
}

//打印邻接表形式
printf("\n邻接表形式：\n");
for(int i=0;i<graph->node_count;i++){
    Node *node=&graph->nodes[i];
    printf("%-32s: ",node->name);
    int first=1;
    for(int j=0;j<graph->edge_count;j++){
        if(graph->edges[j].from==i) {
            if(!first){
                printf("->");
            }
            printf("%s",graph->nodes[graph->edges[j].to].name);
            first=0;
        }
    }
    printf("\n");
}
printf("\n=====================================\n");
}