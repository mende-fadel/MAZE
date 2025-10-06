#ifndef MAZE_SOLVE_H
#define MAZE_SOLVE_H

#include "maze.h"

// ============================================================================
// FILE POUR BFS
// ============================================================================

typedef struct {
    Cell *data;
    int front, rear, capacity;
} Queue;

Queue* createQueue(int capacity);
void enqueue(Queue *q, int r, int c);
Cell dequeue(Queue *q);
bool isQueueEmpty(Queue *q);
void freeQueue(Queue *q);

// ============================================================================
// RÉSOLUTION
// ============================================================================

// Calcule les distances minimales depuis la sortie vers toutes les cellules
// Retourne une matrice H×W où D[r][c] = distance minimale, ou -1 si inatteignable
int** bfsDistances(Maze *maze, int exitR, int exitC);

// Reconstruit le chemin depuis (sr,sc) vers (exitR,exitC) en suivant les distances
// Retourne un tableau de cellules et met à jour pathLength
// Retourne NULL si aucun chemin n'existe
// Reconstruit le chemin - AVEC maze en paramètre
Cell* reconstructPath(int **D, int sr, int sc, int exitR, int exitC, 
                      int H, int W, int *pathLength, Maze *maze);

// Libère la matrice de distances
void freeDistances(int **D, int H);

#endif // MAZE_SOLVE_H