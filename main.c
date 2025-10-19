#include "maze.h"
#include "maze_solve.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ============================================================================
// TESTS UNITAIRES (Fadel Mukoka)
// ============================================================================

void testCanMove() {
    printf("\n=== Test Unitaire: canMove() ===\n");
    
    // Créer un petit labyrinthe 3x3
    Maze *maze = init(3, 3, 12345);
    
    // Test 1: Deux cellules reliées (ouvertes)
    link(maze, 0, 0, 0, 1, true);  // Ouvrir le passage entre (0,0) et (0,1)
    
    if(canMove(maze, 0, 0, EAST)) {
        printf("✓ Test 1 réussi: Peut se déplacer vers l'EST quand le passage est ouvert\n");
    } else {
        printf("✗ Test 1 échoué: Devrait pouvoir se déplacer vers l'EST\n");
    }
    
    // Test 2: Deux cellules avec mur (fermées)
    link(maze, 1, 0, 1, 1, false);  // Fermer le passage entre (1,0) et (1,1)
    
    if(!canMove(maze, 1, 0, EAST)) {
        printf("✓ Test 2 réussi: Ne peut pas se déplacer vers l'EST quand le mur est fermé\n");
    } else {
        printf("✗ Test 2 échoué: Ne devrait pas pouvoir se déplacer vers l'EST\n");
    }
    
    // Test 3: Vérifier symétrie du mouvement
    if(canMove(maze, 0, 1, WEST)) {
        printf("✓ Test 3 réussi: Symétrie vérifiée - peut revenir en arrière\n");
    } else {
        printf("✗ Test 3 échoué: La symétrie n'est pas respectée\n");
    }
    
    freeMaze(maze);
}

void testGetNeighbor() {
    printf("\n=== Test Unitaire: getNeighbor() ===\n");
    
    int nr, nc;
    int H = 5, W = 5;
    
    // Test 1: Déplacement vers l'EST
    if(getNeighbor(2, 3, EAST, H, W, &nr, &nc)) {
        if(nr == 2 && nc == 4) {
            printf("✓ Test 1 réussi: (2,3) + EAST = (2,4)\n");
        } else {
            printf("✗ Test 1 échoué: Attendu (2,4), obtenu (%d,%d)\n", nr, nc);
        }
    }
    
    // Test 2: Déplacement vers le NORD
    if(getNeighbor(2, 3, NORTH, H, W, &nr, &nc)) {
        if(nr == 1 && nc == 3) {
            printf("✓ Test 2 réussi: (2,3) + NORTH = (1,3)\n");
        } else {
            printf("✗ Test 2 échoué: Attendu (1,3), obtenu (%d,%d)\n", nr, nc);
        }
    }
    
    // Test 3: Déplacement vers le SUD
    if(getNeighbor(2, 3, SOUTH, H, W, &nr, &nc)) {
        if(nr == 3 && nc == 3) {
            printf("✓ Test 3 réussi: (2,3) + SOUTH = (3,3)\n");
        } else {
            printf("✗ Test 3 échoué: Attendu (3,3), obtenu (%d,%d)\n", nr, nc);
        }
    }
    
    // Test 4: Déplacement vers l'OUEST
    if(getNeighbor(2, 3, WEST, H, W, &nr, &nc)) {
        if(nr == 2 && nc == 2) {
            printf("✓ Test 4 réussi: (2,3) + WEST = (2,2)\n");
        } else {
            printf("✗ Test 4 échoué: Attendu (2,2), obtenu (%d,%d)\n", nr, nc);
        }
    }
    
    // Test 5: Déplacement hors limites (coin supérieur gauche vers NORD)
    if(!getNeighbor(0, 0, NORTH, H, W, &nr, &nc)) {
        printf("✓ Test 5 réussi: Détection correcte des limites (hors grille)\n");
    } else {
        printf("✗ Test 5 échoué: Devrait détecter que le voisin est hors limites\n");
    }
}

void testHasWallSetWall() {
    printf("\n=== Test Unitaire: hasWall() / setWall() ===\n");
    
    Maze *maze = init(3, 3, 12345);
    
    // Test 1: État initial (tous les bords extérieurs fermés)
    if(hasWall(maze, 0, 0, NORTH)) {
        printf("✓ Test 1 réussi: Mur NORD sur le bord supérieur détecté\n");
    } else {
        printf("✗ Test 1 échoué: Le mur NORD devrait exister\n");
    }
    
    // Test 2: Ajouter un mur puis vérifier
    setWall(maze, 1, 1, EAST, true);
    if(hasWall(maze, 1, 1, EAST)) {
        printf("✓ Test 2 réussi: Mur EAST ajouté et détecté\n");
    } else {
        printf("✗ Test 2 échoué: Le mur EAST devrait exister après setWall\n");
    }
    
    // Test 3: Retirer le mur et vérifier la restauration
    setWall(maze, 1, 1, EAST, false);
    if(!hasWall(maze, 1, 1, EAST)) {
        printf("✓ Test 3 réussi: Mur EAST retiré, état restauré\n");
    } else {
        printf("✗ Test 3 échoué: Le mur EAST devrait être absent après retrait\n");
    }
    
    // Test 4: Vérifier que les autres directions ne sont pas affectées
    setWall(maze, 1, 1, SOUTH, true);
    if(hasWall(maze, 1, 1, SOUTH) && !hasWall(maze, 1, 1, EAST)) {
        printf("✓ Test 4 réussi: Les autres directions ne sont pas affectées\n");
    } else {
        printf("✗ Test 4 échoué: Modification incorrecte des bits\n");
    }
    
    freeMaze(maze);
}

void testLink() {
    printf("\n=== Test Unitaire: link() ===\n");
    
    Maze *maze = init(4, 4, 12345);
    
    // Test 1: Ouvrir un passage et vérifier la symétrie
    link(maze, 1, 1, 1, 2, true);  // Ouvrir entre (1,1) et (1,2)
    
    bool wall1 = hasWall(maze, 1, 1, EAST);
    bool wall2 = hasWall(maze, 1, 2, WEST);
    
    if(!wall1 && !wall2) {
        printf("✓ Test 1 réussi: Passage ouvert symétrique - EAST de (1,1) et WEST de (1,2)\n");
    } else {
        printf("✗ Test 1 échoué: La symétrie n'est pas respectée (wall1=%d, wall2=%d)\n", wall1, wall2);
    }
    
    // Test 2: Fermer un passage et vérifier la symétrie
    link(maze, 2, 1, 2, 2, false);  // Fermer entre (2,1) et (2,2)
    
    wall1 = hasWall(maze, 2, 1, EAST);
    wall2 = hasWall(maze, 2, 2, WEST);
    
    if(wall1 && wall2) {
        printf("✓ Test 2 réussi: Passage fermé symétrique - murs présents des deux côtés\n");
    } else {
        printf("✗ Test 2 échoué: La symétrie n'est pas respectée lors de la fermeture\n");
    }
    
    // Test 3: Vérifier symétrie verticale (NORTH/SOUTH)
    link(maze, 1, 1, 2, 1, true);  // Ouvrir entre (1,1) et (2,1) verticalement
    
    wall1 = hasWall(maze, 1, 1, SOUTH);
    wall2 = hasWall(maze, 2, 1, NORTH);
    
    if(!wall1 && !wall2) {
        printf("✓ Test 3 réussi: Symétrie verticale vérifiée - SOUTH et NORTH cohérents\n");
    } else {
        printf("✗ Test 3 échoué: La symétrie verticale n'est pas respectée\n");
    }
    
    freeMaze(maze);
}

void testBfsDistances() {
    printf("\n=== Test Unitaire: bfsDistances() ===\n");
    
    // Créer un labyrinthe 3x3 simple
    Maze *maze = init(3, 3, 12345);
    
    // Créer un couloir simple: (0,0) -> (0,1) -> (0,2) -> (1,2) -> (2,2)
    link(maze, 0, 0, 0, 1, true);
    link(maze, 0, 1, 0, 2, true);
    link(maze, 0, 2, 1, 2, true);
    link(maze, 1, 2, 2, 2, true);
    
    // Calculer les distances depuis (2,2)
    int **D = bfsDistances(maze, 2, 2);
    
    // Test 1: Distance à la sortie = 0
    if(D[2][2] == 0) {
        printf("✓ Test 1 réussi: Distance à la sortie (2,2) = 0\n");
    } else {
        printf("✗ Test 1 échoué: Distance à la sortie devrait être 0, obtenu %d\n", D[2][2]);
    }
    
    // Test 2: Distance augmente régulièrement
    if(D[1][2] == 1 && D[0][2] == 2 && D[0][1] == 3 && D[0][0] == 4) {
        printf("✓ Test 2 réussi: Les distances augmentent régulièrement le long du chemin\n");
        printf("  D[1][2]=%d, D[0][2]=%d, D[0][1]=%d, D[0][0]=%d\n", 
               D[1][2], D[0][2], D[0][1], D[0][0]);
    } else {
        printf("✗ Test 2 échoué: Les distances ne sont pas cohérentes\n");
        printf("  D[1][2]=%d (attendu 1), D[0][2]=%d (attendu 2), D[0][1]=%d (attendu 3), D[0][0]=%d (attendu 4)\n",
               D[1][2], D[0][2], D[0][1], D[0][0]);
    }
    
    // Test 3: Cellules non accessibles
    // Les cellules (1,0), (1,1), (2,0), (2,1) ne sont pas connectées
    int unreachable = 0;
    if(D[1][0] == -1) unreachable++;
    if(D[1][1] == -1) unreachable++;
    if(D[2][0] == -1) unreachable++;
    if(D[2][1] == -1) unreachable++;
    
    if(unreachable == 4) {
        printf("✓ Test 3 réussi: Les cellules non connectées ont une distance = -1\n");
    } else {
        printf("✗ Test 3 échoué: %d/4 cellules non connectées détectées\n", unreachable);
    }
    
    freeDistances(D, 3);
    freeMaze(maze);
}

void testReconstructPath() {
    printf("\n=== Test Unitaire: reconstructPath() ===\n");
    
    // Créer un labyrinthe 4x4 simple
    Maze *maze = init(4, 4, 12345);
    
    // Créer un chemin en L: (0,0) -> (1,0) -> (2,0) -> (3,0) -> (3,1) -> (3,2) -> (3,3)
    link(maze, 0, 0, 1, 0, true);
    link(maze, 1, 0, 2, 0, true);
    link(maze, 2, 0, 3, 0, true);
    link(maze, 3, 0, 3, 1, true);
    link(maze, 3, 1, 3, 2, true);
    link(maze, 3, 2, 3, 3, true);
    
    // Calculer les distances depuis la sortie (3,3)
    int **D = bfsDistances(maze, 3, 3);
    
    // Reconstruire le chemin de (0,0) à (3,3)
    int pathLength;
    Cell *path = reconstructPath(D, 0, 0, 3, 3, 4, 4, &pathLength, maze);
    
    // Test 1: Le chemin existe
    if(path != NULL) {
        printf("✓ Test 1 réussi: Un chemin a été trouvé\n");
    } else {
        printf("✗ Test 1 échoué: Aucun chemin trouvé\n");
        freeDistances(D, 4);
        freeMaze(maze);
        return;
    }
    
    // Test 2: La longueur du chemin est correcte (devrait être 7 cellules)
    if(pathLength == 7) {
        printf("✓ Test 2 réussi: Longueur du chemin = 7 (attendu)\n");
    } else {
        printf("✗ Test 2 échoué: Longueur = %d, attendu 7\n", pathLength);
    }
    
    // Test 3: Les distances décroissent le long du chemin
    bool distancesDecroissantes = true;
    for(int i = 0; i < pathLength - 1; i++) {
        int d1 = D[path[i].r][path[i].c];
        int d2 = D[path[i+1].r][path[i+1].c];
        
        if(d2 != d1 - 1) {
            distancesDecroissantes = false;
            printf("✗ Erreur: D[%d][%d]=%d -> D[%d][%d]=%d (devrait décroître de 1)\n",
                   path[i].r, path[i].c, d1, path[i+1].r, path[i+1].c, d2);
        }
    }
    
    if(distancesDecroissantes) {
        printf("✓ Test 3 réussi: Les distances décroissent correctement (D[i] = D[i+1] + 1)\n");
    } else {
        printf("✗ Test 3 échoué: Les distances ne décroissent pas correctement\n");
    }
    
    // Test 4: Le début et la fin sont corrects
    if(path[0].r == 0 && path[0].c == 0 && 
       path[pathLength-1].r == 3 && path[pathLength-1].c == 3) {
        printf("✓ Test 4 réussi: Début (0,0) et fin (3,3) corrects\n");
    } else {
        printf("✗ Test 4 échoué: Début ou fin incorrects\n");
    }
    
    free(path);
    freeDistances(D, 4);
    freeMaze(maze);
}

// ============================================================================
// TESTS (anciens tests de validation)
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
    
    // ========================================================================
    // TESTS UNITAIRES
    // ========================================================================
    printf("╔══════════════════════════════════════════════════╗\n");
    printf("║  TESTS UNITAIRES (Fadel Mukoka)                 ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");
    
    testCanMove();
    testGetNeighbor();
    testHasWallSetWall();
    testLink();
    testBfsDistances();
    testReconstructPath();
    
    printf("\n╔══════════════════════════════════════════════════╗\n");
    printf("║  Tests unitaires terminés!                      ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");
    
    // ========================================================================
    // DÉMONSTRATION DU LABYRINTHE
    // ========================================================================
    
    int H = 10, W = 15;
    unsigned int seed = (unsigned int)time(NULL);
    
    printf("\n\nConfiguration:\n");
    printf("  - Dimensions: %d × %d\n", H, W);
    printf("  - Seed: %u\n\n", seed);
    
    // Création et génération
    printf("Création du labyrinthe...\n");
    Maze *maze = init(H, W, seed);
    
    printf("Génération par division récursive...\n");
    divide(maze, 0, 0, H, W);
    
    printf("\n");
    printMaze(maze);
    
    // Tests de validation
    printf("\n╔══════════════════════════════════════════════════╗\n");
    printf("║  TESTS DE VALIDATION                            ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");
    
    testSymmetry(maze);
    testConnectivity(maze, 0, 0);
    testAcyclicity(maze);
    testReproducibility();
    
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