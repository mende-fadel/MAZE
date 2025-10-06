#include "maze.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// FONCTIONS UTILITAIRES
// ============================================================================

int oppositeDir(int dir) {
    switch(dir) {
        case NORTH: return SOUTH;
        case SOUTH: return NORTH;
        case EAST: return WEST;
        case WEST: return EAST;
        default: return 0;
    }
}

bool getNeighbor(int r, int c, int dir, int H, int W, int *nr, int *nc) {
    *nr = r;
    *nc = c;
    switch(dir) {
        case NORTH: (*nr)--; break;
        case SOUTH: (*nr)++; break;
        case EAST: (*nc)++; break;
        case WEST: (*nc)--; break;
        default: return false;
    }
    return (*nr >= 0 && *nr < H && *nc >= 0 && *nc < W);
}

// ============================================================================
// OPÉRATIONS DE BASE
// ============================================================================

Maze* init(int H, int W, unsigned int seed) {
    Maze *maze = (Maze*)malloc(sizeof(Maze));
    maze->height = H;
    maze->width = W;
    maze->seed = seed;
    srand(seed);
    
    // Allouer la grille
    maze->walls = (unsigned char**)malloc(H * sizeof(unsigned char*));
    for(int r = 0; r < H; r++) {
        maze->walls[r] = (unsigned char*)calloc(W, sizeof(unsigned char));
    }
    
    // Fermer tous les bords extérieurs
    for(int r = 0; r < H; r++) {
        for(int c = 0; c < W; c++) {
            if(r == 0) maze->walls[r][c] |= NORTH;
            if(r == H-1) maze->walls[r][c] |= SOUTH;
            if(c == 0) maze->walls[r][c] |= WEST;
            if(c == W-1) maze->walls[r][c] |= EAST;
        }
    }
    
    return maze;
}

bool hasWall(Maze *maze, int r, int c, int dir) {
    return (maze->walls[r][c] & dir) != 0;
}

void setWall(Maze *maze, int r, int c, int dir, bool present) {
    if(present) {
        maze->walls[r][c] |= dir;
    } else {
        maze->walls[r][c] &= ~dir;
    }
}

void link(Maze *maze, int r1, int c1, int r2, int c2, bool open) {
    // Déterminer la direction
    int dir = 0;
    if(r2 == r1 - 1) dir = NORTH;
    else if(r2 == r1 + 1) dir = SOUTH;
    else if(c2 == c1 + 1) dir = EAST;
    else if(c2 == c1 - 1) dir = WEST;
    
    if(dir == 0) return; // Pas adjacentes
    
    // Mettre à jour symétriquement
    setWall(maze, r1, c1, dir, !open);
    setWall(maze, r2, c2, oppositeDir(dir), !open);
}

bool canMove(Maze *maze, int r, int c, int dir) {
    if(hasWall(maze, r, c, dir)) return false;
    
    int nr, nc;
    if(!getNeighbor(r, c, dir, maze->height, maze->width, &nr, &nc)) {
        return false;
    }
    
    return !hasWall(maze, nr, nc, oppositeDir(dir));
}

void freeMaze(Maze *maze) {
    for(int r = 0; r < maze->height; r++) {
        free(maze->walls[r]);
    }
    free(maze->walls);
    free(maze);
}

// ============================================================================
// GÉNÉRATION (Division récursive)
// ============================================================================

void divide(Maze *maze, int r0, int c0, int r1, int c1) {
    int h = r1 - r0;
    int w = c1 - c0;
    
    // Cas de base
    if(h <= 1 && w <= 1) return;
    
    bool horizontal = (h > w);
    
    if(horizontal) {
        // Diviser horizontalement
        if(h <= 1) return;
        
        // Choisir une ligne pour le mur (entre r0 et r1-1)
        int wallRow = r0 + (rand() % (h - 1));
        
        // Placer un mur horizontal complet
        for(int c = c0; c < c1; c++) {
            link(maze, wallRow, c, wallRow + 1, c, false);
        }
        
        // Percer exactement une porte
        int doorCol = c0 + (rand() % w);
        link(maze, wallRow, doorCol, wallRow + 1, doorCol, true);
        
        // Récursion sur les deux parties
        divide(maze, r0, c0, wallRow + 1, c1);
        divide(maze, wallRow + 1, c0, r1, c1);
        
    } else {
        // Diviser verticalement
        if(w <= 1) return;
        
        // Choisir une colonne pour le mur
        int wallCol = c0 + (rand() % (w - 1));
        
        // Placer un mur vertical complet
        for(int r = r0; r < r1; r++) {
            link(maze, r, wallCol, r, wallCol + 1, false);
        }
        
        // Percer exactement une porte
        int doorRow = r0 + (rand() % h);
        link(maze, doorRow, wallCol, doorRow, wallCol + 1, true);
        
        // Récursion sur les deux parties
        divide(maze, r0, c0, r1, wallCol + 1);
        divide(maze, r0, wallCol + 1, r1, c1);
    }
}

// ============================================================================
// AFFICHAGE
// ============================================================================

void printMaze(Maze *maze) {
    int H = maze->height;
    int W = maze->width;
    
    // Ligne du haut
    for(int c = 0; c < W; c++) {
        printf("+---");
    }
    printf("+\n");
    
    // Pour chaque ligne
    for(int r = 0; r < H; r++) {
        // Murs verticaux et cellules
        for(int c = 0; c < W; c++) {
            printf("%s   ", hasWall(maze, r, c, WEST) ? "|" : " ");
        }
        printf("%s\n", hasWall(maze, r, W-1, EAST) ? "|" : " ");
        
        // Murs horizontaux
        for(int c = 0; c < W; c++) {
            printf("+%s", hasWall(maze, r, c, SOUTH) ? "---" : "   ");
        }
        printf("+\n");
    }
}


void printMazeWithPath(Maze *maze, Cell *path, int pathLength) {
    int H = maze->height;
    int W = maze->width;
    
    // Créer une marque pour le chemin
    bool **onPath = (bool**)calloc(H, sizeof(bool*));
    for(int r = 0; r < H; r++) {
        onPath[r] = (bool*)calloc(W, sizeof(bool));
    }
    for(int i = 0; i < pathLength; i++) {
        onPath[path[i].r][path[i].c] = true;
    }
    
    // Ligne du haut
    for(int c = 0; c < W; c++) {
        printf("+---");
    }
    printf("+\n");
    
    // Pour chaque ligne
    for(int r = 0; r < H; r++) {
        // Murs verticaux et cellules
        for(int c = 0; c < W; c++) {
            // Mur ouest
            printf("%s", hasWall(maze, r, c, WEST) ? "|" : " ");
            
            // Contenu de la cellule - CENTRÉ sur 3 caractères
            if(onPath[r][c]) {
                printf(" • ");  // Utiliser • pour plus de visibilité
            } else {
                printf("   ");
            }
        }
        // Mur est de la dernière colonne
        printf("%s\n", hasWall(maze, r, W-1, EAST) ? "|" : " ");
        
        // Murs horizontaux
        for(int c = 0; c < W; c++) {
            printf("+%s", hasWall(maze, r, c, SOUTH) ? "---" : "   ");
        }
        printf("+\n");
    }
    
    // Libérer
    for(int r = 0; r < H; r++) {
        free(onPath[r]);
    }
    free(onPath);
}

void printMazeWithPathNumbers(Maze *maze, Cell *path, int pathLength) {
    int H = maze->height;
    int W = maze->width;
    
    // Créer une carte avec les numéros d'étape
    int **stepNumber = (int**)malloc(H * sizeof(int*));
    for(int r = 0; r < H; r++) {
        stepNumber[r] = (int*)malloc(W * sizeof(int));
        for(int c = 0; c < W; c++) {
            stepNumber[r][c] = -1;  // Pas sur le chemin
        }
    }
    
    for(int i = 0; i < pathLength; i++) {
        int r = path[i].r;
        int c = path[i].c;
        stepNumber[r][c] = i;
    }
    
    // Ligne du haut
    for(int c = 0; c < W; c++) {
        printf("+---");
    }
    printf("+\n");
    
    // Pour chaque ligne
    for(int r = 0; r < H; r++) {
        for(int c = 0; c < W; c++) {
            printf("%s", hasWall(maze, r, c, WEST) ? "|" : " ");
            
            if(stepNumber[r][c] >= 0) {
                // Afficher le numéro modulo 100 sur 3 caractères
                printf("%3d", stepNumber[r][c] % 100);
            } else {
                printf("   ");
            }
        }
        printf("%s\n", hasWall(maze, r, W-1, EAST) ? "|" : " ");
        
        // Murs horizontaux
        for(int c = 0; c < W; c++) {
            printf("+%s", hasWall(maze, r, c, SOUTH) ? "---" : "   ");
        }
        printf("+\n");
    }
    
    // Libérer
    for(int r = 0; r < H; r++) {
        free(stepNumber[r]);
    }
    free(stepNumber);
}