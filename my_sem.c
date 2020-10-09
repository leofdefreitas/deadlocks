#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define GRAPH_SIZE 4

int (*_sem_wait)(sem_t *) = NULL;
int (*_sem_post)(sem_t *) = NULL;

typedef union {
    pthread_t pid;
    sem_t* resource;
} Info; 

/**
 * Controla se grafo ja foi criado.
 * Se value == -1, o grafo nao foi criado.
 * Se nao, value é igual ao tamanho do grafo (quantidade de vertices)
 */ 
int graphCreated = -1; 
Info knownVertex[GRAPH_SIZE];
int numberOfKnownVertex = 0;

typedef struct node {
    int v;
    int type; 
    struct node* next;
} Node;

Node* createNode(int v);

typedef struct graph {
    int size;
    int* visited;
    Node** list;
} Graph;


Graph* createGraph(int n);

void addEdge(Graph *g, int src, int dest);

void removeEdge(Graph *g, int src, int dest);

void printGraph(Graph *g);

int DFS(Graph* g, int v);

void resetVisited(Graph* g);

void testGraph(Graph* g);

Graph* g;


/**
 * Recebe: 
 *      sem_t sem = semaforo (recurso) a ser procurado no vetor knownVertex
 *      pthread_t t = thread (processo) a ser procurada no vetor knownVertex
 * Retorna vetor onde:
 *     positions[0] = index do semaforo (recurso) no vetor knownVertex. Se valor == -1, semaforo nao foi encontrado em knownVertex
 *     positions[1] = index da thread (processo) no vetor knownVertex. Se valor == -1, thread nao foi encontrada em knownVertex
 */
void getVertexIndexes(sem_t *sem, pthread_t t, int positions[]) {
    for (int i=0; i<numberOfKnownVertex; i++) {
        if (knownVertex[i].resource == sem) {
            positions[0] = i;
        } else if (knownVertex[i].pid == t) {
            positions[1] = i;
        }
    }
}

int sem_wait(sem_t *sem) {

    if (!_sem_wait) {
        _sem_wait = dlsym(RTLD_NEXT, "sem_wait");
    } 

    g = createGraph(GRAPH_SIZE);
    pthread_t aux_t = pthread_self();

    int positions[2] = {-1, -1};
    getVertexIndexes(sem, aux_t, positions);
    int posSem = positions[0];
    int posThread = positions[1];

    /**
     * Se getVertexIndexes não encontrou o processo ou o semaforo
     *      salva no vetor knownVertex 
     */
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

    /**
     * Cria aresta processo -> recurso (processo tem interesse em utilizar recurso)
     * Roda DFS para detectar deadlock. 
     * Se encontrar deadlock, retorna erro
     * Se nao:
     *    - Chama sem_wait() original. Após ser liberado:
     *         - Remove aresta processo -> recurso
     *         - Cria aresta recurso -> processo (recurso alocado para o processo)
     */ 
    addEdge(g, posThread, posSem);
    if (DFS(g, posThread) == -1) {
        removeEdge(g, posThread, posSem);
        return -2;
    }

    int r = _sem_wait(sem);
    removeEdge(g, posThread, posSem);
    addEdge(g, posSem, posThread);
    resetVisited(g);

    return r;
}

/**
 * Remove aresta recurso->processo (libera recurso)
 * Chama sem_post() original
 */

int sem_post(sem_t *sem) {

    if (!_sem_post) {
        _sem_post = dlsym(RTLD_NEXT, "sem_post");
    }
    int r; 

    pthread_t aux_t = pthread_self();   
    int positions[2];
    getVertexIndexes(sem, aux_t, positions);
    removeEdge(g, positions[0], positions[1]);

    r = _sem_post(sem);
    return(r);
} 


Node* createNode(int v) {
    Node* newNode = malloc(sizeof(Node));

    newNode->v = v;
    newNode->next = NULL;

    return newNode;
}

Graph* createGraph(int n) {
    if (graphCreated != -1) {
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
    return graph;
}

void addEdge(Graph *g, int src, int dest) {

    Node *newNode = createNode(dest); 
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
    printf("Imprimindo Grafo. \n");
    fflush(stdout);
    for (int v=0; v<g->size; v++) {
        Node* temp = g->list[v];
        printf("%d: [ ", v);
        fflush(stdout);
        while(temp) {
            printf("%d", temp->v);
            fflush(stdout);
            if (temp->next != NULL) printf(", ");
            temp = temp->next;
        }
        printf(" ]\n");
        fflush(stdout);
    }
    printf("-------------------------\n");
    fflush(stdout);
}

int DFS(Graph* g, int v) {
    Node* adjList = g->list[v];
    Node* temp = adjList;

    g->visited[v] = 1;

    
    while (temp != NULL) {
        int connectedV = temp->v;

        if (g->visited[connectedV] == 0) 
            return DFS(g, connectedV);
        else 
            return -1; //Deadlock
        temp = temp->next;
    }
    return 1;
}

void resetVisited(Graph* g) {
    for(int i=0; i<g->size; i++) {
        g->visited[i] = 0;
    }
}
