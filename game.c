#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "options.h"
#include "utils.h"
#include "data_base.h"

// Dados sobre o jogo

typedef struct {
    int size;
    int balls[10];
} Stack;


int maxColumn = 0;
int maxLine = 0;
Stack layout[10];

void printBall(int ball) {
    printf("|\033[1;%dm%3d\033[0m|", ball+30, ball);
}

void cleanGameLayout() {
    for (int i = 0; i < maxLine; i++) {
        for (int j = 0; j < maxColumn; j++) {
            layout[j].balls[i] = 0;
        }
        layout[i].size = 0;
    }
}

int* generateRandomNums(int amount, int range) {
    int* arr = malloc(sizeof(int) * amount);
    for (int i = 0; i < amount; i++) {
        int num = rand() % range;
        while (contains(arr, amount, num)) {
            num = rand() % range;
        } 
        arr[i] = num;
    }
    return arr;
}


void setUpGameLayout(int numColuns, int numLines, int numEmptyColumns) {
    cleanGameLayout();

    maxColumn = numColuns;
    maxLine = numLines;
    int balls[10];

    for (int i = 0; i < 10; i++) {
        balls[i] = 0;
    }

    int* emptyColumns = generateRandomNums(numEmptyColumns, numColuns);

    for (int j = 0; j < numColuns; j++) {
        if (contains(emptyColumns, numEmptyColumns, j)) continue;
        for (int i = 0; i < numLines ; i++) {
            int num = rand() % (numColuns - numEmptyColumns);
            
            while (balls[num] >= numLines) {
                num = rand() % (numColuns - numEmptyColumns);
            }

            balls[num]++;

            layout[j].balls[i] = num + 1;
            layout[j].size++;

        }
    }
}

void printLayout() {
    for (int i = maxLine - 1; i >= 0; i--) {
        for (int j = 0; j < maxColumn; j++) {
            int ball = layout[j].balls[i];

            if (ball == 0) 
                printf("|   |");
            else 
                printBall(ball);
        }
        printf("\n");
    }
    for (int i = 0; i < maxColumn; i++) printf("=====");
}

void showInfo() {
    printf("\n\n\nJogo legal feito por Kaua Otaviano para a materia APC.\n");
}

void startGame() {
    setUpGameLayout(5, 4, 1);
    while (true) {
        // cleanScreen();
        printLayout();
        break;
    }
}

void configGame() {
    printf("configuracao\n");
}

void exitGame(){   
    printf("\nObrigado por jogar!\n");
}

void setUpOptions() {
    zeroOption();
    addOption("Jogar", false, startGame);
    addOption("Configuracoes", true, configGame);
    addOption("Informacoes", true, showInfo);
    addOption("Sair", false, exitGame);
}

int menu() {
    cleanScreen();

    printf("=================== Ball Sort ===================\n");

    showOptions();

    printf("Escolha uma opcao: ");

    int option;
    option = getchar() - '0';

    return option;
}

int main() {
    srand(time(NULL));

    setUpOptions();

    while (true) {
        int choise = menu() - 1;

        if (choise < 0 || choise >= maxOption) continue;

        if (choise == maxOption - 1) {
            options[choise].function();
            break;
        }
        
        showOption(options[choise]);        
    }

    return 0;
}