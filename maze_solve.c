
#include "maze_solve.h"
#include <stdlib.h>
#include <stdio.h>

// ============================================================================
// GESTION DE LA FILE (pour BFS)
// ============================================================================

Queue* createQueue(int capacity) {
    Queue *q = (Queue*)malloc(sizeof(Queue));
    q->capacity = capacity;
    q->front = q->rear = 0;
    q->data = (Cell*)malloc(capacity * sizeof(Cell));
    return q;
}

void enqueue(Queue *q, int r, int c) {
    q->data[q->rear].r = r;
    q->data[q->rear].c = c;
    q->rear++;
}

Cell dequeue(Queue *q) {
    return q->data[q->front++];
}

bool isQueueEmpty(Queue *q) {
    return q->front >= q->rear;
}

void freeQueue(Queue *q) {
    free(q->data);
    free(q);
}

// ============================================================================
// RÉSOLUTION DU LABYRINTHE (BFS)
// ============================================================================

int** bfsDistances(Maze *maze, int exitR, int exitC) {
    int H = maze->height;
    int W = maze->width;
    
    // Allouer la matrice de distances
    int **D = (int**)malloc(H * sizeof(int*));
    for(int r = 0; r < H; r++) {
        D[r] = (int*)malloc(W * sizeof(int));
        for(int c = 0; c < W; c++) {
            D[r][c] = -1; // Non visité
        }
    }
    
    // BFS
    Queue *q = createQueue(H * W);
    D[exitR][exitC] = 0;
    enqueue(q, exitR, exitC);
    
    int directions[] = {NORTH, EAST, SOUTH, WEST};
    
    while(!isQueueEmpty(q)) {
        Cell current = dequeue(q);
        int r = current.r;
        int c = current.c;
        
        // CORRECTION: Vérifier CHAQUE direction avec canMove
        for(int i = 0; i < 4; i++) {
            int dir = directions[i];
            
            // IMPORTANT: Ne traiter que si on PEUT bouger (pas de mur)
            if(canMove(maze, r, c, dir)) {
                int nr, nc;
                getNeighbor(r, c, dir, H, W, &nr, &nc);
                
                // Si pas encore visité, marquer et ajouter à la file
                if(D[nr][nc] == -1) {
                    D[nr][nc] = D[r][c] + 1;
                    enqueue(q, nr, nc);
                }
            }
        }
    }
    
    freeQueue(q);
    return D;
}

Cell* reconstructPath(int **D, int sr, int sc, int exitR, int exitC, 
                      int H, int W, int *pathLength, Maze *maze) {
    if(D[sr][sc] == -1) {
        *pathLength = 0;
        return NULL;
    }
    
    int len = D[sr][sc] + 1;
    Cell *path = (Cell*)malloc(len * sizeof(Cell));
    
    int r = sr, c = sc;
    int idx = 0;
    path[idx++] = (Cell){r, c};
    
    int directions[] = {NORTH, EAST, SOUTH, WEST};
    
    while(r != exitR || c != exitC) {
        bool moved = false;
        
        for(int i = 0; i < 4; i++) {
            int dir = directions[i];
            int nr, nc;
            
            // Obtenir le voisin
            if(getNeighbor(r, c, dir, H, W, &nr, &nc)) {
                // TRIPLE VÉRIFICATION:
                // 1. Distance valide
                // 2. Distance décroissante (D[nr][nc] == D[r][c] - 1)
                // 3. PAS DE MUR (canMove)
                if(D[nr][nc] >= 0 && 
                   D[nr][nc] == D[r][c] - 1 && 
                   canMove(maze, r, c, dir)) {
                    
                    r = nr;
                    c = nc;
                    path[idx++] = (Cell){r, c};
                    moved = true;
                    break;
                }
            }
        }
        
        // Sécurité contre les boucles infinies
        if(!moved) {
            fprintf(stderr, "ERREUR reconstructPath: Bloqué à (%d,%d)\n", r, c);
            fprintf(stderr, "  Distance actuelle: %d\n", D[r][c]);
            fprintf(stderr, "  Voisins accessibles:\n");
            for(int i = 0; i < 4; i++) {
                int dir = directions[i];
                int nr, nc;
                if(getNeighbor(r, c, dir, H, W, &nr, &nc)) {
                    fprintf(stderr, "    (%d,%d): D=%d, canMove=%d\n",
                           nr, nc, D[nr][nc], canMove(maze, r, c, dir));
                }
            }
            free(path);
            *pathLength = 0;
            return NULL;
        }
    }
    
    *pathLength = idx;
    return path;
}

void freeDistances(int **D, int H) {
    for(int r = 0; r < H; r++) {
        free(D[r]);
    }
    free(D);
}