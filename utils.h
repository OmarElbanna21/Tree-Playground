#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

typedef struct {
    long long comparisons;
    long long swaps;
    char opName[64];
} CompTracker;

extern CompTracker gTracker;
void resetTracker(const char *opName);
void countComparison(void);
void countSwap(void);
void printStats(void);

typedef struct {
    long long bytesAllocated;
    long long totalAllocated;
    long long allocCount;
} MemTracker;

extern MemTracker gMem;
void trackAlloc(size_t bytes);
void trackFree(size_t bytes);
void trackRealloc(size_t oldBytes, size_t newBytes);
void printMemStats(void);

#endif
