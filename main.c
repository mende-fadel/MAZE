#include "maze.h"
#include "maze_solve.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ============================================================================
// TESTS
// ============================================================================

void testConnectivity(Maze *maze, int exitR, int exitC) {
    printf("\n=== Test de connectivité ===\n");
    int **D = bfsDistances(maze, exitR, exitC);
    
    int unreachable = 0;
    for(int r = 0; r < maze->height; r++) {
        for(int c = 0; c < maze->width; c++) {
            if(D[r][c] == -1) unreachable++;
        }
    }
    
    if(unreachable == 0) {
        printf("✓ Toutes les cellules sont accessibles\n");
    } else {
        printf("✗ %d cellules inaccessibles\n", unreachable);
    }
    
    freeDistances(D, maze->height);
}

void testAcyclicity(Maze *maze) {
    printf("\n=== Test d'acyclicité ===\n");
    int edges = 0;
    
    // Compter les passages ouverts (sans doubler)
    for(int r = 0; r < maze->height; r++) {
        for(int c = 0; c < maze->width; c++) {
            if(!hasWall(maze, r, c, EAST) && c < maze->width - 1) edges++;
            if(!hasWall(maze, r, c, SOUTH) && r < maze->height - 1) edges++;
        }
    }
    
    int expected = maze->height * maze->width - 1;
    if(edges == expected) {
        printf("✓ Labyrinthe parfait: %d arêtes (attendu: %d)\n", edges, expected);
    } else {
        printf("✗ Nombre d'arêtes incorrect: %d (attendu: %d)\n", edges, expected);
    }
}

void testSymmetry(Maze *maze) {
    printf("\n=== Test de symétrie des murs ===\n");
    int errors = 0;
    
    for(int r = 0; r < maze->height; r++) {
        for(int c = 0; c < maze->width; c++) {
            // Vérifier chaque direction
            int directions[] = {NORTH, EAST, SOUTH, WEST};
            for(int i = 0; i < 4; i++) {
                int dir = directions[i];
                int nr, nc;
                if(getNeighbor(r, c, dir, maze->height, maze->width, &nr, &nc)) {
                    bool wall1 = hasWall(maze, r, c, dir);
                    bool wall2 = hasWall(maze, nr, nc, oppositeDir(dir));
                    if(wall1 != wall2) {
                        errors++;
                    }
                }
            }
        }
    }
    
    if(errors == 0) {
        printf("✓ Tous les murs sont symétriques\n");
    } else {
        printf("✗ %d erreurs de symétrie détectées\n", errors);
    }
}

void testReproducibility() {
    printf("\n=== Test de reproductibilité ===\n");
    unsigned int seed = 12345;
    
    Maze *maze1 = init(5, 5, seed);
    divide(maze1, 0, 0, 5, 5);
    
    Maze *maze2 = init(5, 5, seed);
    divide(maze2, 0, 0, 5, 5);
    
    bool identical = true;
    for(int r = 0; r < 5; r++) {
        for(int c = 0; c < 5; c++) {
            if(maze1->walls[r][c] != maze2->walls[r][c]) {
                identical = false;
                break;
            }
        }
        if(!identical) break;
    }
    
    if(identical) {
        printf("✓ Même seed produit le même labyrinthe\n");
    } else {
        printf("✗ Les labyrinthes diffèrent malgré le même seed\n");
    }
    
    freeMaze(maze1);
    freeMaze(maze2);
}

void debugPath(Cell *path, int pathLength) {
    printf("\nChemins détaillé (%d cellules):\n", pathLength);
    for(int i = 0; i < pathLength; i++) {
        printf("  %d: (%d, %d)\n", i, path[i].r, path[i].c);
    }
    printf("\n");
}

// ============================================================================
// FONCTION PRINCIPALE
// ============================================================================

int main() {
    printf("╔══════════════════════════════════════════════════╗\n");
    printf("║  GÉNÉRATEUR ET SOLVEUR DE LABYRINTHE            ║\n");
    printf("║  L3 Informatique - PSA 2025-2026                ║\n");
    printf("╚══════════════════════════════════════════════════╝\n\n");
    
    int H = 10, W = 15;
    unsigned int seed = (unsigned int)time(NULL);
    
    printf("Configuration:\n");
    printf("  - Dimensions: %d × %d\n", H, W);
    printf("  - Seed: %u\n\n", seed);
    
    // Création et génération
    printf("Création du labyrinthe...\n");
    Maze *maze = init(H, W, seed);
    
    printf("Génération par division récursive...\n");
    divide(maze, 0, 0, H, W);
    
    printf("\n");
    printMaze(maze);
    
    // Tests
    printf("\n╔══════════════════════════════════════════════════╗\n");
    printf("║  TESTS DE VALIDATION                            ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");
    
    testSymmetry(maze);
    testConnectivity(maze, 0, 0);
    testAcyclicity(maze);
    testReproducibility();
    
    // Avant la section "Résolution", ajouter ce test:

    printf("\n╔══════════════════════════════════════════════════╗\n");
    printf("║  DIAGNOSTIC DES MURS                            ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");

    // Résolution
    printf("\n╔══════════════════════════════════════════════════╗\n");
    printf("║  RÉSOLUTION DU LABYRINTHE                       ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");
    
    int exitR = H - 1, exitC = W - 1;
    int startR = 0, startC = 0;
    
    printf("\nChemin de (%d,%d) vers (%d,%d):\n", startR, startC, exitR, exitC);
    
    int **D = bfsDistances(maze, exitR, exitC);
    
    int pathLength;
    Cell *path = reconstructPath(D, startR, startC, exitR, exitC, 
                             H, W, &pathLength, maze); 
    
    if(path) {
        printf("✓ Chemin trouvé! Longueur: %d pas\n\n", pathLength);
        debugPath(path, pathLength);
        printMazeWithPath(maze, path, pathLength);
        free(path);
    } else {
        printf("✗ Aucun chemin trouvé!\n");
    }
    
    // Libération
    freeDistances(D, H);
    freeMaze(maze);
    
    printf("\n╔══════════════════════════════════════════════════╗\n");
    printf("║  Terminé avec succès!                           ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");
    
    return 0;
}