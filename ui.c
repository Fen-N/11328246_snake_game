#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "food.h"
#include "obstacle.h"
#include "ui.h"

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
        printf("  Hit wall, yourself, or obstacle -> Game Over\n\n");
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
    if (score > highScore) highScore = score;
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

void drawMap() {
    system("cls");
    int infoX = WIDTH + 4;
    for (int y = 0; y <= HEIGHT; y++) {
        for (int x = 0; x <= WIDTH; x++) {
            if (y == 0 || y == HEIGHT || x == 0 || x == WIDTH) {
                setColor(15); printf("#");
            } else {
                int printed = 0;
                if (snake[0].x == x && snake[0].y == y) {
                    setColor(10); printf("O"); printed = 1;
                } else {
                    for (int i = 1; i < snakeLength; i++) {
                        if (snake[i].x == x && snake[i].y == y) {
                            setColor(2); printf("o"); printed = 1; break;
                        }
                    }
                }
                if (!printed) {
                    int foodHere = 0;
                    for (int i = 0; i < foodCount; i++) {
                        if (food[i].active && food[i].x == x && food[i].y == y) {
                            if (food[i].type == FOOD_NORMAL) { setColor(12); printf("*"); }
                            else if (food[i].type == FOOD_SPEED_UP) { setColor(14); printf("@"); }
                            else if (food[i].type == FOOD_SHRINK) { setColor(13); printf("&"); }
                            foodHere = 1; break;
                        }
                    }
                    if (!foodHere) {
                        if (isObstacle(x, y)) { setColor(8); printf("X"); }
                        else { setColor(7); printf(" "); }
                    }
                }
            }
        }
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
                if (food[i].active) {
                    if (food[i].type == FOOD_NORMAL) {
                        setColor(12); printf("*"); setColor(7); printf(" (Normal) ");
                    }
                    else if (food[i].type == FOOD_SPEED_UP) {
                        setColor(14); printf("@"); setColor(7); printf(" (Speed Up) ");
                    }
                    else if (food[i].type == FOOD_SHRINK) {
                        setColor(13); printf("&"); setColor(7); printf(" (Shrink) ");
                    }
                }
            }
            setColor(7);
        }
        printf("\n");
    }
    setColor(7);
}