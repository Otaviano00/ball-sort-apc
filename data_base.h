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
    Record* elements;
    int size;
} List;

typedef struct {
    char name[20];
    char cargo[20];
} User;

bool persist(char* table, void* payload); 
Record* findById(char* table, int id);
KeyValue* findByKey(char* table, char* key);
List* findAll(char* table);
bool update(char* table, void* record);

#endif