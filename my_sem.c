#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

#define GRAPH_SIZE 5

int (*_sem_wait)(sem_t *) = NULL;
int (*_sem_post)(sem_t *) = NULL;

typedef union {
    pthread_t pid;
    sem_t* resource;
} Info; 

/**
 * Controla se grafo ja foi criado.
 * Se value == -1, o grafo nao foi criado.
 * Senao, value é igual ao tamanho do grafo (quantidade de vertices)
 */ 
int graphCreated = -1; 
int hasCycle = 0;
Info knownVertex[GRAPH_SIZE];
int numberOfKnownVertex = 0;

typedef struct node {
    int v;
    //Info info;
    int type; 
    struct node* next;
} Node;

Node* createNode(int v/* , Info i */);

typedef struct graph {
    int size;
    int* visited;
    Node** list;
} Graph;


Graph* createGraph(int n);

void addEdge(Graph *g, int src, int dest/* , Info srcInfo, Info destInfo */);

void removeEdge(Graph *g, int src, int dest);

void printGraph(Graph *g);

void DFS(Graph* g, int v);

void resetVisited(Graph* g);

void testGraph(Graph* g);

Graph* g;

/* 
int checkForDeadLock(sem_t *proc){
    FILE *file;
    char *pr;
    
    int bufferLength = 255;
    char buffer[bufferLength];

    file= fopen("auxFile", "w+");
    fprintf(file,"%p",proc);
    fclose(file);

    file= fopen("auxFile", "r+");
    pr=fgets(buffer, bufferLength, file);
    fclose(file);

    file = fopen("entrada", "r");
    while(fgets(buffer, bufferLength, file)) {
        strtok(buffer, "\n");

        printf("\nbuffer: %s proc: %s", buffer, pr);
        
        if(strcmp(buffer,pr)==0){
            return -1;
        } 
    }
    

    file = fopen("entrada", "a");
    fprintf(file,"%p\n",proc);
    fclose(file);
    return 0;
}

 */

/* 
     * sem_wait:
     * Criar aresta processo -> recurso 
     * Rodar DFS. 
     * Se der deadlock, retorna erro
     * Se nao:
     *    - Remove aresta processo -> recurso
     *    - Cria aresta recurso -> processo
     * 
     * sem_post:
     * - Remove aresta recurso -> processo
     *  
     */
 
/* 
0 - p1
1 - r1
2 - p2
3 - r2

0, 1 */

int sem_wait(sem_t *sem) {
    //blablabla
    sem_t *aux_sem = sem;
    int ehIgual = (aux_sem != sem);
    printf("%d", ehIgual);
    /* g = createGraph(GRAPH_SIZE);
    int posSem = -1;
    int posThread = -1;
    pthread_t aux_t = pthread_self();

    for (int i=0; i<GRAPH_SIZE; i++) {
        if (knownVertex[i].resource == sem) {
            posSem = i;
        } else if (knownVertex[i].pid == aux_t) {
            posThread = i;
        }
    }
    if (posSem < 0) {
        knownVertex[numberOfKnownVertex].resource = sem;
        posSem = numberOfKnownVertex;
        numberOfKnownVertex++;
    }
    if (posThread < 0) {
        knownVertex[numberOfKnownVertex].pid = aux_t;
        posThread = numberOfKnownVertex;
        numberOfKnownVertex++;
    }
 */
    


    //int r;
    //testGraph(g);
    //addEdge(g, );

         
    /* printf("Semaforo: %p\n", sem);

    printf("Processo: %ld\n", aux_t);
    int r;    
    if (!_sem_wait) {
        _sem_wait = dlsym(RTLD_NEXT, "sem_wait");
    } */

    // printf("\nSem: %p",sem);
    /* if(checkForDeadLock(sem)!=0){
        return -2;
    } */


    //r = _sem_wait(sem);
    //return(r);
    return 1;
}

/* int graphRemove(sem_t *proc){
    FILE *file;
    char *pr,**aux;
    
    int bufferLength = 255;
    char buffer[bufferLength];

    file= fopen("entrada", "r");
    while(fgets(buffer, bufferLength, file)) {
        strtok(buffer, "\n");

        printf("\nbuffer: %s proc: %s", buffer, pr);
        
        if(strcmp(buffer,pr)==0){
            return -1;
        } 
    }
    fclose(file);

    file= fopen("auxFile", "w");
    pr=fgets(buffer, bufferLength, file);
    fclose(file);
}

int sem_post(sem_t *sem) {
    int r;    
    if (!_sem_post) {
        _sem_post = dlsym(RTLD_NEXT, "sem_post");
    }

    r = _sem_post(sem);
    return(r);
} */


Node* createNode(int v/*,  Info i */) {
    Node* newNode = malloc(sizeof(Node));

    newNode->v = v;
    //newNode->info = i;
    newNode->next = NULL;

    return newNode;
}

Graph* createGraph(int n) {
    if (graphCreated != -1) {
        //printf("Graph already exists (current graph has %d vertices).\n", g->size);
        return g;   
    }

    Graph* graph = malloc(sizeof(Graph));

    graph->size = n;
    graph->list = malloc(n * sizeof(Node*));
    graph->visited = malloc(n*sizeof(int));

    for (int i=0; i < n; i++) {
        graph->list[i] = NULL;
        graph->visited[i] = 0;
    }

    graphCreated = n;
    //printf("Graph created with %d vertices.\n", n);
    return graph;
}

void addEdge(Graph *g, int src, int dest/* , Info srcInfo, Info destInfo */) {
    Node *newNode = createNode(dest/* , destInfo */); 
    newNode->next = g->list[src];
    g->list[src] = newNode;
}


void removeEdge(Graph *g, int src, int dest) {
    Node *auxNode = g->list[src];
    if (auxNode->next == NULL) {
        g->list[src] = NULL;
    } else {
        while(auxNode->next != NULL && auxNode->next->v != dest) {
            auxNode = auxNode->next;
        }
        auxNode->next = auxNode->next->next;
    }
}

void printGraph(Graph *g) {
    for (int v=0; v<g->size; v++) {
        Node* temp = g->list[v];
        while(temp) {
            printf("<%d, %d>", v, temp->v);
            temp = temp->next;
        }
        printf("\n");
    }
}

void DFS(Graph* g, int v) {
    Node* adjList = g->list[v];
    Node* temp = adjList;

    g->visited[v] = 1;

    
    while (temp != NULL) {
        int connectedV = temp->v;

        if (g->visited[connectedV] == 0) {
            DFS(g, connectedV);
        }
        else {
            printf("Ciclo encontrado em <%d, %d>.\n", v, connectedV);
            hasCycle = 1;
        }
        temp = temp->next;
    }
}

void resetVisited(Graph* g) {
    hasCycle = 0;
    for(int i=0; i<g->size; i++) {
        g->visited[i] = 0;
    }
}


void testGraph(Graph* g) {
    addEdge(g, 0, 1);
    addEdge(g, 1, 2);
    addEdge(g, 2, 3);
    addEdge(g, 3, 0);
    DFS(g, 0);
    if (hasCycle) printf("Deadlock\n");
    else printf("No deadlock\n");

    removeEdge(g, 3, 0);
    resetVisited(g);

    if (hasCycle) printf("Deadlock\n");
    else printf("No deadlock\n");
}
