#ifndef STACK_H
#define STACK_H

#define STACK_INIT_CAP 64

typedef struct {
    int *data;
    int top;
    int capacity;
} Stack;

Stack *createStack(int capacity);
void   destroyStack(Stack **s);
void   push(Stack *s, int val);
int    pop(Stack *s);
int    stackPeek(const Stack *s);
int    stackIsEmpty(const Stack *s);
void   printStack(const Stack *s);

#endif
