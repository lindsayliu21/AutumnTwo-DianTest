#include"dependency_graph.h"
void init_graph(DependencyGraph *graph){
    graph->node_count=0;
    graph->edge_count=0;
    memset(graph->nodes,0,sizeof(graph->nodes));
    memset(graph->edges,0,sizeof(graph->edges));
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
        printf("%d\t%-16s\t%-16s\n",i,graph->nodes[edge->from].name,graph->nodes[edge->to].name);
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

int has_cycle(DependencyGraph *graph){
    //给visited数组和队列数组份配空间并初始化全部赋值为0
    //标记顶点是否被访问过的数组（0=未访问，1=已访问）
    int *visited=(int *)calloc(graph->node_count,sizeof(int));
    //标记顶点是否处于 "当前递归路径" 上的数组(0=不在，1=在)
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
