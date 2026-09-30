#include <stdio.h>
#include <string.h>
#include "utils.h"

CompTracker gTracker;
MemTracker  gMem;

void resetTracker(const char *opName) {
    gTracker.comparisons = 0;
    gTracker.swaps       = 0;
    strncpy(gTracker.opName, opName ? opName : "?", 63);
    gTracker.opName[63]  = '\0';
}

void countComparison(void) {
    gTracker.comparisons++;
}

void countSwap(void) {
    gTracker.swaps++;
}

void printStats(void) {
    printf("\n  Complexity (%s)\n", gTracker.opName);
    printf("  Comparisons : %lld\n", gTracker.comparisons);
    printf("  Swaps/Moves : %lld\n\n", gTracker.swaps);
}

void trackAlloc(size_t bytes) {
    gMem.bytesAllocated += (long long)bytes;
    gMem.totalAllocated += (long long)bytes;
    gMem.allocCount++;
}

void trackFree(size_t bytes) {
    gMem.bytesAllocated -= (long long)bytes;
    if (gMem.allocCount > 0) gMem.allocCount--;
}

void trackRealloc(size_t oldBytes, size_t newBytes) {
    if (newBytes > oldBytes) {
        gMem.bytesAllocated += (long long)(newBytes - oldBytes);
        gMem.totalAllocated += (long long)(newBytes - oldBytes);
    } else {
        gMem.bytesAllocated -= (long long)(oldBytes - newBytes);
    }
}

void printMemStats(void) {
    printf("\n  Memory Usage\n");
    printf("  Current bytes   : %lld\n", gMem.bytesAllocated);
    printf("  Total allocated : %lld\n", gMem.totalAllocated);
    printf("  Active blocks   : %lld\n\n", gMem.allocCount);
}
