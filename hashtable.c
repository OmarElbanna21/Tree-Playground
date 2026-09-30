#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"
#include "utils.h"

int hashFn(int key, int capacity) {
    return ((key % capacity) + capacity) % capacity;
}

HashLP *createHashLP(int capacity) {
    HashLP *h;

    if (capacity <= 0) {
        capacity = HT_DEFAULT_CAP;
    }

    h = (HashLP *)malloc(sizeof(HashLP));
    if (!h) {
        return NULL;
    }
    trackAlloc(sizeof(HashLP));

    h->keys = (int *)malloc(sizeof(int) * capacity);
    if (!h->keys) {
        trackFree(sizeof(HashLP));
        free(h);
        return NULL;
    }
    trackAlloc(sizeof(int) * capacity);

    h->status = (SlotStatus *)malloc(sizeof(SlotStatus) * capacity);
    if (!h->status) {
        trackFree(sizeof(int) * capacity);
        free(h->keys);
        trackFree(sizeof(HashLP));
        free(h);
        return NULL;
    }
    trackAlloc(sizeof(SlotStatus) * capacity);

    for (int i = 0; i < capacity; i++) {
        h->keys[i] = 0;
        h->status[i] = EMPTY;
    }

    h->capacity = capacity;
    h->size = 0;
    return h;
}

void destroyHashLP(HashLP **h) {
    if (!h || !*h) {
        return;
    }

    trackFree(sizeof(int) * (size_t)(*h)->capacity);
    free((*h)->keys);
    trackFree(sizeof(SlotStatus) * (size_t)(*h)->capacity);
    free((*h)->status);
    trackFree(sizeof(HashLP));
    free(*h);
    *h = NULL;
}

void printHashLP(const HashLP *h) {
    int i;

    if (!h) {
        return;
    }

    printf("\n  Linear Probing Table (capacity = %d)\n", h->capacity);
    printf("  ------------------------------------\n");
    for (i = 0; i < h->capacity; i++) {
        const char *state = h->status[i] == EMPTY ? "EMPTY" :
                            h->status[i] == OCCUPIED ? "OCCUPIED" : "DELETED";

        if (h->status[i] == OCCUPIED) {
            printf("  [%2d] %-8s %d\n", i, state, h->keys[i]);
        } else {
            printf("  [%2d] %-8s -\n", i, state);
        }
    }
    printf("  Load factor: %.2f (%d/%d)\n\n", (float)h->size / h->capacity, h->size, h->capacity);
}

int lpInsert(HashLP *h, int key, int stepByStep) {
    int idx;
    int start;
    int firstDeleted = -1;

    if (!h) {
        return 0;
    }
    if (h->size >= h->capacity) {
        printf("  Table is full!\n");
        return 0;
    }

    resetTracker("LP Insert");
    idx = hashFn(key, h->capacity);
    start = idx;

    if (stepByStep) {
        printf("  h(%d) = %d\n", key, idx);
    }

    do {
        countComparison();

        if (h->status[idx] == OCCUPIED) {
            if (h->keys[idx] == key) {
                printf("  Key %d already exists.\n", key);
                printStats();
                return 0;
            }
            if (stepByStep) {
                printf("  Slot %d occupied -> probe next.\n", idx);
            }
        } else if (h->status[idx] == DELETED) {
            if (firstDeleted == -1) {
                firstDeleted = idx;
            }
            if (stepByStep) {
                printf("  Slot %d deleted -> keep probing.\n", idx);
            }
        } else {
            int target = firstDeleted != -1 ? firstDeleted : idx;

            h->keys[target] = key;
            h->status[target] = OCCUPIED;
            h->size++;
            if (stepByStep) {
                printf("  Inserted at slot %d.\n", target);
            }
            printHashLP(h);
            printStats();
            return 1;
        }

        idx = (idx + 1) % h->capacity;
    } while (idx != start);

    if (firstDeleted != -1) {
        h->keys[firstDeleted] = key;
        h->status[firstDeleted] = OCCUPIED;
        h->size++;
        if (stepByStep) {
            printf("  Inserted at slot %d.\n", firstDeleted);
        }
        printHashLP(h);
        printStats();
        return 1;
    }

    printf("  Table full after probing!\n");
    printStats();
    return 0;
}

int lpSearch(HashLP *h, int key, int stepByStep) {
    int idx;
    int start;

    if (!h) {
        return -1;
    }

    resetTracker("LP Search");
    idx = hashFn(key, h->capacity);
    start = idx;

    if (stepByStep) {
        printf("  h(%d) = %d\n", key, idx);
    }

    do {
        countComparison();

        if (h->status[idx] == EMPTY) {
            printf("  Empty slot at %d -> key %d not found.\n", idx, key);
            printStats();
            return -1;
        }
        if (h->status[idx] == OCCUPIED && h->keys[idx] == key) {
            printf("  Found key %d at slot %d.\n", key, idx);
            printStats();
            return idx;
        }
        if (stepByStep) {
            printf("  Slot %d -> keep probing.\n", idx);
        }
        idx = (idx + 1) % h->capacity;
    } while (idx != start);

    printf("  Key %d not found.\n", key);
    printStats();
    return -1;
}

int lpDelete(HashLP *h, int key, int stepByStep) {
    int idx;
    int start;

    if (!h) {
        return 0;
    }

    resetTracker("LP Delete");
    idx = hashFn(key, h->capacity);
    start = idx;

    if (stepByStep) {
        printf("  h(%d) = %d\n", key, idx);
    }

    do {
        countComparison();

        if (h->status[idx] == EMPTY) {
            printf("  Key %d not found.\n", key);
            printStats();
            return 0;
        }
        if (h->status[idx] == OCCUPIED && h->keys[idx] == key) {
            h->status[idx] = DELETED;
            h->size--;
            printf("  Deleted key %d from slot %d.\n", key, idx);
            printHashLP(h);
            printStats();
            return 1;
        }
        idx = (idx + 1) % h->capacity;
    } while (idx != start);

    printf("  Key %d not found.\n", key);
    printStats();
    return 0;
}

void lpStats(const HashLP *h) {
    int occupied = 0;
    int deleted = 0;
    int i;

    if (!h) {
        return;
    }

    for (i = 0; i < h->capacity; i++) {
        if (h->status[i] == OCCUPIED) {
            occupied++;
        } else if (h->status[i] == DELETED) {
            deleted++;
        }
    }

    printf("\n  Linear Probing Stats\n");
    printf("  Capacity : %d\n", h->capacity);
    printf("  Occupied : %d\n", occupied);
    printf("  Deleted  : %d\n", deleted);
    printf("  Load     : %.2f\n\n", (float)occupied / h->capacity);
}

HashChain *createHashChain(int capacity) {
    HashChain *h;

    if (capacity <= 0) {
        capacity = HT_DEFAULT_CAP;
    }

    h = (HashChain *)malloc(sizeof(HashChain));
    if (!h) {
        return NULL;
    }
    trackAlloc(sizeof(HashChain));

    h->buckets = (ChainNode **)calloc((size_t)capacity, sizeof(ChainNode *));
    if (!h->buckets) {
        trackFree(sizeof(HashChain));
        free(h);
        return NULL;
    }
    trackAlloc(sizeof(ChainNode *) * capacity);

    h->capacity = capacity;
    h->size = 0;
    return h;
}

void destroyHashChain(HashChain **h) {
    int i;

    if (!h || !*h) {
        return;
    }

    for (i = 0; i < (*h)->capacity; i++) {
        ChainNode *cur = (*h)->buckets[i];

        while (cur) {
            ChainNode *next = cur->next;
            trackFree(sizeof(ChainNode));
            free(cur);
            cur = next;
        }
    }

    trackFree(sizeof(ChainNode *) * (size_t)(*h)->capacity);
    free((*h)->buckets);
    trackFree(sizeof(HashChain));
    free(*h);
    *h = NULL;
}

void printHashChain(const HashChain *h) {
    int i;

    if (!h) {
        return;
    }

    printf("\n  Separate Chaining Table (capacity = %d)\n", h->capacity);
    for (i = 0; i < h->capacity; i++) {
        ChainNode *cur = h->buckets[i];

        printf("  [%2d] -> ", i);
        if (!cur) {
            printf("NULL");
        }
        while (cur) {
            printf("[%d] -> ", cur->key);
            cur = cur->next;
        }
        printf("NULL\n");
    }
    printf("  Load factor: %.2f (%d/%d)\n\n", (float)h->size / h->capacity, h->size, h->capacity);
}

int chainInsert(HashChain *h, int key, int stepByStep) {
    ChainNode *cur;
    ChainNode *node;
    int idx;

    if (!h) {
        return 0;
    }

    resetTracker("Chain Insert");
    idx = hashFn(key, h->capacity);
    if (stepByStep) {
        printf("  h(%d) = %d\n", key, idx);
    }

    cur = h->buckets[idx];
    while (cur) {
        countComparison();
        if (cur->key == key) {
            printf("  Key %d already exists.\n", key);
            printStats();
            return 0;
        }
        cur = cur->next;
    }

    node = (ChainNode *)malloc(sizeof(ChainNode));
    if (!node) {
        return 0;
    }
    trackAlloc(sizeof(ChainNode));

    node->key = key;
    node->next = h->buckets[idx];
    h->buckets[idx] = node;
    h->size++;

    if (stepByStep) {
        printf("  Inserted at bucket %d.\n", idx);
    }
    printHashChain(h);
    printStats();
    return 1;
}

int chainSearch(HashChain *h, int key, int stepByStep) {
    ChainNode *cur;
    int idx;
    int pos = 0;

    if (!h) {
        return 0;
    }

    resetTracker("Chain Search");
    idx = hashFn(key, h->capacity);
    if (stepByStep) {
        printf("  h(%d) = %d -> search chain.\n", key, idx);
    }

    cur = h->buckets[idx];
    while (cur) {
        countComparison();
        if (cur->key == key) {
            printf("  Found key %d at bucket %d, position %d.\n", key, idx, pos);
            printStats();
            return 1;
        }
        cur = cur->next;
        pos++;
    }

    printf("  Key %d not found.\n", key);
    printStats();
    return 0;
}

int chainDelete(HashChain *h, int key, int stepByStep) {
    ChainNode *cur;
    ChainNode *prev = NULL;
    int idx;

    if (!h) {
        return 0;
    }

    resetTracker("Chain Delete");
    idx = hashFn(key, h->capacity);
    if (stepByStep) {
        printf("  h(%d) = %d\n", key, idx);
    }

    cur = h->buckets[idx];
    while (cur) {
        countComparison();
        if (cur->key == key) {
            if (prev) {
                prev->next = cur->next;
            } else {
                h->buckets[idx] = cur->next;
            }
            trackFree(sizeof(ChainNode));
            free(cur);
            h->size--;
            printf("  Deleted key %d from bucket %d.\n", key, idx);
            printHashChain(h);
            printStats();
            return 1;
        }
        prev = cur;
        cur = cur->next;
    }

    printf("  Key %d not found.\n", key);
    printStats();
    return 0;
}

void chainStats(const HashChain *h) {
    int i;
    int maxChain = 0;
    int emptyBuckets = 0;

    if (!h) {
        return;
    }

    for (i = 0; i < h->capacity; i++) {
        int len = 0;
        ChainNode *cur = h->buckets[i];

        while (cur) {
            len++;
            cur = cur->next;
        }

        if (len == 0) {
            emptyBuckets++;
        }
        if (len > maxChain) {
            maxChain = len;
        }
    }

    printf("\n  Chaining Stats\n");
    printf("  Capacity      : %d\n", h->capacity);
    printf("  Total keys    : %d\n", h->size);
    printf("  Empty buckets : %d\n", emptyBuckets);
    printf("  Longest chain : %d\n", maxChain);
    printf("  Load factor   : %.2f\n\n", (float)h->size / h->capacity);
}
