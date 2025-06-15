#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "obstacle.h"

Obstacle obstacles[MAX_OBSTACLES];
int obstacleCount = 0;
static int obstacleTriggerCount = 0;

void resetObstacles() {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacles[i].active = 0;
    }
    obstacleCount = 0;
    obstacleTriggerCount = 0;
}

int dist2(int x1, int y1, int x2, int y2) {
    int dx = x1 - x2;
    int dy = y1 - y2;
    return dx * dx + dy * dy;
}

void tryGenerateObstacle() {
    int triggerTimes = score / 5;
    if (triggerTimes > obstacleTriggerCount) {
        int n = triggerTimes;
        for (int k = 0; k < n && obstacleCount < MAX_OBSTACLES; k++) {
            for (int i = 0; i < MAX_OBSTACLES; i++) {
                if (!obstacles[i].active) {
                    int valid = 0;
                    while (!valid) {
                        obstacles[i].x = rand() % (WIDTH - 2) + 1;
                        obstacles[i].y = rand() % (HEIGHT - 2) + 1;
                        valid = !isCollision(obstacles[i].x, obstacles[i].y);
                        for (int j = 0; j < snakeLength; j++) {
                            if (dist2(obstacles[i].x, obstacles[i].y, snake[j].x, snake[j].y) < 9)
                                valid = 0;
                        }
                    }
                    obstacles[i].active = 1;
                    obstacles[i].appearTime = time(NULL);
                    obstacleCount++;
                    break;
                }
            }
        }
        obstacleTriggerCount = triggerTimes;
    }
}

void updateObstacles() {
    time_t now = time(NULL);
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active && now - obstacles[i].appearTime >= 15) {
            obstacles[i].active = 0;
            if (obstacleCount > 0) obstacleCount--;
        }
    }
}

int isObstacle(int x, int y) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active && obstacles[i].x == x && obstacles[i].y == y)
            return 1;
    }
    return 0;
}