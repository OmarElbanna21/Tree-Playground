#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include "utils.h"

Stack *createStack(int capacity) {
    if (capacity <= 0) capacity = STACK_INIT_CAP;
    Stack *s = (Stack *)malloc(sizeof(Stack));
    if (!s) { fprintf(stderr, "Error: malloc failed for Stack.\n"); return NULL; }
    trackAlloc(sizeof(Stack));
    s->data = (int *)malloc(sizeof(int) * capacity);
    if (!s->data) { trackFree(sizeof(Stack)); free(s); return NULL; }
    trackAlloc(sizeof(int) * capacity);
    s->top      = -1;
    s->capacity = capacity;
    return s;
}

void destroyStack(Stack **s) {
    if (!s || !*s) return;
    trackFree(sizeof(int) * (size_t)(*s)->capacity);
    free((*s)->data);
    trackFree(sizeof(Stack));
    free(*s);
    *s = NULL;
}

void push(Stack *s, int val) {
    if (!s) return;
    /* Resize if full */
    if (s->top + 1 == s->capacity) {
        int oldCapacity = s->capacity;
        int *newData;
        s->capacity *= 2;
        newData = (int *)realloc(s->data, sizeof(int) * s->capacity);
        if (!newData) { fprintf(stderr, "Error: realloc failed.\n"); exit(1); }
        s->data = newData;
        trackRealloc(sizeof(int) * (size_t)oldCapacity, sizeof(int) * (size_t)s->capacity);
    }
    s->data[++s->top] = val;
}

int pop(Stack *s) {
    if (!s || s->top == -1) {
        fprintf(stderr, "Error: pop from empty stack.\n");
        return -1;
    }
    return s->data[s->top--];
}

int stackPeek(const Stack *s) {
    if (!s || s->top == -1) return -1;
    return s->data[s->top];
}

int stackIsEmpty(const Stack *s) {
    return (!s || s->top == -1);
}

void printStack(const Stack *s) {
    if (stackIsEmpty(s)) { printf("  [Empty Stack]\n"); return; }
    printf("  Top -> ");
    for (int i = s->top; i >= 0; i--) printf("%d ", s->data[i]);
    printf("<- Bottom\n");
}
