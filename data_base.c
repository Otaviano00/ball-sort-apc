#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "utils.h"
#include "data_base.h"

#define DIR "tables/"
#define TYPE_FILE ".bin"

TableType getTableType(char* table) {
    if (strcmp(table, "users") == 0) return TABLE_USERS;
    if (strcmp(table, "configs") == 0) return TABLE_CONFIGS;
    if (strcmp(table, "infos") == 0) return TABLE_INFOS;
    return TABLE_UNKNOWN;
}

char* createFilePath(char* file) {
    char* filePath = malloc(sizeof(char) * (strlen(DIR) + strlen(file) + strlen(TYPE_FILE)));
    strcpy(filePath, DIR);
    strcat(filePath, file);
    strcat(filePath, TYPE_FILE);

    return filePath;
}

bool keyExists(char* table, char* key) {
    char* filePath;
    int numOfRecords = 0;
    KeyValue* keyValue;
    FILE* arq;
    bool found = false;

    filePath = createFilePath(table);

    arq = fopen(filePath, "rb");

    if (arq == NULL) {
        printf("Erro ao abrir arquivo: %s\n", filePath);
        free(filePath);
        return NULL;
    }

    keyValue = malloc(sizeof(KeyValue));
    found = false;
    do {
        fread(keyValue, sizeof(KeyValue), 1, arq);

        if (strcmp(keyValue->key, key) == 0) {
            found = true;
            break;
        }
    } while(feof(arq) == 0);

    free(filePath);
    free(keyValue);
    fclose(arq);

    // Se encontrou a chave, retorna true. Se não retorna false
    return found;
}

int getLastId(char* table) {
    char* filePath;
    Record record;
    int sizeRecord = 0;
    KeyValue keyValue;
    FILE* arq;
    int maxId = 0;
    int pos = 0;
    fpos_t fpos;

    switch (getTableType(table)) {
        case TABLE_USERS:
            filePath = createFilePath(table);
    
            sizeRecord = sizeof(int) + sizeof(User);

            pos = fsize(filePath) - sizeRecord;

            arq = fopen(filePath, "rb");
            
            fseek(arq, pos, SEEK_SET);
            
            fread(&maxId, sizeof(int), 1, arq);    

            fclose(arq);
            break;
        default:
            printf("Tabela '%s' nao encontrada\n", table);
    }
    free(filePath);
    return maxId;
}

bool persist(char* table, void* payload) {
    char* filePath;
    Record record;
    int sizeRecord = 0;
    KeyValue keyValue;
    FILE* arq;
    int maxId = 0;
    int pos = 0;
    fpos_t fpos;

    switch (getTableType(table)) {
        case TABLE_USERS:
            filePath = createFilePath(table);

            maxId = getLastId(table) + 1;

            record = (Record) {&maxId, payload};
            
            arq = fopen(filePath, "ab");

            fwrite(record.key, sizeof(int), 1, arq);
            fwrite(record.value, sizeof(User), 1, arq);
    
            fclose(arq);
            break;
        case TABLE_CONFIGS:
        case TABLE_INFOS:
            filePath = createFilePath(table);

            if (keyExists(table, ((KeyValue*) payload)->key)) {
                printf("A Key '%s' jah existe. Use outra key.\n", ((KeyValue*) payload)->key);
                free(filePath);
                return false;
            };

            arq = fopen(filePath, "ab");

            fwrite((KeyValue*) payload, sizeof(KeyValue), 1, arq);

            fclose(arq);
            break;
        default:
            printf("Tabela '%s' nao encontrada\n", table);
            return false;
    }
    free(filePath);
    return true;
}

Record* findById(char* table, int id) {
    char* filePath;
    Record* record;
    int sizeRecord = 0;
    FILE* arq;
    int maxId = 0;
    bool found = false;

    switch (getTableType(table)) {
        case TABLE_USERS:
            filePath = createFilePath(table);

            maxId = getLastId(table);
      
            if (id > maxId) return NULL;

            filePath = createFilePath(table);

            arq = fopen(filePath, "rb");

            if (arq == NULL) {
                printf("Erro ao abrir arquivo: %s\n", filePath);
                free(filePath);
                return NULL;
            }

            record = malloc(sizeof(Record));  
            record->key = malloc(sizeof(int));
            record->value = malloc(sizeof(User));

            found = false;
            do {
                fread(record->key, sizeof(int), 1, arq);
                fread(record->value, sizeof(User), 1, arq);

                if (*(int*) record->key == id) {
                    found = true;
                    break;
                }
            } while(feof(arq) == 0);

            if (found) {
                printf("Id: %d e nome: %s\n", *(int*) record->key, (*(User*) record->value).name);
            } else { 
                printf("Id %s nao encontrada.\n", id);
                free(filePath);
                freeRecord(record);
                free(record);
                fclose(arq);
                return NULL;
            }

            fclose(arq);
            break;
        default:
            printf("Tabela '%s' nao encontrada\n", table);
    }
    free(filePath);
    return record;
}

KeyValue* findByKey(char* table, char* key) {
    char* filePath;
    int sizeRecord = 0;
    int numOfRecords = 0;
    KeyValue* keyValue;
    FILE* arq;
    bool found = false;
    int maxId = 0;

    switch (getTableType(table)) {
        case TABLE_INFOS:
        case TABLE_CONFIGS:
            filePath = createFilePath(table);
            sizeRecord = sizeof(KeyValue);

            arq = fopen(filePath, "rb");

            if (arq == NULL) {
                printf("Erro ao abrir arquivo: %s\n", filePath);
                free(filePath);
                return NULL;
            }

            keyValue = malloc(sizeof(KeyValue));
            found = false;
            do {
                fread(keyValue, sizeof(KeyValue), 1, arq);

                if (strcmp(keyValue->key, key) == 0) {
                    found = true;
                    break;
                }
            } while(feof(arq) == 0);

            if (found) {
                printf("Key: %s e Value: %s\n", keyValue->key, keyValue->value);
            } else {
                printf("Key %s nao encontrada.\n", key);
                free(filePath);
                freeRecord((Record*) keyValue);
                fclose(arq);
                return NULL;
            }

            fclose(arq);
            break;
        default:
            printf("Tabela '%s' nao encontrada\n", table);
    }
    free(filePath);
    return keyValue;
}

List* findAll(char* table) {
    char* filePath;
    Record* record;
    KeyValue* keyValue;
    List* list;
    FILE* arq;
    int sizeRecord = 0;
    int numOfRecords = 0;
    bool found = false;
    
    switch (getTableType(table)) {
        case TABLE_USERS:
            filePath = createFilePath(table);
            sizeRecord = sizeof(int) + sizeof(User);
            numOfRecords = fsize(filePath) / sizeRecord;
      
            filePath = createFilePath(table);

            arq = fopen(filePath, "rb");

            if (arq == NULL) {
                printf("Erro ao abrir arquivo: %s\n", filePath);
                free(filePath);
                return NULL;
            }

            list = malloc(sizeof(List));
            
            list->elements = malloc(sizeof(Record) * numOfRecords);
            for (int i = 0; i < numOfRecords; i++) {
                list->elements[i].key = malloc(sizeof(int));
                list->elements[i].value = malloc(sizeof(User));
            }
            list->size = numOfRecords;

            for (int i = 0; i < numOfRecords; i++) {
                fread((int*) list->elements[i].key, sizeof(int), 1, arq);
                fread((User*) list->elements[i].value, sizeof(User), 1, arq);
            }

            fclose(arq);
            break;
        case TABLE_INFOS:
        case TABLE_CONFIGS:
            filePath = createFilePath(table);
            sizeRecord = sizeof(KeyValue);
            numOfRecords = fsize(filePath) / sizeRecord;

            filePath = createFilePath(table);

            arq = fopen(filePath, "rb");

            if (arq == NULL) {
                printf("Erro ao abrir arquivo: %s\n", filePath);
                free(filePath);
                return NULL;
            }

            list = malloc(sizeof(List));
            
            list->elements = malloc(sizeof(Record) * numOfRecords);
            for (int i = 0; i < numOfRecords; i++) {
                list->elements[i].key = malloc(sizeof(char) * 65);
                list->elements[i].value = malloc(sizeof(char) * 1025);
            }
            list->size = numOfRecords;

             for (int i = 0; i < numOfRecords; i++) {
                fread((char*) list->elements[i].key, 65, 1, arq);
                fread((char*) list->elements[i].value, 1025, 1, arq);
                
                printf("chave: %s e valor: %s\n", list->elements[i].key, list->elements[i].value);
            }

            fclose(arq);
            break;
        default:
            printf("Tabela '%s' nao encontrada\n", table);
    }
    free(filePath);
    return list;
}

bool update(char* table, Record* payload) {
    char* filePath;
    int sizeRecord = 0;
    Record* record;
    KeyValue* keyValue;
    FILE* arq;
    int maxId = -1;
    int pos = 0;
    fpos_t fpos;
    bool found = false;

    switch (getTableType(table)) {
        case TABLE_USERS:
            filePath = createFilePath(table);

            arq = fopen(filePath, "rb");
            
            found = false;
            do {
                User temp;
                fgetpos(arq, &fpos);
                fread(&maxId, sizeof(int), 1, arq);  
                fread(&temp, sizeof(User), 1, arq);
                
                if (maxId == *(int*) payload->key) {
                    found = true;
                    break;
                }
            } while(feof(arq) == 0);

            fclose(arq);

            if (!found) return false; 


            arq = fopen(filePath, "r+b");

            fsetpos(arq, &fpos);
            fseek(arq, sizeof(int), SEEK_CUR);

            record = ((Record*) payload);
            fwrite(record->value, sizeof(User), 1, arq);


            fclose(arq);
            break;
        case TABLE_CONFIGS:
        case TABLE_INFOS:
            filePath = createFilePath(table);
            sizeRecord = sizeof(KeyValue);
            maxId = fsize(filePath) / sizeRecord;

            arq = fopen(filePath, "r+b");

            keyValue = ((KeyValue*) payload);

            fpos_t pos;
            for (int i = 0; i < maxId; i++) {
                fgetpos(arq, &pos);
                KeyValue temp;
                fread(&temp, sizeof(KeyValue), 1, arq);

                if (strcmp(temp.key, keyValue->key) == 0) break;
            }

            fsetpos(arq, &pos);

            fwrite(keyValue, sizeof(KeyValue), 1, arq);

            fclose(arq);
            break;
        default:
            printf("Tabela '%s' nao encontrada\n", table);
    }
    free(filePath);
    return true;
}

bool deleteById(char* table, int id) {
    char* filePath;
    Record* record;
    int sizeRecord = 0;
    FILE* arq;
    int maxId = 0;
    List* allRecords;

    switch (getTableType(table)) {
        case TABLE_USERS:
            filePath = createFilePath(table);
            maxId = getLastId(table);
            if (id > maxId) return false;

            allRecords = findAll(table);

            arq = fopen(filePath, "wb");

            for (int i = 0; i < allRecords->size; i++) {
                if (*(int*) allRecords->elements[i].key == id) continue;

                fwrite(allRecords->elements[i].key, sizeof(int), 1, arq);
                fwrite(allRecords->elements[i].value, sizeof(User), 1, arq);
            }
            
            printf("Usuario deletado com sucesso!");
            
            fclose(arq);
            freeList(allRecords);
            break;
        default:
            printf("Tabela '%s' nao encontrada\n", table);
        }
    free(filePath);
    return true;
}

bool deleteByKey(char* table, char* key) {
char* filePath;
    Record* record;
    int sizeRecord = 0;
    FILE* arq;
    int maxId = 0;
    List* allRecords;

    switch (getTableType(table)) {
        case TABLE_CONFIGS:
        case TABLE_INFOS:
            filePath = createFilePath(table);
        
            if (!keyExists(table, key)) return false;

            allRecords = findAll(table);

            arq = fopen(filePath, "wb");

            for (int i = 0; i < allRecords->size; i++) {
                if (strcmp((char*) allRecords->elements[i].key, key) == 0) continue;

                fwrite((char*) allRecords->elements[i].key, sizeof(char) * 65, 1, arq);
                fwrite((char*) allRecords->elements[i].value, sizeof(char) * 1025, 1, arq);
            }

            printf("Registro deletado com sucesso!");
            
            fclose(arq);
            freeList(allRecords);
            break;
        default:
            printf("Tabela '%s' nao encontrada\n", table);
        }
    free(filePath);
    return true;
}