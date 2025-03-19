#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 10         // Number of nodes
#define P 30         // Edge probability in percentage
#define INF 1000000  // A large number representing no connection

// Copies a source graph (adjacency matrix) to a destination graph.
void copyGraph(int src[N][N], int dest[N][N]){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            dest[i][j] = src[i][j];
        }
    }
}

// Performs the Floyd–Warshall algorithm using weighted edges.
void floydWarshall(int graph[N][N], int dist[N][N]){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            // If i == j, distance is 0. If there's an edge, use its weight; otherwise, INF.
            dist[i][j] = (i == j) ? 0 : (graph[i][j] ? graph[i][j] : INF);
        }
    }
    
    for(int k = 0; k < N; k++){
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                if(dist[i][k] + dist[k][j] < dist[i][j]){
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }
}

// Computes the graph's diameter (i.e. the maximum finite shortest path).
int computeDiameter(int dist[N][N]){
    int diameter = 0;
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            if(dist[i][j] < INF && dist[i][j] > diameter){
                diameter = dist[i][j];
            }
        }
    }
    return diameter;
}

int main(){
    int graph[N][N] = {0}, attack[N][N] = {0};
    int dist[N][N], attack_dist[N][N];
    int worm_u = -1, worm_v = -1;

    srand(time(NULL));

    // Generate a random weighted undirected graph.
    // If an edge exists between nodes i and j, assign a random weight (1 to 10).
    for(int i = 0; i < N; i++){
        for(int j = i + 1; j < N; j++){
            if(rand() % 100 < P){
                int weight = (rand() % 10) + 1;  // weight between 1 and 10
                graph[i][j] = graph[j][i] = weight;
            }
        }
    }
    
    // Copy the original graph to the attacked graph.
    copyGraph(graph, attack);

    // Inject wormhole: find the first pair (i, j) with no edge, and add an edge with weight 1.
    for(int i = 0; i < N && worm_u == -1; i++){
        for(int j = i + 1; j < N; j++){
            if(graph[i][j] == 0){
                attack[i][j] = attack[j][i] = 1; // Low weight wormhole edge.
                worm_u = i;
                worm_v = j;
                break;
            }
        }
    }
    
    if(worm_u != -1 && worm_v != -1){
        printf("\nWormhole added between nodes %d and %d\n", worm_u, worm_v);
    }
    else{
        printf("No way to add wormhole.\n");
    }
    
    // Calculate the shortest paths using the Floyd–Warshall algorithm.
    floydWarshall(graph, dist);
    floydWarshall(attack, attack_dist);
    
    // Compute the diameters for both graphs.
    int graph_diameter = computeDiameter(dist);
    int attack_diameter = computeDiameter(attack_dist);
    
    printf("\nOriginal Graph Diameter = %d \t After Attack Graph Diameter = %d\n", graph_diameter, attack_diameter);
    
    // If the attacked graph has a smaller diameter, it indicates that the wormhole created a shortcut.
    if(graph_diameter > attack_diameter){
        printf("\nWormhole detected based on diameter reduction.\n");
    } else {
        printf("\nWormhole not detected based on diameter reduction.\n");
    }
    
    return 0;
}
