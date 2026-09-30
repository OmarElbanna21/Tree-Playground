#ifndef HASHTABLE_H
#define HASHTABLE_H

#define HT_DEFAULT_CAP 11

typedef enum { EMPTY, OCCUPIED, DELETED } SlotStatus;

typedef struct {
    int *keys;
    SlotStatus *status;
    int capacity;
    int size;
} HashLP;

HashLP *createHashLP(int capacity);
void    destroyHashLP(HashLP **h);
int     lpInsert(HashLP *h, int key, int stepByStep);
int     lpSearch(HashLP *h, int key, int stepByStep);
int     lpDelete(HashLP *h, int key, int stepByStep);
void    printHashLP(const HashLP *h);
void    lpStats(const HashLP *h);
typedef struct ChainNode {
    int key;
    struct ChainNode *next;
} ChainNode;

typedef struct {
    ChainNode **buckets;
    int capacity;
    int size;
} HashChain;

HashChain *createHashChain(int capacity);
void       destroyHashChain(HashChain **h);
int        chainInsert(HashChain *h, int key, int stepByStep);
int        chainSearch(HashChain *h, int key, int stepByStep);
int        chainDelete(HashChain *h, int key, int stepByStep);
void       printHashChain(const HashChain *h);
void       chainStats(const HashChain *h);
int hashFn(int key, int capacity);

#endif
