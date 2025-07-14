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

void pressEnter(int numOfBreaks) {
  for (int i = 0; i < numOfBreaks; i++) printf("\n");

  printf("Pressione <Enter> para continuar...");
  getchar();
  cleanScreen();
}

bool contains(int* arr, int size, int num) {
  for (int i = 0; i < size; i++) {
      if (arr[i] == num) return true; 
  }   
  return false;
}

int char2int(char caractere) {
  int num = caractere - '0';

  if (num < 0 || num > 9) return caractere;
  
  return num;
}

char int2char(int num) {
  if (num < 0 || num > 9) return num;
  return num + '0';
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
      
  free(record->value);
  free(record->key);
}