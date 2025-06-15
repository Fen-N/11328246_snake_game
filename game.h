#ifndef GAME_H
#define GAME_H

#include <windows.h>
#include <time.h>

#define WIDTH  20
#define HEIGHT 15
#define MAX_SNAKE_SIZE 100

typedef struct {
    int x, y;
} Point;

extern Point snake[MAX_SNAKE_SIZE];
extern int snakeLength;
extern char direction;
extern int score, gameOver, highScore, speed, started;
extern HANDLE hConsole;

void initGame();
void playGame();
void input();
void logic();
int isCollision(int x, int y);
void setColor(int color);

#endif