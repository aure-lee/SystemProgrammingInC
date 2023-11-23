#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TODO()\
do{\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)


#include "concur-hash.h"

// hash
/* this is the D. J. Bernstein hash function */
static unsigned int djb_hash(const char *cp) {
    unsigned hash = 5381;
    while(*cp)
        hash = 33 * hash ^ (unsigned char) *cp++;
    return hash;
}

void hash_init(hash_t *ht, int num_buckets){
    ht->mu = calloc(num_buckets, sizeof(pthread_mutex_t));
    for (int i = 0; i < num_buckets; i ++) {
        pthread_mutex_init(&ht->mu[i], 0);
    }
    ht->buckets = calloc(num_buckets, sizeof(*ht->buckets));
    ht->num_buckets = num_buckets;
}

void hash_insert(hash_t *ht, char *key, char *value) {
    // Exercise 2: complete the code below:
    // Add your code here:
    unsigned int hv = djb_hash(key);
    unsigned int n = hv % ht->num_buckets;
    pthread_mutex_lock(&ht->mu[n]);
    struct Node_t *tmp = calloc(1, sizeof(*tmp));
    tmp->key = key;
    tmp->value = value;
    tmp->next = ht->buckets[n];
    ht->buckets[n] = tmp;
    // printf("Key: %s, Value: %s\n", tmp->key, tmp->value);
    pthread_mutex_unlock(&ht->mu[n]);
    // TODO();
}

char *hash_lookup(hash_t *ht, char *key) {
    // Exercise 2: complete the code below:
    // Add your code here:
    unsigned int hv = djb_hash(key);
    unsigned int n = hv % ht->num_buckets;
    pthread_mutex_lock(&ht->mu[n]);
    struct Node_t *tmp = ht->buckets[n];
    while (tmp != NULL) {
        if (strcmp(tmp->key, key) == 0) {
            pthread_mutex_unlock(&ht->mu[n]);
            return tmp->value;
        }
        tmp = tmp->next;
    }
    pthread_mutex_unlock(&ht->mu[n]);
    return NULL;
    // TODO();
}

// return 0 for success; 1 for non-exiting "key"
int hash_delete(hash_t *ht, char *key){
    // Exercise 2: complete the code below:
    // Add your code here:
    unsigned int hv = djb_hash(key);
    unsigned int n = hv % ht->num_buckets;
    pthread_mutex_lock(&ht->mu[n]);
    struct Node_t *tmp = ht->buckets[n];
    struct Node_t *p = tmp;
    while (tmp != NULL) {
        if (strcmp(tmp->key, key) == 0) {
            if (tmp == ht->buckets[n]) {
                ht->buckets[n] = tmp->next;
            }
            else {
                p->next = tmp->next;
            }
            free(tmp);
            pthread_mutex_unlock(&ht->mu[n]);
            return 0;
        }
        if (p != tmp) p = p->next;
        tmp = tmp->next;
    }
    pthread_mutex_unlock(&ht->mu[n]);
    // TODO();
    return 1;
}

int hash_size(hash_t *ht){
    int size = 0;
    // Exercise 2: complete the code below:
    // Add your code here:
    for (int i = 0; i < ht->num_buckets; i ++) {
        struct Node_t *node = ht->buckets[i];
        pthread_mutex_lock(&ht->mu[i]);
        while (node != NULL) {
            size ++;
            node = node->next;
        }
        pthread_mutex_unlock(&ht->mu[i]);
    }
    // TODO();
    return size;
}
