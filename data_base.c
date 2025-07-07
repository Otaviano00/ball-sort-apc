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

int maxIdUser = 1;

char* createFilePath(char* file) {
    char* filePath = malloc(sizeof(char) * (strlen(DIR) + strlen(file) + strlen(TYPE_FILE)));
    strcpy(filePath, DIR);
    strcat(filePath, file);
    strcat(filePath, TYPE_FILE);

    return filePath;
}

bool insertInto(char* table, void* payload) {
    char* filePath;
    Record record;
    KeyValue keyValue;
    FILE* arq;

    switch (getTableType(table)) {
        case TABLE_USERS:
            filePath = createFilePath(table);
            record = (Record) {&maxIdUser, payload};
            
            arq = fopen(filePath, "ab");

            fwrite(record.key, sizeof(int), 1, arq);
            fwrite(record.value, sizeof(User), 1, arq);
    
            fclose(arq);
            
            maxIdUser++;
            break;
        case TABLE_CONFIGS:
        case TABLE_INFOS:
            filePath = createFilePath(table);

            arq = fopen(filePath, "ab");

            fwrite((KeyValue*) payload, sizeof(KeyValue), 1, arq);

            fclose(arq);
            break;
        default:
            printf("Tabela '%s' não encontrada\n", table);
            return false;
    }
    return true;
}

bool selectFrom(char* table, int index, Record record) {

}