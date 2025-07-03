#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define NUM_OPTIONS 10

typedef void (*OneArgFunction)(int);
typedef void (*NoArgsFunction)();

typedef struct {
    char name[20];
    bool hasTitle;
    NoArgsFunction function;
} Option;

Option options[NUM_OPTIONS];
int maxOption = 0;

void cleanScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void showInfo() {
    printf("\n\n\nJogo legal feito por Kaua Otaviano para a materia APC.\n");
}

void startGame() {
    printf("jogo\n");
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