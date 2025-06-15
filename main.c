#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "game.h"
#include "food.h"
#include "obstacle.h"
#include "ui.h"

int main() {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    char choice;

    srand((unsigned)time(NULL));

    showStartScreen();

    do {
        playGame();
        showEndScreen(&choice);
    } while (choice == 'Y' || choice == 'y');

    printf("\nThank you for playing!\n");
    return 0;
}