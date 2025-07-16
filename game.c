#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "options.h"
#include "utils.h"
#include "data_base.h"

// Dados sobre o jogo
Option menuStart[NUM_OPTIONS];
Option menuConfig[NUM_OPTIONS];
User userOn;

FILE* arq;
int maxColumn = 0;
int maxLine = 0;
Stack layout[10];

void initialScreen() {
    while (true) {
        cleanScreen();
        printf(" ________________________________________________________________________\n");
        printf("|                                                                       |\n");
        printf("|         ____              __   __   _____                    __       |\n");
        printf("|        / __ )   ____ _   / /  / /  / ___/  ____     _____   / /_      |\n");
        printf("|       / __  |  / __ `/  / /  / /   \\__ \\  / __ \\   / ___/  / __/      |\n");
        printf("|      / /_/ /  / /_/ /  / /  / /   ___/ / / /_/ /  / /     / /_        |\n");
        printf("|     /_____/   \\__,_/  /_/  /_/  /_____/  \\____/  /_/     /___/        |\n");
        printf("|                                                                       |\n");
        printf("|                                                                       |\n");
        printf("|                                                           by Otaviano |\n");
        printf(" ________________________________________________________________________\n");

        char nickname[20];
        printf("\n\nDigite o seu nickname: ");
        scanf(" %[^\n]", nickname);
        getchar();

        if (strlen(nickname) == 0) continue;

        strcpy(userOn.name, nickname);
        userOn.points = 0;
        bool sucess = persist("users", &userOn);

        /*
            Quando o registro é persistido e ele já existe, a função retorna false. Ela pode 
            retornar false em outros cenários (como tabela inválida por exemplo), mas por agora vou considerar que false == existe
        */
        if (!sucess) {
            char option;

            do {
                printf("\nEsse nome de usuario jah estah cadastrado. Gostaria de continuar com ele? [S/N] ");                
                scanf("%c", &option);
                cleanBuffer();
            } while(option != 'S' && option != 's' && option != 'N' && option != 'n');

            if (option == 'N' || option == 'n') continue;
        }
        
        userOn = *(User*) findUserByName(nickname);

        break;
    }
}

void printBall(char ball) {
    int color = 30;
    switch (ball) {
        case '*': color += 1; break;
        case '#': color += 2; break;
        case '@': color += 3; break;
        case '&': color += 4; break;
        case '%': color += 5; break;

    }
    printf("|\033[1;%dm%3c\033[0m|", color, ball);
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


void generateRandomGame(int numColuns, int numLines, int numEmptyColumns) {
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

void printLayout(Level* level) {
    printf("Level %d\n\n", level->order);

    for (int i = level->maxHeight - 1; i >= 0; i--) {
        for (int j = 0; j < level->numColumns; j++) {
            char ball = level->columns[j].balls[i];

            if (ball == 'X') 
                printf("|   |");
            else 
                printBall(ball);
        }
        printf("\n");
    }
    for (int i = 0; i < level->numColumns; i++) printf("=====");
    printf("\n");
    for (int i = 1; i <= level->numColumns; i++) printf("%3d  ", i);
    printf("\n\n");
}

void showInfo() {
    printf("\n\n\nJogo legal feito por Kaua Otaviano para a materia APC.\n");
}

bool verifyColumnComplete(Level* level, int origColumn, int destColumn) {
    char pivotBall = level->columns[destColumn-1].balls[0];
    for (int i = 0; i < level->maxHeight; i++) {
        if (level->columns[destColumn-1].balls[i] != pivotBall) return false;
    }

    level->columns[origColumn-1].complete = false;
    level->columns[destColumn-1].complete = true;

    return true;
}

bool verifyWin(Level* level) {
    int validColumns = 0;
    for (int i = 0; i < level->numColumns; i++) {
        if (level->columns[i].complete) validColumns++;
    }

    if (validColumns < level->numColumns - level->numEmptyColumns) return false;

    return true;
}

bool changeBalls(Level* level, int origColumn, int destColumn) {
    printf("\n");
    int origSize = 0;
    int destSize = 0;
    
    if (origColumn < 1 || origColumn > level->numColumns) {
        printf("Coluna de origem invalida!");
        return false;
    }

    origSize = level->columns[origColumn-1].size;
    
    if (origSize <= 0) {
        printf("Coluna de origem estah vazia!\n");
        return false;
    }

    if (destColumn < 1 || destColumn > level->numColumns) {
        printf("Coluna de destino invalida!\n");
        return false;
    }
    
    destSize = level->columns[destColumn-1].size;

    if (destSize >= level->maxHeight) {
        printf("Coluna de destino estah cheia!\n");
        return false;
    }

    char origBall = level->columns[origColumn-1].balls[origSize-1];
    char destBall = level->columns[destColumn-1].balls[destSize-1];

    if (origBall != destBall && destSize != 0) {
        printf("A bola de destino eh diferente da de origem\n");
        return false;
    }

    while (destSize < level->maxHeight && ( destSize == 0 || level->columns[origColumn-1].balls[origSize-1] == level->columns[destColumn-1].balls[destSize-1])) {
        origBall = level->columns[origColumn-1].balls[origSize-1];
        level->columns[destColumn-1].balls[destSize] = origBall;

        level->columns[origColumn-1].size = --origSize;
        level->columns[destColumn-1].size = ++destSize;

        level->columns[origColumn-1].balls[origSize] = 'X';
    }

    
    bool statusOrigColumn = level->columns[origColumn-1].complete;
    if (verifyColumnComplete(level, origColumn, destColumn) && !statusOrigColumn ) {
        printf("Voce fechou a coluna %d!\n", destColumn);

        if (!verifyWin(level)) pressEnter(1, false);
    }

    return true;
}

void startGame() {
    // setUpGameLayout(5, 4, 1);
    List* allLevels = findAll("levels");
    int numLevel = 0;
    for (int i = 1; i <= allLevels->size; i++) {
        Level*  level = (Level*) (findById("levels", i)->value);
        while (true) {
            cleanScreen();
            printLayout(level);

            int origColumn, destColumn;
            printf("Informe a coluna de origem: ");
            scanf("%d", &origColumn);
            printf("Informe a coluna de destino: ");
            scanf("%d", &destColumn);

            getchar();

            bool sucess = changeBalls(level, origColumn, destColumn);
            if (!sucess) {
                pressEnter(1, false);
                continue;
            };

            if (verifyWin(level)) {
                printf("\nPARABENS!!! VOCE VENCEU!!!\n");
                printf("PARABENS!!! VOCE VENCEU!!!\n");
                printf("PARABENS!!! VOCE VENCEU!!!\n");
                printf("PARABENS!!! VOCE VENCEU!!!\n");
                break;
            };
        }

        if (i == allLevels->size) {
            printf("\nParabens!!! Voce zerou o jogo!\n");
            pressEnter(1, true);
            break;
        }

        char option;
        do {
            printf("\nGostaria de ir para a proxima fase? [S/N] ");
            scanf("%c", &option);
            cleanBuffer();
        } while(option != 'S' && option != 's' && option != 'N' && option != 'n');

        if (option == 'N' || option == 'n') break;
    }

    freeList(allLevels);
}

void configGame() {
    printf("configuracao\n");
}

void exitGame(){   
    printf("\nObrigado por jogar!\n");
}

bool setUpLevels() {
    cleanTable("levels");

    arq = fopen("entrada.txt", "r");

    if (arq == NULL) {
        printf("Arquivo de configuracao dos fases nao encontrada.\n");
        printf("Adicione o arquivo 'entrada.txt' para executar o programa corretamente!\n");
        return false;
    }  
    
    Level level;
    level.numColumns = 0;
    level.maxHeight = 0;
    level.order = 0;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            level.columns[i].complete = false;
            level.columns[i].balls[j] = 'X';
        }
    }

    char caractere;
    int count = 0;
    int maxHeight = -1;
    int numEmptyColumns = 0;
    while (fscanf(arq, "%c\n", &caractere) != EOF) {
        if (caractere == '-') {
            if (level.numColumns > 0) {
                level.order = ++count;
                level.maxHeight = maxHeight;
                level.numEmptyColumns = numEmptyColumns;

                numEmptyColumns = 0;
                maxHeight = -1;
                
                persist("levels", &level);
            }
            level.numColumns = 0;
            level.maxHeight = 0;
            level.order = 0;
            for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 10; j++) {
                    level.columns[i].complete = false;
                    level.columns[i].balls[j] = 'X';
                }
            }
            continue;
        }

        if (caractere >= '0' && caractere <= '9') {
            if (caractere == '0') numEmptyColumns++;

            if (char2int(caractere) >= maxHeight) maxHeight = char2int(caractere);
            level.columns[level.numColumns].size = char2int(caractere);
            for (int i = 0; i < char2int(caractere); i++) {
                char ball;
                fscanf(arq, "%c\n", &ball);
                level.columns[level.numColumns].balls[i] = ball;
            }
            level.numColumns++;
        }
    }

    if (level.numColumns > 0) {
        level.order = ++count;
        level.maxHeight = maxHeight;
        level.numEmptyColumns = numEmptyColumns;
        persist("levels", &level);
    }

    fclose(arq);
    return true;
}

void setUpOptions() {
    zeroOption();
    addOption(menuStart, "Jogar", false, false, startGame);
    addOption(menuStart, "Configuracoes", true, true, configGame);
    addOption(menuStart, "Informacoes", true, true, showInfo);
    addOption(menuStart, "Sair", false, false, exitGame);
}

int menuInitial() {
    cleanScreen();

    printf("=================== Ball Sort ===================\n");
    printf("User: %s -- Points: %d\n", userOn.name, userOn.points);

    showOptions(menuStart);

    printf("Escolha uma opcao: ");

    int option;
    option = getchar() - '0';
    cleanBuffer();
    return option;
}

int main() {
    srand(time(NULL));

    setUpOptions();

    if (!setUpLevels()) return 0;

    initialScreen();

    while (true) {
        int choise = menuInitial() - 1;

        if (choise < 0 || choise >= maxOption) continue;

        if (choise == maxOption - 1) {
            menuStart[choise].function();
            break;
        }
        
        showOption(menuStart[choise]);        
    }

    return 0;
}