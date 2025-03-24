#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define MAX 100

typedef struct {
    int vertices[MAX];
    int length;
    int totalWeight;
} Path;

int adjMatrix[MAX][MAX];
int visited[MAX];
Path paths[MAX];
int pathCount = 0;
int n;

void copyPath(Path *dest, Path *src) {
    dest->length = src->length;
    dest->totalWeight = src->totalWeight;
    memcpy(dest->vertices, src->vertices, src->length * sizeof(int));
}

// DFS to find node-disjoint paths
void dfs(int current, int destination, Path *currentPath) {
    if (current == destination) {
        copyPath(&paths[pathCount++], currentPath);
        return;
    }

    visited[current] = 1;  // Mark node as visited

    for (int i = 0; i < n; i++) {
        if (!visited[i] && adjMatrix[current][i] > 0) {
            currentPath->vertices[currentPath->length++] = i;
            currentPath->totalWeight += adjMatrix[current][i];

            dfs(i, destination, currentPath);

            // Backtrack
            currentPath->length--;
            currentPath->totalWeight -= adjMatrix[current][i];
        }
    }

    visited[current] = 0;  // Unmark for other paths
}

// Sort paths by weight (shortest first)
int comparePath(const void *a, const void *b) {
    Path *pathA = (Path *)a;
    Path *pathB = (Path *)b;
    return pathA->totalWeight - pathB->totalWeight;
}

// Print a path
void printPath(Path *path) {
    for (int i = 0; i < path->length; i++) {
        if (i > 0) printf(" -> ");
        printf("%d", path->vertices[i]);
    }
    printf(" (Weight: %d)\n", path->totalWeight);
}

int main() {
    int edgesCount, src, des, weight;
    int start, end, k;

    printf("\nEnter the number of vertices: ");
    scanf("%d", &n);

    printf("\nEnter the number of edges: ");
    scanf("%d", &edgesCount);

    memset(adjMatrix, 0, sizeof(adjMatrix)); // No edge means 0

    // Input edges
    printf("Enter edges (src, dest, weight):\n");
    for (int i = 0; i < edgesCount; i++) {
        scanf("%d %d %d", &src, &des, &weight);
        adjMatrix[src][des] = weight;
        adjMatrix[des][src] = weight;  // Undirected graph
    }

    // Input source and destination
    printf("\nEnter source and destination: ");
    scanf("%d %d", &start, &end);

    Path currentPath = {.length = 0, .totalWeight = 0};
    currentPath.vertices[currentPath.length++] = start;

    memset(visited, 0, sizeof(visited));
    dfs(start, end, &currentPath);

    // Sort paths by weight
    qsort(paths, pathCount, sizeof(Path), comparePath);

    // Print all node-disjoint paths
    printf("\nNode-disjoint paths from %d to %d:\n", start, end);
    int disjointCount = 0;
    memset(visited, 0, sizeof(visited));  // Reset visited for final selection

    for (int i = 0; i < pathCount; i++) {
        int isDisjoint = 1;

        // Check if the path shares nodes with previously selected paths
        for (int j = 1; j < paths[i].length - 1; j++) {
            if (visited[paths[i].vertices[j]]) {
                isDisjoint = 0;
                break;
            }
        }

        if (isDisjoint) {
            disjointCount++;
            printf("Path %d: ", disjointCount);
            printPath(&paths[i]);

            // Mark nodes in this path as used6
            
            for (int j = 1; j < paths[i].length - 1; j++) {
                visited[paths[i].vertices[j]] = 1;
            }
        }
    }

    if (disjointCount == 0) {
        printf("No node-disjoint paths found.\n");
    }

    return 0;
}
