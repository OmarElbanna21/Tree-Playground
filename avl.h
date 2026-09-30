#ifndef AVL_H
#define AVL_H

#include "display.h"

typedef struct AVLNode {
    int data;
    struct AVLNode *left;
    struct AVLNode *right;
    int height;
} AVLNode;

typedef struct {
    AVLNode *root;
    int size;
} AVL;

AVL     *createAVL(void);
void     destroyAVL(AVL **t);
void     clearAVL(AVL *t);
void     avlInsert(AVL *t, int value, int stepByStep);
int      avlDelete(AVL *t, int value, int stepByStep);
AVLNode *avlSearch(AVL *t, int value);
void avlInorder   (AVL *t);
void avlPreorder  (AVL *t);
void avlPostorder (AVL *t);
void avlLevelOrder(AVL *t);
void avlLevelOrderStepByStep(AVL *t);
int  avlHeight   (const AVL *t);
int  avlNodeCount(const AVL *t);
void avlStats    (const AVL *t);
void printAVL(AVL *t);

#endif
