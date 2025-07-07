#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>

#define NUM_OPTIONS 10

// Definições de tipos de função
typedef void (*OneArgFunction)(int);
typedef void (*NoArgsFunction)();

// Estrutura para as opções
typedef struct {
    char name[20];
    bool hasTitle;
    NoArgsFunction function;
} Option;

// // Variáveis globais (declarações externas)
extern Option options[NUM_OPTIONS];
extern int maxOption;

// Declarações das funções
void zeroOption();
void endOption();
void addOption(char* name, bool hasTitle, NoArgsFunction function);
void showOptions();
void showOption(Option option);

#endif // OPTIONS_H