#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define NUM_OPTIONS 10

typedef void (*OneArgFunction)(int);
typedef void (*NoArgsFunction)();

// Dados das opções

typedef struct {
    char name[20];
    bool hasTitle;
    NoArgsFunction function;
} Option;

Option options[NUM_OPTIONS];
int maxOption = 0;

// Dados sobre o jogo

typedef struct {
    int size;
    int balls[10];
} Stack;


int sizeLayout = 0;
Stack layout[10];


void cleanScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void printBall(int ball) {
    printf("|\033[1;%dm%3d\033[0m|", ball+30, ball);
}

void setUpGameLayout(int size) {
    sizeLayout = size;
    for (int i = 0; i < sizeLayout; i++) {
        for (int j = 0; j < sizeLayout; j++) {
            layout[j].balls[i] = size;
        }
    }
}

void printLayout() {
    for (int i = sizeLayout - 1; i >= 0; i--) {
        for (int j = 0; j < sizeLayout; j++) {
            int ball = layout[j].balls[i];

            if (ball == 0) 
                printf("|   |");
            else 
                printBall(ball);
        }
        printf("\n");
    }
    for (int i = 0; i < sizeLayout; i++) printf("=====");
}

void showInfo() {
    printf("\n\n\nJogo legal feito por Kaua Otaviano para a materia APC.\n");
}

void startGame() {
    while (true) {
        cleanScreen();
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

void endOption() {
    printf("\n\n\nPressione Enter para voltar...");
    while (getchar() != '\n'); 
    getchar();
}

void addOption(char* name, bool hasTitle, NoArgsFunction function) {
    strcpy(options[maxOption].name, name);
    options[maxOption].hasTitle = hasTitle;
    options[maxOption].function = function;
    maxOption++;
}

void setUpOptions() {
    maxOption = 0;
    addOption("Jogar", false, startGame);
    addOption("Configuracoes", true, configGame);
    addOption("Informacoes", true, showInfo);
    addOption("Sair", false, exitGame);
}

void showOptions() {
    for (int i = 0; i < maxOption; i++) {
        printf("[%d] - %s\n", i+1, options[i].name);
    }
    printf("\n");
}

void showOption(Option option) {
    cleanScreen();
    if (option.hasTitle) printf("=================== %s ===================\n", option.name);

    option.function();

    endOption();
}

// Printa o menu e retorna a opção escolhida pelo usuário
int menu() {
    cleanScreen();

    setUpOptions();

    printf("=================== Ball Sort ===================\n");

    showOptions();

    printf("Escolha uma opcao: ");

    int option;
    option = getchar() - '0';

    return option;
}

int main() {
    setUpGameLayout(6);
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