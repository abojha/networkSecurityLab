#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX 100

typedef struct{
    int src, des, weight;
} Edge;

typedef struct{
    int vertices[MAX];
    int pathlength;
    int totalWeight;
} Path;

Edge edges[MAX];
int edgeCount = 0;
Path paths[MAX];
int pathCount = 0;

int adjMatrix[MAX][MAX];
int visited[MAX];
int n;

void addEdge(int src, int dest, int weight){
    adjMatrix[src][dest] = weight;
    adjMatrix[dest][src] = weight;
    edges[edgeCount++] = (Edge){src, dest, weight};
}

void copyPath(Path *dest, Path *src){
    dest->pathlength = src->pathlength;
    dest->totalWeight = src->totalWeight;
    memcpy(dest->vertices, src->vertices, src->pathlength * sizeof(int));
}

void dfs(int current, int destination, Path *currentPath){
    if(current == destination){
        copyPath(&paths[pathCount++], currentPath);
        return;
    }

    visited[current] = 1;

    for(int i = 0; i < n; i++){
        if(!visited[i] && adjMatrix[current][i] > 0){
            currentPath->vertices[currentPath->pathlength++] = i;
            currentPath->totalWeight += adjMatrix[current][i];

            dfs(i, destination, currentPath);

            currentPath->pathlength--;
            currentPath->totalWeight -= adjMatrix[current][i];

        }
    }
    visited[current]=0;
}

int comparePath(const void  *a, const void *b){
    Path *PathA = (Path *)a;
    Path *PathB = (Path *)b;

    return PathA->totalWeight - PathB->totalWeight;
}

void printPath(Path *path){
    for(int i = 0; i < path->pathlength; i++){
        printf("%d ->", path->vertices[i]);
    }

    printf("(Weights :: %d)", path->totalWeight);
}

int main(){
    int src, dest, weight;
    int start, end, k;
    int edgeCounts = 0;

    printf("Enter the number of vertices & Edges\n");
    scanf("%d %d", &n, &edgeCounts);

    memset(adjMatrix, 0, sizeof(adjMatrix));

    // Input all the src, dest, weight of graph
    printf("Enter the (src, dest, weights)\n");
    for(int i = 0; i < edgeCounts; i++){
        scanf("%d %d %d", &src, &dest, &weight);
        addEdge(src, dest, weight);
    }

    // Input source, destination and k
    printf("\nEnter the source, destination and K\n");
    scanf("%d %d %d", &start, &end, &k);

    Path currentPath = {.pathlength = 0, .totalWeight = 0};
    currentPath.vertices[currentPath.pathlength++] = 0;

    memset(visited, 0, sizeof(visited));

    dfs(start, end, &currentPath);

    qsort(paths, pathCount, sizeof(Path), comparePath);

    // Print all disjoint path
    int disJoint = 0;
    memset(visited, 0, sizeof(visited));
    for(int i = 0; i < pathCount; i++){
        int isDisjoint = 1;
        
        for(int j = 1; j < paths[i].pathlength - 1; i++){
            if(visited[paths[i].vertices[j]]){
                isDisjoint = 0;
                break;
            }
        }

        if(isDisjoint){
            disJoint++;
            printPath(&paths[i]);
        }

        for(int j = 1; j < paths[i].pathlength - 1; i++){
            visited[paths[i].vertices[j]] = 1;
        }

    }

    return 0;
}