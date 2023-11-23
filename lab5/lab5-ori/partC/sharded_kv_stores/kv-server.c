#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




#define TABLE_SIZE 1000

typedef struct MapNode {
    char *key;
    char *value;
    struct MapNode *next;
} MapNode;

MapNode *hashTable[TABLE_SIZE];

// hash
/* D. J. Bernstein hash function */
static unsigned int djb_hash(const char *cp) {
    unsigned hash = 5381;
    while (*cp)
        hash = 33 * hash ^ (unsigned char)*cp++;
    return hash % TABLE_SIZE;
}

void put(char *key, char *value) {
    unsigned int index = djb_hash(key);
    MapNode *newNode = malloc(sizeof(MapNode));
    newNode->key = strdup(key);
    newNode->value = strdup(value);
    newNode->next = NULL;

    if (!hashTable[index]) {
        hashTable[index] = newNode;
    } else {
        MapNode *current = hashTable[index];
        while (current) {
            if (strcmp(current->key, key) == 0) {
                free(current->value);
                current->value = strdup(value);
                free(newNode->key);
                free(newNode->value);
                free(newNode);
                return ;
            }
            current = current->next;
        }
        newNode->next = hashTable[index];
        hashTable[index] = newNode;
    }
}

char *get(char *key) {
    unsigned int index = djb_hash(key);
    MapNode *current = hashTable[index];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return NULL;
}

void delete(char *key) {
    unsigned int index = djb_hash(key);
    MapNode *current = hashTable[index];
    MapNode *prev = NULL;

    while (current) {
        if (strcmp(current->key, key) == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                hashTable[index] = current->next;
            }
            free(current->key);
            free(current->value);
            free(current);
            return ;
        }
        prev = current;
        current = current->next;
    }
}

// int main() {
//     put("majingnan", "localhost:1234");
//     put("xuhao", "localhost:1234");
//     put("zhaojiacheng", "localhost:1234");
//     printf("Ma: %s\n", get("majingnan"));
//     printf("Xu: %s\n", get("xuhao"));
//     printf("Zhao: %s\n", get("zhaojiacheng"));
//     delete("xuhao");
//     printf("Xu: %s\n", get("xuhao"));
//     return 0;
// }