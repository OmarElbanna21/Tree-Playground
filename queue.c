#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "utils.h"

Queue *createQueue(void) {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    if (!q) { fprintf(stderr, "Error: malloc failed for Queue.\n"); return NULL; }
    trackAlloc(sizeof(Queue));
    q->front = q->rear = NULL;
    q->size  = 0;
    return q;
}

void destroyQueue(Queue **q) {
    if (!q || !*q) return;
    while (!queueIsEmpty(*q)) dequeue(*q);
    trackFree(sizeof(Queue));
    free(*q);
    *q = NULL;
}

int enqueue(Queue *q, int val) {
    if (!q) return 0;
    QNode *node = (QNode *)malloc(sizeof(QNode));
    if (!node) { fprintf(stderr, "Error: malloc failed for QNode.\n"); return 0; }
    trackAlloc(sizeof(QNode));
    node->data = val;
    node->next = NULL;

    if (q->rear) q->rear->next = node;
    else         q->front      = node;   /* first element */
    q->rear = node;
    q->size++;
    return 1;
}

int dequeue(Queue *q) {
    if (!q || !q->front) {
        fprintf(stderr, "Error: dequeue from empty queue.\n");
        return -1;
    }
    QNode *tmp  = q->front;
    int    val  = tmp->data;
    q->front    = tmp->next;
    if (!q->front) q->rear = NULL;   /* queue became empty */
    trackFree(sizeof(QNode));
    free(tmp);
    q->size--;
    return val;
}

int queuePeek(const Queue *q) {
    if (!q || !q->front) return -1;
    return q->front->data;
}

int queueIsEmpty(const Queue *q) {
    return (!q || q->size == 0);
}

void printQueue(const Queue *q) {
    if (queueIsEmpty(q)) { printf("  [Empty Queue]\n"); return; }
    printf("  Front -> ");
    for (QNode *cur = q->front; cur; cur = cur->next)
        printf("%d ", cur->data);
    printf("<- Rear\n");
}
