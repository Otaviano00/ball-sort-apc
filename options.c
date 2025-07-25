#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "options.h"
#include "utils.h"

void endOption() {
    printf("\n\n\nPressione <Enter> para voltar...");
    cleanBuffer();
}

void addOption(Menu* menu, char* name, bool hasTitle, bool needEnd, NoArgsFunction function) {
    int numOptions = menu->numOptions;

    strcpy(menu->options[numOptions].name, name);
    menu->options[numOptions].hasTitle = hasTitle;
    menu->options[numOptions].needEnd = needEnd;
    menu->options[numOptions].function = function;

    menu->numOptions++;
}

void showOptions(Menu menu) {
    for (int i = 0; i < menu.numOptions; i++) {
        printf("[%d] - %s\n", i+1, menu.options[i].name);
    }
    printf("\n");
}

void showOption(Option option) {
    cleanScreen();
    if (option.hasTitle) printf("=================== %s ===================\n", option.name);

    option.function();

    if (option.needEnd) endOption();
}

void showMenu(Menu menu, User userOn) {
    while (true) {
        Record* temp = findUserByName(userOn.name);

        userOn = *(User*) temp->value;

        cleanScreen();

        printf("=================== %s ===================\n", menu.title);
        printf("User: %s -- Max Level: %d\n", userOn.name, userOn.points);

        freeRecord(temp);

        showOptions(menu);

        printf("Escolha uma opcao: ");

        int choise;
        choise = getchar() - '0';
        cleanBuffer();

        choise--;

        if (choise < 0 || choise >= menu.numOptions) continue;

        if (choise == menu.numOptions - 1) {
            if (menu.options[choise].function == NULL) break;
            menu.options[choise].function();
            break;
        }
    
        if (menu.options[choise].function != NULL) showOption(menu.options[choise]);        
    }
}