#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include "data_base.h"

void cleanScreen(); 
void cleanBuffer();
void pressEnter(int numOfBreaks, bool clean);
bool contains(int* arr, int size, int num);
int char2int(char caractere);
char int2char(int num);
int fsize(char* file);
void freeList(List* list);
void freeRecord(Record* record);
bool questionBoolean(char* message);

#endif