#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define WIDTH  20
#define HEIGHT 15
#define MAX_SNAKE_SIZE 100
#define MAX_FOOD_ON_MAP 3 // 最多同時出現3個食物
#define MAX_OBSTACLES 20

typedef struct {
    int x, y;
} Point;

Point snake[MAX_SNAKE_SIZE];
int snakeLength;
char direction;

// 移除減速食物
typedef enum {
    FOOD_NORMAL,
    FOOD_SPEED_UP,
    FOOD_SHRINK,
    FOOD_TYPE_COUNT
} FoodType;

typedef struct {
    int x, y;
    FoodType type;
    int active; // 1: 有效, 0: 無效
} Food;

Food food[MAX_FOOD_ON_MAP];
int foodCount = 1;
int speed = 150; // 遊戲速度(ms)

int score = 0;
int gameOver = 0;
int highScore = 0; // 新增最高分變數

HANDLE hConsole;

// 障礙物結構
typedef struct {
    int x, y;
    int active;
    time_t appearTime;
} Obstacle;

Obstacle obstacles[MAX_OBSTACLES];
int obstacleCount = 0;
int lastScoreForObstacle = 0;
int obstacleTriggerCount = 0; // 新增：記錄已觸發幾次障礙物生成

void initGame();
void drawMap();
void input();
void logic();
void generateFood();
void applyFoodEffect(FoodType type);
int isCollision(int x, int y);
void playGame();
void setColor(int color);
void showStartScreen();
void showEndScreen(char *choice);

// 新增障礙物相關函數宣告
void tryGenerateObstacle();
void updateObstacles();
int isObstacle(int x, int y);

int main() {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    char choice;

    srand((unsigned)time(NULL)); // 修正：只在 main() 執行一次亂數種子初始化

    showStartScreen();

    do {
        playGame();
        showEndScreen(&choice);
    } while (choice == 'Y' || choice == 'y');

    printf("\nThank you for playing!\n");
    return 0;
}

void showStartScreen() {
    char choice;
    while (1) {
        system("cls");
        setColor(14); printf("========== S N A K E ==========\n\n");
        setColor(7);  printf("Instructions:\n");
        printf("  W Up  S Down  A Left  D Right\n");
        printf("  Eat different food to get different effects:\n");
        setColor(12); printf("    *"); setColor(7); printf("  (Red)   : Normal, +1 score, grow\n");
        setColor(14); printf("    @"); setColor(7); printf("  (Yellow): Speed Up, +2 score, grow, faster\n");
        setColor(13); printf("    &"); setColor(7); printf("  (Purple): Shrink, +3 score, shrink\n");
        printf("  1 to 3 food items appear randomly at the same time\n");
        setColor(8);  printf("    X"); setColor(7); printf("  (Gray)  : Obstacle, avoid hitting!\n");
        printf("  Every time your score increases by 5, the nth time will generate n obstacles (each lasts 15 seconds)\n");
        printf("  Hit wall, yourself, or obstacle → Game Over\n\n");
        printf("Please select:\n");
        printf("  1. Start Game\n");
        printf("  2. Exit\n\n");
        printf("Enter option (1/2): ");

        choice = _getch();
        if (choice == '1') break;
        else if (choice == '2') {
            printf("\nThank you for playing!\n");
            exit(0);
        }
    }
}

void showEndScreen(char *choice) {
    if (score > highScore) highScore = score; // 更新最高分
    while (1) {
        system("cls");
        setColor(14); printf("========== G A M E   O V E R ==========\n\n");
        setColor(7);  printf("Your Score: %d\n", score);
        printf("High Score: %d\n\n", highScore);
        printf("Would you like to restart? (Y/N): ");
        *choice = _getch();
        if (*choice == 'Y' || *choice == 'y' || *choice == 'N' || *choice == 'n') {
            printf("%c\n", *choice);
            break;
        }
    }
}

int started = 0; // 遊戲是否已開始移動

void playGame() {
    initGame();
    started = 0; // 每次新遊戲都重設
    while (!gameOver) {
        drawMap();
        input();
        if (started) {
            logic();
            updateObstacles(); // 更新障礙物狀態
        }
        Sleep(speed);
    }
}

void initGame() {
    snakeLength = 1; // 一開始只顯示蛇頭
    snake[0].x = WIDTH / 2;
    snake[0].y = HEIGHT / 2;
    // 蛇身預設位置（等開始時才顯示）
    for (int i = 1; i < 3; i++) {
        snake[i].x = snake[0].x - i;
        snake[i].y = snake[0].y;
    }
    direction = 'D';
    score = 0;
    gameOver = 0;
    speed = 150;
    generateFood();
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacles[i].active = 0;
    }
    obstacleCount = 0;
    lastScoreForObstacle = 0;
    obstacleTriggerCount = 0;
}

// 修改 input 函數，讓初始時按 a 也能正確展開蛇身
void input() {
    if (_kbhit()) {
        char ch = _getch();
        if (!started) {
            // 只要按下 wasd 任一鍵就展開蛇身，並設定方向
            if (ch == 'W' || ch == 'w') {
                direction = 'W';
                snakeLength = 3;
                started = 1;
            } else if (ch == 'S' || ch == 's') {
                direction = 'S';
                snakeLength = 3;
                started = 1;
            } else if (ch == 'A' || ch == 'a') {
                direction = 'A';
                // 重新設定蛇身在蛇頭右側
                snake[1].x = snake[0].x + 1;
                snake[1].y = snake[0].y;
                snake[2].x = snake[0].x + 2;
                snake[2].y = snake[0].y;
                snakeLength = 3;
                started = 1;
            } else if (ch == 'D' || ch == 'd') {
                direction = 'D';
                // 重新設定蛇身在蛇頭左側
                snake[1].x = snake[0].x - 1;
                snake[1].y = snake[0].y;
                snake[2].x = snake[0].x - 2;
                snake[2].y = snake[0].y;
                snakeLength = 3;
                started = 1;
            }
        } else {
            // 遊戲已開始後，維持原本的方向判斷
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

    // 撞牆、撞自己或撞障礙物
    if (next.x <= 0 || next.x >= WIDTH || next.y <= 0 || next.y >= HEIGHT ||
        isCollision(next.x, next.y) || isObstacle(next.x, next.y)) {
        gameOver = 1;
        return;
    }

    for (int i = snakeLength - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }
    snake[0] = next;

    // 檢查是否吃到任何一個食物
    for (int i = 0; i < foodCount; i++) {
        if (food[i].active && next.x == food[i].x && next.y == food[i].y) {
            applyFoodEffect(food[i].type);
            food[i].active = 0;
        }
    }

    // 若所有食物都被吃掉，重新產生
    int allEaten = 1;
    for (int i = 0; i < foodCount; i++) {
        if (food[i].active) {
            allEaten = 0;
            break;
        }
    }
    if (allEaten) {
        generateFood();
    }

    // 每得5分生成障礙物，第n次生成n個
    tryGenerateObstacle();
}

// 計算兩點距離的平方
int dist2(int x1, int y1, int x2, int y2) {
    int dx = x1 - x2;
    int dy = y1 - y2;
    return dx * dx + dy * dy;
}

// 修改障礙物生成邏輯：每得5分，第n次生成n個障礙物，且不會在玩家當前位置太近的地方出現
void tryGenerateObstacle() {
    int triggerTimes = score / 5;
    if (triggerTimes > obstacleTriggerCount) {
        int n = triggerTimes; // 第n次
        int generated = 0;
        for (int k = 0; k < n && obstacleCount < MAX_OBSTACLES; k++) {
            // 找到一個可用的障礙物槽
            for (int i = 0; i < MAX_OBSTACLES; i++) {
                if (!obstacles[i].active) {
                    int valid = 0;
                    int tryCount = 0;
                    while (!valid && tryCount < 100) {
                        obstacles[i].x = rand() % (WIDTH - 2) + 1;
                        obstacles[i].y = rand() % (HEIGHT - 2) + 1;
                        // 不與蛇身、食物、其他障礙物重疊，且離蛇頭至少3格遠
                        valid = !isCollision(obstacles[i].x, obstacles[i].y) && !isObstacle(obstacles[i].x, obstacles[i].y);
                        for (int j = 0; j < foodCount; j++) {
                            if (food[j].active && food[j].x == obstacles[i].x && food[j].y == obstacles[i].y) {
                                valid = 0;
                                break;
                            }
                        }
                        // 距離蛇頭至少3格遠(歐氏距離平方>=9)
                        if (valid && dist2(obstacles[i].x, obstacles[i].y, snake[0].x, snake[0].y) < 9) {
                            valid = 0;
                        }
                        tryCount++;
                    }
                    if (valid) {
                        obstacles[i].active = 1;
                        obstacles[i].appearTime = time(NULL);
                        obstacleCount++;
                        generated++;
                    }
                    break;
                }
            }
        }
        obstacleTriggerCount = triggerTimes;
    }
}

// 每次循環檢查障礙物是否超過15秒，若超過則移除
void updateObstacles() {
    time_t now = time(NULL);
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active && now - obstacles[i].appearTime >= 15) {
            obstacles[i].active = 0;
            // 修正：只在障礙物實際被移除時才減少 obstacleCount
            if (obstacleCount > 0) obstacleCount--;
        }
    }
}

// 判斷(x, y)是否為障礙物
int isObstacle(int x, int y) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active && obstacles[i].x == x && obstacles[i].y == y)
            return 1;
    }
    return 0;
}

void generateFood() {
    // 修正：只在程式啟動時呼叫 srand，這裡不再呼叫 srand
    foodCount = (rand() % MAX_FOOD_ON_MAP) + 1;
    for (int i = 0; i < foodCount; i++) {
        int valid = 0;
        while (!valid) {
            food[i].x = rand() % (WIDTH - 2) + 1;
            food[i].y = rand() % (HEIGHT - 2) + 1;
            food[i].type = (FoodType)(rand() % FOOD_TYPE_COUNT);
            food[i].active = 1;
            // 確保不與蛇身或其他食物重疊
            valid = !isCollision(food[i].x, food[i].y);
            for (int j = 0; j < i; j++) {
                if (food[j].x == food[i].x && food[j].y == food[i].y) {
                    valid = 0;
                    break;
                }
            }
        }
    }
    // 若未用到的食物設為無效
    for (int i = foodCount; i < MAX_FOOD_ON_MAP; i++) {
        food[i].active = 0;
    }
}

// 根據食物種類給予不同效果
void applyFoodEffect(FoodType type) {
    switch (type) {
        case FOOD_NORMAL:
            if (snakeLength < MAX_SNAKE_SIZE) snakeLength++;
            score++;
            break;
        case FOOD_SPEED_UP:
            if (snakeLength < MAX_SNAKE_SIZE) snakeLength++;
            score += 2;
            if (speed > 60) speed -= 20; // 加速
            break;
        case FOOD_SHRINK:
            if (snakeLength > 3) snakeLength -= 2; // 蛇身縮短
            score += 3;
            break;
        default:
            if (snakeLength < MAX_SNAKE_SIZE) snakeLength++;
            score++;
            break;
    }
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

// 補上 drawMap 函數
void drawMap() {
    system("cls");

    // 計算右側資訊欄的起始 x 座標
    int infoX = WIDTH + 4;

    for (int y = 0; y <= HEIGHT; y++) {
        // 先畫主遊戲區
        for (int x = 0; x <= WIDTH; x++) {
            if (y == 0 || y == HEIGHT || x == 0 || x == WIDTH) {
                setColor(15); printf("#");
            } else {
                int printed = 0;
                // 蛇
                for (int i = 0; i < snakeLength; i++) {
                    if (snake[i].x == x && snake[i].y == y) {
                        if (i == 0) { setColor(10); printf("O"); }
                        else       { setColor(2);  printf("o"); }
                        printed = 1;
                        break;
                    }
                }
                // 食物
                if (!printed) {
                    for (int i = 0; i < foodCount; i++) {
                        if (food[i].active && food[i].x == x && food[i].y == y) {
                            switch (food[i].type) {
                                case FOOD_NORMAL:    setColor(12); printf("*"); break; // 紅色
                                case FOOD_SPEED_UP:  setColor(14); printf("@"); break; // 黃色
                                case FOOD_SHRINK:    setColor(13); printf("&"); break; // 紫色
                                default:             setColor(7);  printf("*"); break;
                            }
                            printed = 1;
                            break;
                        }
                    }
                }
                // 障礙物
                if (!printed) {
                    for (int i = 0; i < MAX_OBSTACLES; i++) {
                        if (obstacles[i].active && obstacles[i].x == x && obstacles[i].y == y) {
                            setColor(8); printf("X"); // 灰色障礙物
                            printed = 1;
                            break;
                        }
                    }
                }
                if (!printed) printf(" ");
            }
        }

        // 右側資訊欄
        setColor(7);
        if (y == 2) {
            printf("%*sScore: %d", infoX - WIDTH, "", score);
        } else if (y == 4) {
            printf("%*sHigh Score: %d", infoX - WIDTH, "", highScore);
        } else if (y == 6) {
            printf("%*sSpeed: %d ms", infoX - WIDTH, "", speed);
        } else if (y == 8) {
            printf("%*sFood types: ", infoX - WIDTH, "");
            for (int i = 0; i < foodCount; i++) {
                if (!food[i].active) continue;
                switch (food[i].type) {
                    case FOOD_NORMAL:    printf("Normal(*) "); break;
                    case FOOD_SPEED_UP:  printf("SpeedUp(@) "); break;
                    case FOOD_SHRINK:    printf("Shrink(&) "); break;
                    default:             printf("Unknown "); break;
                }
            }
        }
        printf("\n");
    }
    setColor(7);
}
