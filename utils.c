#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "utils.h"
#include "data_base.h"

void cleanScreen() {
  #ifdef _WIN32
      system("cls");
  #else
      system("clear");
  #endif
}

bool contains(int* arr, int size, int num) {
  for (int i = 0; i < size; i++) {
      if (arr[i] == num) return true; 
  }   
  return false;
}

int fsize(char* file) {
  int size;
  FILE* fh;

  fh = fopen(file, "rb");
  if(fh != NULL){
    if(fseek(fh, 0, SEEK_END) != 0){
      fclose(fh);
      return -1;
    }

    size = ftell(fh);
    fclose(fh);
    return size;
  }

  return -1; //error
}

void freeList(List* list) {
  if (list == NULL) return;
  
  for (int i = 0; i < list->size; i++) {
      freeRecord(&(list->elements[i]));
  }
  free(list->elements);
  free(list);
}

void freeRecord(Record* record) {
  if (record == NULL) return;
  free(record->key);
  free(record->value);
  free(record);
}