#include <stdlib.h>
#include "game.h"
#include "food.h"

Food food[MAX_FOOD_ON_MAP];
int foodCount = 1;

void generateFood() {
    foodCount = (rand() % MAX_FOOD_ON_MAP) + 1;
    for (int i = 0; i < foodCount; i++) {
        int valid = 0;
        while (!valid) {
            food[i].x = rand() % (WIDTH - 2) + 1;
            food[i].y = rand() % (HEIGHT - 2) + 1;
            food[i].type = (FoodType)(rand() % FOOD_TYPE_COUNT);
            food[i].active = 1;
            valid = !isCollision(food[i].x, food[i].y);
            for (int j = 0; j < i; j++) {
                if (food[j].active && food[j].x == food[i].x && food[j].y == food[i].y)
                    valid = 0;
            }
        }
    }
    for (int i = foodCount; i < MAX_FOOD_ON_MAP; i++) {
        food[i].active = 0;
    }
}

void applyFoodEffect(FoodType type) {
    switch (type) {
        case FOOD_NORMAL:
            if (snakeLength < MAX_SNAKE_SIZE) snakeLength++;
            score++;
            break;
        case FOOD_SPEED_UP:
            if (snakeLength < MAX_SNAKE_SIZE) snakeLength++;
            score += 2;
            if (speed > 60) speed -= 20;
            break;
        case FOOD_SHRINK:
            if (snakeLength > 3) snakeLength -= 2;
            score += 3;
            break;
        default:
            if (snakeLength < MAX_SNAKE_SIZE) snakeLength++;
            score++;
            break;
    }
}