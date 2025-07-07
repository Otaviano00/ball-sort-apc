#ifndef DATABASE_H
#define DATABASE_H  

typedef struct {
    char key[65];
    char value[1025];
} KeyValue;

typedef enum {
    TABLE_USERS,
    TABLE_CONFIGS,
    TABLE_INFOS,
    TABLE_UNKNOWN
} TableType;

typedef struct {
    void* key;
    void* value; // Pointer to the payload, type can vary
} Record;

typedef struct {
    char name[20];
} User;

extern int maxIdUser;

bool insertInto(char* table, void* payload); 

#endif