#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <time.h>
#define MAX_OBSTACLES 20

typedef struct {
    int x, y;
    int active;
    time_t appearTime;
} Obstacle;

extern Obstacle obstacles[MAX_OBSTACLES];
extern int obstacleCount;

void tryGenerateObstacle();
void updateObstacles();
int isObstacle(int x, int y);
void resetObstacles();

#endif