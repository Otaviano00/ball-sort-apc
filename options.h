#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>
#include "data_base.h"

#define NUM_OPTIONS 10

// Definições de tipos de função
typedef void (*OneArgFunction)(int);
typedef void (*NoArgsFunction)();

// Estrutura para as opções
typedef struct {
    char name[20];
    bool hasTitle;
    bool needEnd;
    NoArgsFunction function;
} Option;

typedef struct {
    Option options[NUM_OPTIONS];
    int numOptions;
    char title[20];
} Menu;

// Declarações das funções
void endOption();
void addOption(Menu* menu, char* name, bool hasTitle, bool needEnd, NoArgsFunction function);
void showOptions(Menu menu);
void showOption(Option option);
void showMenu(Menu menu, User userOn);

#endif // OPTIONS_H