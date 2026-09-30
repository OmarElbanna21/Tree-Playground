#ifndef HEAP_H
#define HEAP_H

#define DEFAULT_CAPACITY 127

typedef struct {
    int *data;
    int size;
    int capacity;
    int isMaxHeap;
} Heap;

Heap *createHeap(int capacity, int isMaxHeap);
void  destroyHeap(Heap **h);
void  insert(Heap *h, int value, int stepByStep);
int   extractRoot(Heap *h, int stepByStep);
int   peekRoot(const Heap *h);
int   search(const Heap *h, int value);
void  deleteAt(Heap *h, int index, int stepByStep);
void  deleteValue(Heap *h, int value, int stepByStep);
void  clearHeap(Heap *h);
Heap *buildFromArray(int *arr, int n, int isMaxHeap, int stepByStep);
void  heapSort(int *arr, int n, int stepByStep);
void heapifyUp(Heap *h, int i, int stepByStep);
void heapifyDown(Heap *h, int i, int stepByStep);
int  hasHigherPriority(const Heap *h, int a, int b);

#endif
