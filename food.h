#ifndef FOOD_H
#define FOOD_H

#define MAX_FOOD_ON_MAP 3

typedef enum {
    FOOD_NORMAL,
    FOOD_SPEED_UP,
    FOOD_SHRINK,
    FOOD_TYPE_COUNT
} FoodType;

typedef struct {
    int x, y;
    FoodType type;
    int active;
} Food;

extern Food food[MAX_FOOD_ON_MAP];
extern int foodCount;

void generateFood();
void applyFoodEffect(FoodType type);

#endif