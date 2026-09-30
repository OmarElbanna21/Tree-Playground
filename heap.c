#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heap.h"
#include "display.h"
#include "utils.h"

static void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
    countSwap();
}

int hasHigherPriority(const Heap *h, int a, int b) {
    return h->isMaxHeap ? (a > b) : (a < b);
}

Heap *createHeap(int capacity, int isMaxHeap) {
    Heap *h;

    if (capacity <= 0) {
        capacity = DEFAULT_CAPACITY;
    }

    h = (Heap *)malloc(sizeof(Heap));
    if (!h) {
        fprintf(stderr, "Error: malloc failed for Heap.\n");
        return NULL;
    }
    trackAlloc(sizeof(Heap));

    h->data = (int *)malloc(sizeof(int) * capacity);
    if (!h->data) {
        trackFree(sizeof(Heap));
        free(h);
        fprintf(stderr, "Error: malloc failed for data.\n");
        return NULL;
    }
    trackAlloc(sizeof(int) * capacity);

    h->size = 0;
    h->capacity = capacity;
    h->isMaxHeap = isMaxHeap;
    return h;
}

void destroyHeap(Heap **h) {
    if (!h || !*h) {
        return;
    }

    trackFree(sizeof(int) * (size_t)(*h)->capacity);
    free((*h)->data);
    trackFree(sizeof(Heap));
    free(*h);
    *h = NULL;
}

void clearHeap(Heap *h) {
    if (!h) {
        return;
    }

    h->size = 0;
    printf("  Heap cleared.\n");
}

int peekRoot(const Heap *h) {
    if (!h || h->size == 0) {
        printf("  Heap is empty.\n");
        return -1;
    }
    return h->data[0];
}

int search(const Heap *h, int value) {
    int i;

    if (!h) {
        return -1;
    }

    for (i = 0; i < h->size; i++) {
        if (h->data[i] == value) {
            return i;
        }
    }
    return -1;
}

void heapifyUp(Heap *h, int i, int stepByStep) {
    while (i > 0) {
        int parent = (i - 1) / 2;

        if (!hasHigherPriority(h, h->data[i], h->data[parent])) {
            break;
        }

        if (stepByStep) {
            printf("  Swap index %d (%d) with parent %d (%d)\n",
                   i, h->data[i], parent, h->data[parent]);
        }

        swap(&h->data[i], &h->data[parent]);
        i = parent;

        if (stepByStep) {
            printHeapTree(h->data, h->size);
        }
    }
}

void heapifyDown(Heap *h, int i, int stepByStep) {
    while (1) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int best = i;

        if (left < h->size && hasHigherPriority(h, h->data[left], h->data[best])) {
            best = left;
        }
        if (right < h->size && hasHigherPriority(h, h->data[right], h->data[best])) {
            best = right;
        }
        if (best == i) {
            break;
        }

        if (stepByStep) {
            printf("  Swap index %d (%d) with index %d (%d)\n",
                   i, h->data[i], best, h->data[best]);
        }

        swap(&h->data[i], &h->data[best]);
        i = best;

        if (stepByStep) {
            printHeapTree(h->data, h->size);
        }
    }
}

void insert(Heap *h, int value, int stepByStep) {
    if (!h) {
        return;
    }

    if (h->size == h->capacity) {
        int newCapacity = h->capacity * 2;
        int *newData = (int *)realloc(h->data, sizeof(int) * newCapacity);

        if (!newData) {
            fprintf(stderr, "Error: realloc failed.\n");
            return;
        }

        trackRealloc(sizeof(int) * (size_t)h->capacity, sizeof(int) * (size_t)newCapacity);
        h->data = newData;
        h->capacity = newCapacity;
        printf("  Heap resized to %d.\n", h->capacity);
    }

    h->data[h->size] = value;
    h->size++;

    printf("  Inserted %d at index %d.\n", value, h->size - 1);
    if (stepByStep) {
        printHeapTree(h->data, h->size);
    }

    heapifyUp(h, h->size - 1, stepByStep);

    if (!stepByStep) {
        printf("  After insert %d:\n", value);
        printHeapTree(h->data, h->size);
    }
}

int extractRoot(Heap *h, int stepByStep) {
    int root;

    if (!h || h->size == 0) {
        fprintf(stderr, "  Error: Cannot extract from empty heap.\n");
        return -1;
    }

    root = h->data[0];
    printf("  Extracting root: %d\n", root);

    h->data[0] = h->data[h->size - 1];
    h->size--;

    if (stepByStep && h->size > 0) {
        printf("  Moved last element (%d) to root.\n", h->data[0]);
        printHeapTree(h->data, h->size);
    }

    if (h->size > 0) {
        heapifyDown(h, 0, stepByStep);
    }

    if (!stepByStep) {
        printf("  After extraction:\n");
        printHeapTree(h->data, h->size);
    }

    return root;
}

void deleteAt(Heap *h, int index, int stepByStep) {
    if (!h || index < 0 || index >= h->size) {
        fprintf(stderr, "  Error: Invalid index %d.\n", index);
        return;
    }

    printf("  Deleting element at index %d (value = %d).\n", index, h->data[index]);
    h->data[index] = h->data[h->size - 1];
    h->size--;

    if (h->size == 0 || index == h->size) {
        printf("  After deletion:\n");
        printHeapTree(h->data, h->size);
        return;
    }

    heapifyUp(h, index, stepByStep);
    if (index < h->size) {
        heapifyDown(h, index, stepByStep);
    }

    if (!stepByStep) {
        printf("  After deletion:\n");
        printHeapTree(h->data, h->size);
    }
}

void deleteValue(Heap *h, int value, int stepByStep) {
    int idx = search(h, value);

    if (idx == -1) {
        printf("  Value %d not found in heap.\n", value);
        return;
    }

    deleteAt(h, idx, stepByStep);
}

Heap *buildFromArray(int *arr, int n, int isMaxHeap, int stepByStep) {
    Heap *h;
    int i;

    if (!arr || n <= 0) {
        fprintf(stderr, "  Error: Invalid array.\n");
        return NULL;
    }

    h = createHeap(n, isMaxHeap);
    if (!h) {
        return NULL;
    }

    memcpy(h->data, arr, sizeof(int) * n);
    h->size = n;

    printf("  Initial array loaded:\n");
    printHeapTree(h->data, h->size);

    for (i = n / 2 - 1; i >= 0; i--) {
        if (stepByStep) {
            printf("  Heapifying index %d (%d)\n", i, h->data[i]);
        }
        heapifyDown(h, i, stepByStep);
    }

    if (!stepByStep) {
        printf("  Heap built:\n");
        printHeapTree(h->data, h->size);
    }

    return h;
}

void heapSort(int *arr, int n, int stepByStep) {
    Heap temp;
    int i;
    int end;

    if (!arr || n <= 1) {
        return;
    }

    printf("\n  === Heap Sort ===\n");

    temp.data = arr;
    temp.size = n;
    temp.capacity = n;
    temp.isMaxHeap = 1;

    for (i = n / 2 - 1; i >= 0; i--) {
        heapifyDown(&temp, i, 0);
    }

    printf("  Max Heap built:\n");
    printHeapTree(arr, n);

    for (end = n - 1; end > 0; end--) {
        if (stepByStep) {
            printf("  Extract max (%d), place at index %d.\n", arr[0], end);
        }

        swap(&arr[0], &arr[end]);
        temp.size--;
        heapifyDown(&temp, 0, 0);

        if (stepByStep) {
            printf("  Heap remaining:\n");
            printHeapTree(arr, temp.size);
            printf("  Sorted so far: ");
            for (i = end; i < n; i++) {
                printf("%d ", arr[i]);
            }
            printf("\n\n");
        }
    }

    printf("  Sorted array: ");
    for (i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n\n");
}
