#ifndef QUEUE_H
#define QUEUE_H

typedef struct QNode {
    int data;
    struct QNode *next;
} QNode;

typedef struct {
    QNode *front;
    QNode *rear;
    int    size;
} Queue;

Queue *createQueue(void);
void   destroyQueue(Queue **q);
int    enqueue(Queue *q, int val);
int    dequeue(Queue *q);
int    queuePeek(const Queue *q);
int    queueIsEmpty(const Queue *q);
void   printQueue(const Queue *q);

#endif
