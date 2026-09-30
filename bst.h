#ifndef BST_H
#define BST_H

#include "display.h"

typedef struct BSTNode {
    int data;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

typedef struct {
    BSTNode *root;
    int size;
} BST;

BST     *createBST(void);
void     destroyBST(BST **t);
void     clearBST(BST *t);
int      bstInsert(BST *t, int value, int stepByStep);
int      bstDelete(BST *t, int value, int stepByStep);
BSTNode *bstSearch(BST *t, int value);
void bstInorder   (BST *t);
void bstPreorder  (BST *t);
void bstPostorder (BST *t);
void bstLevelOrder(BST *t);
void bstInorderIterative   (BST *t, int stepByStep);
void bstPreorderIterative  (BST *t, int stepByStep);
void bstLevelOrderStepByStep(BST *t);
int  bstHeight   (const BST *t);
int  bstNodeCount(const BST *t);
int  bstIsValid  (const BST *t);
void bstStats    (const BST *t);
void printBST(BST *t);
void fillVirtualArr(BSTNode *node, int *arr, int idx);
BSTNode *bstMinNode(BSTNode *node);

#endif
