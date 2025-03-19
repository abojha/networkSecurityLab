#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>


#define N 10
#define P 30
#define INF 1000000

void copyGraph(int src[N][N], int dest[N][N]){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            dest[i][j] = src[i][j];
        }
    }
}

void floydWarshall(int graph[N][N], int dist[N][N]){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            dist[i][j] = (i == j) ? 0 : (graph[i][j] ? 1 : INF);
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




int main(){

    int graph[N][N] = {0}, attack[N][N] = {0};
    int degree[N] = {0}, attack_degree[N] = {0};
    int dist[N][N], attack_dist[N][N];
    int worm_u = -1, worm_v = -1;


    srand(time(NULL));
    // make a random graph
    for(int i = 0; i < N; i++){
        for(int j = i + 1; j < N; j++){
            if(rand() % 100 < P){
                graph[i][j] = graph[j][i] = 1;
            }
        }
    }

    copyGraph(graph, attack);


    // inject warmhole
    for(int i = 0; i < N && worm_u == -1; i++){
        for(int j = i + 1; j < N; j++){
            if(graph[i][j] == 0){
                attack[i][j] = 1;
                attack[j][i] = 1;
                worm_u = i;
                worm_v = j;
                break;
            }
        }
    }


    if(worm_u != -1 && worm_v != -1){
        printf("\nWormhole added %d %d\n", worm_u, worm_v);
    }
    else{
        printf("No way to add warmhole");
    }

    // calculate Node degree for both the graphs
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            degree[i] += graph[i][j];
            attack_degree[i] += attack[i][j];
        }
        printf("Node = %d  \t Node Degree = %d \t Node Degree (Attack) = %d\n", i, degree[i], attack_degree[i]);
    }


    // calculating the shortest path
    floydWarshall(graph, dist);
    floydWarshall(attack, attack_dist);


    if(worm_u != -1 && dist[worm_u][worm_v] > 1 &&  attack_dist[worm_u][worm_v] == 1){
        printf("WormHole attack between %d and %d", worm_u, worm_v);
    }
    else{
        printf("No wormhole attack");
    }

    return 0;
}