#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>

// Constantes pour les directions (bits)
#define NORTH 1  // 0001
#define EAST  2  // 0010
#define SOUTH 4  // 0100
#define WEST  8  // 1000

// Structure du labyrinthe
typedef struct {
    int height;              // H
    int width;               // W
    unsigned char **walls;   // grille H×W de murs (bits)
    unsigned int seed;       // graine RNG
} Maze;

// Structure pour une cellule
typedef struct {
    int r, c;
} Cell;

// ============================================================================
// OPÉRATIONS DE BASE
// ============================================================================

// Initialise le labyrinthe H×W avec tous les bords fermés
Maze* init(int H, int W, unsigned int seed);

// Vérifie si un mur existe dans la direction dir
bool hasWall(Maze *maze, int r, int c, int dir);

// Définit un mur (sans symétrie, usage interne)
void setWall(Maze *maze, int r, int c, int dir, bool present);

// Ouvre/ferme le mur entre deux cellules adjacentes (avec symétrie)
void link(Maze *maze, int r1, int c1, int r2, int c2, bool open);

// Vérifie si on peut se déplacer dans une direction
bool canMove(Maze *maze, int r, int c, int dir);

// Libère la mémoire du labyrinthe
void freeMaze(Maze *maze);

// ============================================================================
// GÉNÉRATION
// ============================================================================

// Génère le labyrinthe par division récursive
void divide(Maze *maze, int r0, int c0, int r1, int c1);

// ============================================================================
// AFFICHAGE
// ============================================================================

// Affiche le labyrinthe en ASCII
void printMaze(Maze *maze);

// Affiche le labyrinthe avec un chemin marqué
void printMazeWithPath(Maze *maze, Cell *path, int pathLength);

// Affiche le labyrinthe avec les numéros d'étape du chemin
void printMazeWithPathNumbers(Maze *maze, Cell *path, int pathLength);

// ============================================================================
// UTILITAIRES
// ============================================================================

// Retourne la direction opposée
int oppositeDir(int dir);

// Calcule les coordonnées du voisin dans une direction
bool getNeighbor(int r, int c, int dir, int H, int W, int *nr, int *nc);

#endif // MAZE_H