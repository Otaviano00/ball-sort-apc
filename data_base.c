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

bool persist(char* table, void* payload) {
    char* filePath;
    Record record;
    int sizeRecord = 0;
    KeyValue keyValue;
    FILE* arq;
    int maxId = 0;

    switch (getTableType(table)) {
        case TABLE_USERS:
            filePath = createFilePath(table);
    
            sizeRecord = sizeof(int) + sizeof(User);

            maxId = fsize(filePath) / sizeRecord;

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
            printf("Tabela '%s' não encontrada\n", table);
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

    switch (getTableType(table)) {
        case TABLE_USERS:
            filePath = createFilePath(table);
            sizeRecord = sizeof(int) + sizeof(User);
            maxId = fsize(filePath) / sizeRecord;
      
            if (id > maxId) return NULL;

            filePath = createFilePath(table);

            arq = fopen(filePath, "rb");

            if (arq == NULL) {
                printf("Erro ao abrir arquivo: %s\n", filePath);
                free(filePath);
                return NULL;
            }

            fseek(arq, sizeRecord * id, SEEK_SET);

            record = malloc(sizeof(Record));
            
            record->key = malloc(sizeof(int));
            record->value = malloc(sizeof(User));

            fread(record->key, sizeof(int), 1, arq);
            fread(record->value, sizeof(User), 1, arq);
            
            printf("Id: %d e nome: %s\n", *(int*) record->key, (*(User*) record->value).name);

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
                free(keyValue);
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
    int maxId = 0;

    switch (getTableType(table)) {
        case TABLE_USERS:
            filePath = createFilePath(table);
            sizeRecord = sizeof(int) + sizeof(User);
            maxId = fsize(filePath) / sizeRecord;
      
            filePath = createFilePath(table);

            arq = fopen(filePath, "rb");

            if (arq == NULL) {
                printf("Erro ao abrir arquivo: %s\n", filePath);
                free(filePath);
                return NULL;
            }

            list = malloc(sizeof(List));
            
            list->elements = malloc(sizeof(Record) * maxId);
            for (int i = 0; i < maxId; i++) {
                list->elements[i].key = malloc(sizeof(int));
                list->elements[i].value = malloc(sizeof(User));
            }
            list->size = maxId;

            for (int i = 0; i < maxId; i++) {
                printf("oi%d\n", maxId);
                fread((int*) list->elements[i].key, sizeof(int), 1, arq);
                fread((User*) list->elements[i].value, sizeof(User), 1, arq);
            }

            fclose(arq);
            break;
        case TABLE_INFOS:
        case TABLE_CONFIGS:
            filePath = createFilePath(table);
            sizeRecord = sizeof(KeyValue);
            maxId = fsize(filePath) / sizeRecord;

            filePath = createFilePath(table);

            arq = fopen(filePath, "rb");

            if (arq == NULL) {
                printf("Erro ao abrir arquivo: %s\n", filePath);
                free(filePath);
                return NULL;
            }

            list = malloc(sizeof(List));
            
            list->elements = malloc(sizeof(Record) * maxId);
            list->size = maxId;

            for (int i = 0; i < maxId; i++) {
                fread(&(list->elements[i]), sizeof(KeyValue), 1, arq);
            }

            fclose(arq);
            break;
        default:
            printf("Tabela '%s' nao encontrada\n", table);
    }
    free(filePath);
    return list;
}

bool update(char* table, void* payload) {
    char* filePath;
    int sizeRecord = 0;
    Record* record;
    KeyValue* keyValue;
    FILE* arq;
    int maxId = 0;
    int pos = 0;

    switch (getTableType(table)) {
        case TABLE_USERS:
            filePath = createFilePath(table);
            sizeRecord = sizeof(int) + sizeof(User);

            arq = fopen(filePath, "r+b");

            record = ((Record*) payload);

            pos = (*(int*) record->key) * sizeRecord;

            fseek(arq, pos, SEEK_SET);

            fwrite(record->key, sizeof(int), 1, arq);
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
    return record;
}
