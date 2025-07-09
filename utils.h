#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include "data_base.h"

void cleanScreen(); // Assumindo que existe em outro arquivo
bool contains(int* arr, int size, int num);
int fsize(char* file);
void freeList(List* list);
void freeRecord(Record* record);

#endif