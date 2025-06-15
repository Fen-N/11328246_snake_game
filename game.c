#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include "game.h"
#include "food.h"
#include "obstacle.h"
#include "ui.h"

// 不要重複定義 Point、WIDTH、HEIGHT、MAX_SNAKE_SIZE 等
// 只需要定義全域變數（不加 extern）
Point snake[MAX_SNAKE_SIZE];
int snakeLength;
char direction;
int score = 0, gameOver = 0, highScore = 0, speed = 150, started = 0;
HANDLE hConsole;

void initGame() {
    snakeLength = 1;
    snake[0].x = WIDTH / 2;
    snake[0].y = HEIGHT / 2;
    for (int i = 1; i < 3; i++) {
        snake[i].x = snake[0].x - i;
        snake[i].y = snake[0].y;
    }
    direction = 'D';
    score = 0;
    gameOver = 0;
    speed = 150;
    generateFood();
    resetObstacles();
    started = 0;
}

void playGame() {
    initGame();
    while (!gameOver) {
        drawMap();
        input();
        if (started) {
            logic();
            updateObstacles();
        }
        Sleep(speed);
    }
}

void input() {
    if (_kbhit()) {
        char ch = _getch();
        if (!started) {
            if (ch == 'W' || ch == 'w') {
                direction = 'W'; snakeLength = 3; started = 1;
            } else if (ch == 'S' || ch == 's') {
                direction = 'S'; snakeLength = 3; started = 1;
            } else if (ch == 'A' || ch == 'a') {
                direction = 'A';
                snake[1].x = snake[0].x + 1; snake[1].y = snake[0].y;
                snake[2].x = snake[0].x + 2; snake[2].y = snake[0].y;
                snakeLength = 3; started = 1;
            } else if (ch == 'D' || ch == 'd') {
                direction = 'D';
                snake[1].x = snake[0].x - 1; snake[1].y = snake[0].y;
                snake[2].x = snake[0].x - 2; snake[2].y = snake[0].y;
                snakeLength = 3; started = 1;
            }
        } else {
            if ((ch == 'W' || ch == 'w') && direction != 'S') direction = 'W';
            else if ((ch == 'S' || ch == 's') && direction != 'W') direction = 'S';
            else if ((ch == 'A' || ch == 'a') && direction != 'D') direction = 'A';
            else if ((ch == 'D' || ch == 'd') && direction != 'A') direction = 'D';
        }
    }
}

void logic() {
    Point next = snake[0];
    if (direction == 'W') next.y--;
    else if (direction == 'S') next.y++;
    else if (direction == 'A') next.x--;
    else if (direction == 'D') next.x++;

    if (next.x <= 0 || next.x >= WIDTH || next.y <= 0 || next.y >= HEIGHT ||
        isCollision(next.x, next.y) || isObstacle(next.x, next.y)) {
        gameOver = 1;
        return;
    }

    for (int i = snakeLength - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }
    snake[0] = next;

    for (int i = 0; i < foodCount; i++) {
        if (food[i].active && next.x == food[i].x && next.y == food[i].y) {
            applyFoodEffect(food[i].type);
            food[i].active = 0;
        }
    }

    int allEaten = 1;
    for (int i = 0; i < foodCount; i++) {
        if (food[i].active) { allEaten = 0; break; }
    }
    if (allEaten) generateFood();

    tryGenerateObstacle();
}

int isCollision(int x, int y) {
    for (int i = 0; i < snakeLength; i++) {
        if (snake[i].x == x && snake[i].y == y)
            return 1;
    }
    return 0;
}

void setColor(int color) {
    SetConsoleTextAttribute(hConsole, color);
}

