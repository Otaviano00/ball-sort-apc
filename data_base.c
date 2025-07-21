#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "utils.h"
#include "data_base.h"

#define DIR "tables/"
#define TYPE_FILE ".bin"

// Funções auxiliares

TableType getTableType(char* table) {
    if (strcmp(table, "users") == 0) return TABLE_USERS;
    if (strcmp(table, "configs") == 0) return TABLE_CONFIGS;
    if (strcmp(table, "levels") == 0) return TABLE_LEVEL;
    if (strcmp(table, "infos") == 0) return TABLE_INFOS;
    if (strcmp(table, "ranking") == 0) return TABLE_RANKING;
    return TABLE_UNKNOWN;
}

char* createFilePath(char* file) {
    char* filePath = malloc(sizeof(char) * (strlen(DIR) + strlen(file) + strlen(TYPE_FILE)));
    strcpy(filePath, DIR);
    strcat(filePath, file);
    strcat(filePath, TYPE_FILE);

    return filePath;
}

void cleanTable(char* table) {
    if (getTableType(table) == TABLE_UNKNOWN) return;

    char* filePath = createFilePath(table);

    FILE* arq = fopen(filePath, "wb");
    fclose(arq);

    free(filePath);
}

bool keyExists(char* table, char* key) {
    char* filePath;
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

    return found;
}


int getLastId(char* table) {
    char* filePath;
    int sizeRecord = 0;
    FILE* arq;
    int maxId = 0;
    int pos = 0;
    
    filePath = createFilePath(table);
    arq = fopen(filePath, "rb");

    switch (getTableType(table)) {
        case TABLE_USERS:
            sizeRecord = sizeof(int) + sizeof(User);
            break;
        case TABLE_LEVEL:
            sizeRecord = sizeof(int) + sizeof(Level);
            break;
        case TABLE_RANKING:
            sizeRecord = sizeof(int) + sizeof(Ranking);
            break;
        default:
            printf("Tabela '%s' nao encontrada\n", table);
    }
        
    pos = fsize(filePath) - sizeRecord;
    fseek(arq, pos, SEEK_SET);
    fread(&maxId, sizeof(int), 1, arq);    
    
    fclose(arq);
    free(filePath);
    return maxId;
}

// Funções de leitura:

Record* findUserByName(char* name) {
    char* filePath;
    FILE* arq;
    Record* record = malloc(sizeof(Record));
    bool found = false;

    filePath = createFilePath("users");

    arq = fopen(filePath, "rb");

    if (arq == NULL) {
        printf("Erro ao abrir arquivo: %s\n", filePath);
        free(filePath);
        return NULL;
    }

    User* user = malloc(sizeof(User));
    found = false;
    do {
        int temp;
        fread(&temp, sizeof(int), 1, arq);
        fread(user, sizeof(User), 1, arq);

        if (strcmp(user->name, name) == 0) {
            record->key = malloc(sizeof(int));
            record->value = malloc(sizeof(User));
            *(int*) record->key = temp;
            *(User*) record->value = *user;

            found = true;
            break;
        }
    } while(feof(arq) == 0);

    if (!found) {
        printf("Nome %s nao encontrado.\n", name);
        free(filePath);
        free(user);
        fclose(arq);
        return NULL;
    }

    fclose(arq);
 
    free(filePath);
    return record;
}


Record* findById(char* table, int id) {
    char* filePath;
    Record* record;
    FILE* arq;
    int maxId = 0;
    bool found = false;
    
    filePath = createFilePath(table);
    maxId = getLastId(table);
    if (id > maxId) return NULL;
    arq = fopen(filePath, "rb");

    if (arq == NULL) {
        printf("Erro ao abrir arquivo: %s\n", filePath);
        free(filePath);
        return NULL;
    }

    switch (getTableType(table)) {
        case TABLE_USERS:
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

            break;
        case TABLE_LEVEL:
            record = malloc(sizeof(Record));  
            record->key = malloc(sizeof(int));
            record->value = malloc(sizeof(Level));

            found = false;
            do {
                fread(record->key, sizeof(int), 1, arq);
                fread(record->value, sizeof(Level), 1, arq);

                if (*(int*) record->key == id) {
                    found = true;
                    break;
                }
            } while(feof(arq) == 0);

            break;
        default:
            printf("Tabela '%s' nao encontrada\n", table);
            found = false;
            break;
    }

    if (!found) { 
        printf("Id %s nao encontrada.\n", id);
        free(filePath);
        freeRecord(record);
        free(record);
        fclose(arq);
        return NULL;
    }

    fclose(arq);
    free(filePath);
    return record;
}

KeyValue* findByKey(char* table, char* key) {
    char* filePath;
    int sizeRecord = 0;
    KeyValue* keyValue;
    FILE* arq;
    bool found = false;

    if (getTableType(table) == TABLE_UNKNOWN) {
        printf("Tabela '%s' nao encontrada\n", table);
        return NULL;
    }

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

    if (!found) {
        printf("Key %s nao encontrada.\n", key);
        free(filePath);
        freeRecord((Record*) keyValue);
        fclose(arq);
        return NULL;
    }

    fclose(arq);
    free(filePath);
    return keyValue;
}

List* findAll(char* table) {
    char* filePath;
    List* list;
    FILE* arq;
    int sizeRecord = 0;
    int numOfRecords = 0;
    
    filePath = createFilePath(table);
    arq = fopen(filePath, "rb");

    if (arq == NULL) {
        printf("Erro ao abrir arquivo: %s\n", filePath);
        free(filePath);
        return NULL;
    }

    list = malloc(sizeof(List));

    switch (getTableType(table)) {
        case TABLE_LEVEL:
            sizeRecord = sizeof(int) + sizeof(Level);
            numOfRecords = fsize(filePath) / sizeRecord;
                
            list->elements = malloc(sizeof(Record) * numOfRecords);
            for (int i = 0; i < numOfRecords; i++) {
                list->elements[i].key = malloc(sizeof(int));
                list->elements[i].value = malloc(sizeof(Level));
            }
            list->size = numOfRecords;

            for (int i = 0; i < numOfRecords; i++) {
                fread((int*) list->elements[i].key, sizeof(int), 1, arq);
                fread((Level*) list->elements[i].value, sizeof(Level), 1, arq);
            }

            break;

        case TABLE_USERS:
            sizeRecord = sizeof(int) + sizeof(User);
            numOfRecords = fsize(filePath) / sizeRecord;
                
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

            break;

        case TABLE_RANKING:
            sizeRecord = sizeof(int) + sizeof(Ranking);
            numOfRecords = fsize(filePath) / sizeRecord;
                
            list->elements = malloc(sizeof(Record) * numOfRecords);
            for (int i = 0; i < numOfRecords; i++) {
                list->elements[i].key = malloc(sizeof(int));
                list->elements[i].value = malloc(sizeof(Ranking));
            }
            list->size = numOfRecords;

            for (int i = 0; i < numOfRecords; i++) {
                fread((int*) list->elements[i].key, sizeof(int), 1, arq);
                fread((Ranking*) list->elements[i].value, sizeof(Ranking), 1, arq);
            }

            break;
        case TABLE_INFOS:
        case TABLE_CONFIGS:
            sizeRecord = sizeof(KeyValue);
            numOfRecords = fsize(filePath) / sizeRecord;
            
            list->elements = malloc(sizeof(Record) * numOfRecords);
            for (int i = 0; i < numOfRecords; i++) {
                list->elements[i].key = malloc(sizeof(char) * 65);
                list->elements[i].value = malloc(sizeof(char) * 1025);
            }
            list->size = numOfRecords;

             for (int i = 0; i < numOfRecords; i++) {
                fread((char*) list->elements[i].key, 65, 1, arq);
                fread((char*) list->elements[i].value, 1025, 1, arq);
            }
            break;
        default:
            printf("Tabela '%s' nao encontrada\n", table);
            return NULL;
    }

    fclose(arq);
    free(filePath);
    return list;
}

bool persist(char* table, void* payload) {
    char* filePath;
    Record record;
    KeyValue keyValue;
    FILE* arq;
    int maxId = 0;

    bool ret = true;
    
    filePath = createFilePath(table);
    maxId = getLastId(table) + 1;
    record = (Record) {&maxId, payload};
    arq = fopen(filePath, "ab");

    switch (getTableType(table)) {
        case TABLE_USERS:

            if (findUserByName(((User*) payload)->name) != NULL) {
                // printf("O nickname '%s' jah estah cadastrado. Use outro por favor.\n", ((User*) payload)->name);
                ret = false;
                break;
            }

            fwrite(record.key, sizeof(int), 1, arq);
            fwrite(record.value, sizeof(User), 1, arq);
    
            break;
        case TABLE_RANKING:
            fwrite(record.key, sizeof(int), 1, arq);
            fwrite(record.value, sizeof(Ranking), 1, arq);

            break;
        case TABLE_LEVEL:            
            fwrite(record.key, sizeof(int), 1, arq);
            fwrite(record.value, sizeof(Level), 1, arq);
              
            break;
        case TABLE_CONFIGS:
        case TABLE_INFOS:            
            if (keyExists(table, ((KeyValue*) payload)->key)) {
                printf("A Key '%s' jah existe. Use outra key.\n", ((KeyValue*) payload)->key);
                ret = false;
                break;
            };
                        
            fwrite((KeyValue*) payload, sizeof(KeyValue), 1, arq);
            
            fclose(arq);
            break;
        default:
            printf("Tabela '%s' nao encontrada\n", table);
            ret = false;
            break;
    }

    fclose(arq);
    free(filePath);
    return ret;
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
                int id;
                fgetpos(arq, &fpos);
                fread(&id, sizeof(int), 1, arq);  
                fread(&temp, sizeof(User), 1, arq);
                
                if (id == *(int*) payload->key) {
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

            arq = fopen(filePath, "r+b");

            keyValue = ((KeyValue*) payload);

            found = false;
            do {
                fgetpos(arq, &fpos);
                KeyValue temp;
                fread(&temp, sizeof(KeyValue), 1, arq);
                
                if (strcmp(temp.key, keyValue->key) == 0) {
                    found = true;
                    break;
                };
            } while(feof(arq) == 0);

            if (!found) {
                free(filePath);
                fclose(arq);
                return false;
            }

            fsetpos(arq, &fpos);

            fwrite(keyValue, sizeof(KeyValue), 1, arq);

            fclose(arq);
            break;
        default:
            printf("Tabela '%s' nao encontrada\n", table);
            return false;
    }
    free(filePath);
    return true;
}

bool deleteById(char* table, int id) {
    char* filePath;
    FILE* arq;
    int maxId = 0;
    List* allRecords;
    bool ret = true;
    
    filePath = createFilePath(table);
    maxId = getLastId(table);
    if (id > maxId) return false;

    allRecords = findAll(table);

    arq = fopen(filePath, "wb");

    switch (getTableType(table)) {
        case TABLE_USERS:
            for (int i = 0; i < allRecords->size; i++) {
                if (*(int*) allRecords->elements[i].key == id) continue;

                fwrite(allRecords->elements[i].key, sizeof(int), 1, arq);
                fwrite(allRecords->elements[i].value, sizeof(User), 1, arq);
            }
            
            printf("Usuario deletado com sucesso!");
            break;
        default:
            printf("Tabela '%s' nao encontrada\n", table);
            ret = false;
            break;
        }
                
    fclose(arq);
    freeList(allRecords);
    free(filePath);
    return ret;
}

bool deleteByKey(char* table, char* key) {
    char* filePath;
    Record* record;
    int sizeRecord = 0;
    FILE* arq;
    int maxId = 0;
    List* allRecords;

    if (getTableType(table) == TABLE_UNKNOWN) {
        printf("Tabela %s nao encontrada.\n", table);
        return false;
    }

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
    free(filePath);
    return true;
}