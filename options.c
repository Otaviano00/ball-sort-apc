#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "options.h"
#include "utils.h"

int maxOption;

void zeroOption() {
    maxOption = 0;
}

void endOption() {
    printf("\n\n\nPressione <Enter> para voltar...");
    while (getchar() != '\n'); 
    getchar();
}

void addOption(Option* options, char* name, bool hasTitle, bool needEnd, NoArgsFunction function) {
    strcpy(options[maxOption].name, name);
    options[maxOption].hasTitle = hasTitle;
    options[maxOption].needEnd = needEnd;
    options[maxOption].function = function;
    maxOption++;
}

void showOptions(Option* options) {
    for (int i = 0; i < maxOption; i++) {
        printf("[%d] - %s\n", i+1, options[i].name);
    }
    printf("\n");
}

void showOption(Option option) {
    cleanScreen();
    if (option.hasTitle) printf("=================== %s ===================\n", option.name);

    option.function();

    if (option.needEnd) endOption();
}