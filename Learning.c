/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Employee{

    char Name[50];
    int Salary;

};

void Insert(struct Employee *Arr, int *Size, char *Name, int Salary);
void Delete(struct Employee *Arr, int *Size, char *Name);

int main(){

    int Len;
    scanf("%d", &Len);

    struct Employee *Arr = (struct Employee *)malloc(Len*sizeof(struct Employee));

    for(int x = 0; x < Len; x++){
        printf("Hello\n");
        
        scanf("%s", &(((Arr+x)->Name)[0]));
        scanf("%d", &((Arr+x)->Salary));

    }

    for(int x = 0; x < Len; x++){

        printf("%s, %d\n", &((Arr+x)->Name)[0], (Arr+x)->Salary);

    }

    char Name[50];
    int Salary;

    scanf("%s", &Name[0]);
    scanf("%d", &Salary);

    Insert(Arr, &Len, &Name[0], Salary);

    for(int x = 0; x < Len; x++){

        printf("%s, %d\n", &((Arr+x)->Name)[0], (Arr+x)->Salary);

    }

    scanf("%s", &Name[0]);

    Delete(Arr, &Len, &Name[0]);

    for(int x = 0; x < Len; x++){

        printf("%s, %d\n", &((Arr+x)->Name)[0], (Arr+x)->Salary);

    }

    return 0;

}

void Insert(struct Employee *Arr, int *Size, char *Name, int Salary){

    Arr = (struct Employee *)realloc(Arr, ++(*Size));

    int Index = 2;
    
    struct Employee New = {*Name, Salary};
    struct Employee Temp1 = *(Arr+Index);
    struct Employee Temp2;
    *(Arr+Index) = New;

    for(int x = Index+1; x < *Size; x++){

        Temp2 = *(Arr + x);
        *(Arr + x) = Temp1;
        Temp1 = Temp2;

    }

    return;

}

void Delete(struct Employee *Arr, int *Size, char *Name){

    int Index;

    for(Index = 0; strcmp(&((Arr+Index)->Name)[0], Name) == 0; Index++);

    for(int x = Index+1; Index < *Size; Index++){

        *(Arr + x - 1) = *(Arr + x);

    }

    Arr = (struct Employee *)realloc(Arr, --(*Size));

    return;

}*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <omp.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct {
    int V;
    int **capacity, **flow;
    int *height, *excess;
} Graph;

Graph* createGraph(int V) {
    Graph* g = (Graph*)malloc(sizeof(Graph));
    if (!g) {
        fprintf(stderr, "Memory allocation failed for Graph\n");
        exit(1);
    }
    g->V = V;
    g->capacity = (int**)malloc(V * sizeof(int*));
    g->flow = (int**)malloc(V * sizeof(int*));
    g->height = (int*)calloc(V, sizeof(int));
    g->excess = (int*)calloc(V, sizeof(int));

    if (!g->capacity || !g->flow || !g->height || !g->excess) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    for (int i = 0; i < V; ++i) {
        g->capacity[i] = (int*)calloc(V, sizeof(int));
        g->flow[i] = (int*)calloc(V, sizeof(int));
        if (!g->capacity[i] || !g->flow[i]) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
    }

    return g;
}

void addEdge(Graph* g, int u, int v, int cap) {
    g->capacity[u][v] = cap;
}

void push(Graph* g, int u, int v) {
    int flow = MIN(g->excess[u], g->capacity[u][v] - g->flow[u][v]);
    g->flow[u][v] += flow;
    g->flow[v][u] -= flow;
    g->excess[u] -= flow;
    g->excess[v] += flow;
}

void relabel(Graph* g, int u) {
    int min_height = INT_MAX;
    for (int v = 0; v < g->V; v++) {
        if (g->capacity[u][v] - g->flow[u][v] > 0) {
            min_height = MIN(min_height, g->height[v]);
        }
    }
    if (min_height < INT_MAX) {
        g->height[u] = min_height + 1;
    }
}

void initPreflow(Graph* g, int source) {
    g->height[source] = g->V;
    for (int v = 0; v < g->V; v++) {
        int flow = g->capacity[source][v];
        g->flow[source][v] = flow;
        g->flow[v][source] = -flow;
        g->excess[v] = flow;
        g->excess[source] -= flow;
    }
}

int pushRelabelMaxFlow(Graph* g, int source, int sink) {
    initPreflow(g, source);

    int *active = (int*)calloc(g->V, sizeof(int));
    for (int i = 0; i < g->V; i++) {
        if (i != source && i != sink) active[i] = 1;
    }

    while (1) {
        int work_done = 0;

        #pragma omp parallel for schedule(dynamic) reduction(|:work_done)
        for (int u = 0; u < g->V; u++) {
            if (active[u]) {
                active[u] = 0;
                while (g->excess[u] > 0) {
                    for (int v = 0; v < g->V; v++) {
                        if (g->excess[u] > 0 && g->height[u] == g->height[v] + 1 && g->capacity[u][v] - g->flow[u][v] > 0) {
                            push(g, u, v);
                            if (v != source && v != sink) active[v] = 1;
                            work_done = 1;
                        }
                    }
                    if (g->excess[u] > 0) {
                        relabel(g, u);
                        active[u] = 1;
                        work_done = 1;
                        break;
                    }
                }
            }
        }

        if (!work_done) break;
    }

    free(active);
    return g->excess[sink];
}

void freeGraph(Graph* g) {
    for (int i = 0; i < g->V; i++) {
        free(g->capacity[i]);
        free(g->flow[i]);
    }
    free(g->capacity);
    free(g->flow);
    free(g->height);
    free(g->excess);
    free(g);
}

int main() {
    int V = 6;  // Number of vertices
    Graph* g = createGraph(V);

    // Add edges (u, v, capacity)
    addEdge(g, 0, 1, 16);
    addEdge(g, 0, 2, 13);
    addEdge(g, 1, 2, 10);
    addEdge(g, 1, 3, 12);
    addEdge(g, 2, 1, 4);
    addEdge(g, 2, 4, 14);
    addEdge(g, 3, 2, 9);
    addEdge(g, 3, 5, 20);
    addEdge(g, 4, 3, 7);
    addEdge(g, 4, 5, 4);

    int source = 0, sink = 5;

    #pragma omp parallel
    {
        #pragma omp single
        printf("Number of threads: %d\n", omp_get_num_threads());
    }

    int maxFlow = pushRelabelMaxFlow(g, source, sink);
    printf("Maximum Flow: %d\n", maxFlow);

    freeGraph(g);
    return 0;
}