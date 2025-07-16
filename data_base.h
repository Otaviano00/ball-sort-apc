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
    TABLE_RANKING,
    TABLE_LEVEL,
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
    int points;
} User;

typedef struct {
    int size;
    char balls[10];
    bool complete;
} Stack;

typedef struct {
    int order;
    Stack columns[10];
    int maxHeight;
    int numColumns;
    int numEmptyColumns;
} Level;


bool persist(char* table, void* payload); 
Record* findById(char* table, int id);
User* findUserByName(char* name);
KeyValue* findByKey(char* table, char* key);
List* findAll(char* table);
bool update(char* table, Record* record);
bool deleteById(char* table, int id);
bool deleteByKey(char* table, char* key);
void cleanTable(char* table);

#endif