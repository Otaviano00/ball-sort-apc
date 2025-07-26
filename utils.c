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

void pressEnter(int numOfBreaks, bool clean) {
  for (int i = 0; i < numOfBreaks; i++) printf("\n");

  printf("Pressione <Enter> para continuar...");

  cleanBuffer();

  if (clean) cleanScreen();
}

void cleanBuffer() {
  int c;
  while ((c = getchar()) != '\n');
}

bool contains(int* arr, int size, int num) {
  for (int i = 0; i < size; i++) {
      if (arr[i] == num) return true; 
  }   
  return false;
}

bool strContainsChar(char* arr, int size, char caractere) {
  for (int i = 0; i < size; i++) {
      if (arr[i] == caractere) return true; 
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
    if (list == NULL) {
        return;
    }
        
    for (int i = 0; i < list->size; i++) {        
        if (list->elements[i].key != NULL) {
            free(list->elements[i].key);
            list->elements[i].key = NULL;
        }
        
        if (list->elements[i].value != NULL) {
            free(list->elements[i].value);
            list->elements[i].value = NULL;
        }
    }
    
    free(list->elements);
    free(list);
}

void freeRecord(Record* record) {
    if (record == NULL) {
      return;
    };
    
    if (record->key != NULL) {
        free(record->key);
    }
    if (record->value != NULL) {
        free(record->value);
    }
}


bool questionBoolean(char* message) {
    char option;
    do {
        printf("\n");
        printf("%s", message);
        printf(" [S/N] ");
        scanf("%c", &option);
        cleanBuffer();
    } while(option != 'S' && option != 's' && option != 'N' && option != 'n');

    if (option == 'N' || option == 'n') return false;

    return true;
}