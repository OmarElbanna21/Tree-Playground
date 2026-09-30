#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "bst.h"
#include "display.h"
#include "utils.h"

static BSTNode *newNode(int value) {
    BSTNode *node = (BSTNode *)malloc(sizeof(BSTNode));

    if (!node) {
        fprintf(stderr, "Error: malloc failed for BSTNode.\n");
        return NULL;
    }

    trackAlloc(sizeof(BSTNode));
    node->data = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

static void destroyNodes(BSTNode *node) {
    if (!node) {
        return;
    }

    destroyNodes(node->left);
    destroyNodes(node->right);
    trackFree(sizeof(BSTNode));
    free(node);
}

static BSTNode *insertNode(BSTNode *node, int value, int stepByStep, int *inserted) {
    if (!node) {
        *inserted = 1;
        return newNode(value);
    }

    countComparison();
    if (value < node->data) {
        if (stepByStep) {
            printf("  %d < %d -> left\n", value, node->data);
        }
        node->left = insertNode(node->left, value, stepByStep, inserted);
    } else if (value > node->data) {
        if (stepByStep) {
            printf("  %d > %d -> right\n", value, node->data);
        }
        node->right = insertNode(node->right, value, stepByStep, inserted);
    } else if (stepByStep) {
        printf("  %d already exists.\n", value);
    }

    return node;
}

static BSTNode *deleteNode(BSTNode *node, int value, int stepByStep, int *deleted) {
    if (!node) {
        return NULL;
    }

    countComparison();
    if (value < node->data) {
        if (stepByStep) {
            printf("  %d < %d -> left\n", value, node->data);
        }
        node->left = deleteNode(node->left, value, stepByStep, deleted);
        return node;
    }
    if (value > node->data) {
        if (stepByStep) {
            printf("  %d > %d -> right\n", value, node->data);
        }
        node->right = deleteNode(node->right, value, stepByStep, deleted);
        return node;
    }

    *deleted = 1;
    if (!node->left) {
        BSTNode *right = node->right;
        if (stepByStep) {
            printf("  Removing %d.\n", node->data);
        }
        trackFree(sizeof(BSTNode));
        free(node);
        return right;
    }
    if (!node->right) {
        BSTNode *left = node->left;
        if (stepByStep) {
            printf("  Removing %d.\n", node->data);
        }
        trackFree(sizeof(BSTNode));
        free(node);
        return left;
    }

    {
        BSTNode *succ = bstMinNode(node->right);
        if (stepByStep) {
            printf("  Replace %d with successor %d.\n", node->data, succ->data);
        }
        node->data = succ->data;
        node->right = deleteNode(node->right, succ->data, stepByStep, deleted);
    }
    return node;
}

static void inorderRec(BSTNode *node) {
    if (!node) {
        return;
    }

    inorderRec(node->left);
    printf("%d ", node->data);
    inorderRec(node->right);
}

static void preorderRec(BSTNode *node) {
    if (!node) {
        return;
    }

    printf("%d ", node->data);
    preorderRec(node->left);
    preorderRec(node->right);
}

static void postorderRec(BSTNode *node) {
    if (!node) {
        return;
    }

    postorderRec(node->left);
    postorderRec(node->right);
    printf("%d ", node->data);
}

static int heightRec(BSTNode *node) {
    int left;
    int right;

    if (!node) {
        return 0;
    }

    left = heightRec(node->left);
    right = heightRec(node->right);
    return 1 + (left > right ? left : right);
}

static int countRec(BSTNode *node) {
    if (!node) {
        return 0;
    }
    return 1 + countRec(node->left) + countRec(node->right);
}

static int isValidRec(BSTNode *node, long long minValue, long long maxValue) {
    if (!node) {
        return 1;
    }
    if (node->data <= minValue || node->data >= maxValue) {
        return 0;
    }
    return isValidRec(node->left, minValue, node->data)
        && isValidRec(node->right, node->data, maxValue);
}

static int optimalHeight(int nodes) {
    int height = 0;
    int levelNodes = 1;
    int covered = 0;

    while (covered < nodes) {
        height++;
        covered += levelNodes;
        levelNodes <<= 1;
    }
    return height;
}

BST *createBST(void) {
    BST *tree = (BST *)malloc(sizeof(BST));

    if (!tree) {
        fprintf(stderr, "Error: malloc failed for BST.\n");
        return NULL;
    }

    trackAlloc(sizeof(BST));
    tree->root = NULL;
    tree->size = 0;
    return tree;
}

void destroyBST(BST **t) {
    if (!t || !*t) {
        return;
    }

    destroyNodes((*t)->root);
    trackFree(sizeof(BST));
    free(*t);
    *t = NULL;
}

void clearBST(BST *t) {
    if (!t) {
        return;
    }

    destroyNodes(t->root);
    t->root = NULL;
    t->size = 0;
    printf("  BST cleared.\n");
}

int bstInsert(BST *t, int value, int stepByStep) {
    int inserted = 0;

    if (!t) {
        return 0;
    }

    resetTracker("BST Insert");
    if (stepByStep) {
        printf("\n  Inserting %d\n", value);
    }

    t->root = insertNode(t->root, value, stepByStep, &inserted);
    if (inserted) {
        t->size++;
        printf("  Inserted %d. Size = %d.\n", value, t->size);
        printBST(t);
    }

    printStats();
    return inserted;
}

BSTNode *bstSearch(BST *t, int value) {
    BSTNode *cur;

    if (!t) {
        return NULL;
    }

    resetTracker("BST Search");
    cur = t->root;

    while (cur) {
        countComparison();
        if (value == cur->data) {
            printf("  Found %d.\n", value);
            printStats();
            return cur;
        }
        if (value < cur->data) {
            printf("  %d < %d -> left\n", value, cur->data);
            cur = cur->left;
        } else {
            printf("  %d > %d -> right\n", value, cur->data);
            cur = cur->right;
        }
    }

    printf("  %d not found.\n", value);
    printStats();
    return NULL;
}

BSTNode *bstMinNode(BSTNode *node) {
    while (node && node->left) {
        node = node->left;
    }
    return node;
}

int bstDelete(BST *t, int value, int stepByStep) {
    int deleted = 0;

    if (!t) {
        return 0;
    }

    resetTracker("BST Delete");
    if (stepByStep) {
        printf("\n  Deleting %d\n", value);
    }

    t->root = deleteNode(t->root, value, stepByStep, &deleted);
    if (deleted) {
        t->size--;
        printf("  Deleted %d. Size = %d.\n", value, t->size);
        printBST(t);
    } else {
        printf("  %d not found.\n", value);
    }

    printStats();
    return deleted;
}

void bstInorder(BST *t) {
    printf("  Inorder   : ");
    inorderRec(t ? t->root : NULL);
    printf("\n");
}

void bstPreorder(BST *t) {
    printf("  Preorder  : ");
    preorderRec(t ? t->root : NULL);
    printf("\n");
}

void bstPostorder(BST *t) {
    printf("  Postorder : ");
    postorderRec(t ? t->root : NULL);
    printf("\n");
}

void bstLevelOrder(BST *t) {
    BSTNode **queue;
    int head = 0;
    int tail = 0;

    if (!t || !t->root) {
        printf("  [Empty]\n");
        return;
    }

    queue = (BSTNode **)malloc(sizeof(BSTNode *) * (size_t)t->size);
    if (!queue) {
        fprintf(stderr, "Error: malloc failed for BFS queue.\n");
        return;
    }

    queue[tail++] = t->root;
    printf("  Level-Order: ");
    while (head < tail) {
        BSTNode *cur = queue[head++];
        printf("%d ", cur->data);
        if (cur->left) {
            queue[tail++] = cur->left;
        }
        if (cur->right) {
            queue[tail++] = cur->right;
        }
    }
    printf("\n");
    free(queue);
}

void bstInorderIterative(BST *t, int stepByStep) {
    BSTNode **stack;
    BSTNode *cur;
    int top = -1;

    if (!t || !t->root) {
        printf("  [Empty]\n");
        return;
    }

    stack = (BSTNode **)malloc(sizeof(BSTNode *) * (size_t)(t->size + 1));
    if (!stack) {
        fprintf(stderr, "Error: malloc failed for stack.\n");
        return;
    }

    printf("\n  Iterative Inorder\n");
    printf("  Result: ");

    cur = t->root;
    while (cur || top >= 0) {
        while (cur) {
            stack[++top] = cur;
            if (stepByStep) {
                printf("\n  Push %d", cur->data);
            }
            cur = cur->left;
        }

        cur = stack[top--];
        printf("%d ", cur->data);
        if (stepByStep) {
            printf("<- visit");
        }
        cur = cur->right;
    }

    printf("\n");
    free(stack);
}

void bstPreorderIterative(BST *t, int stepByStep) {
    BSTNode **stack;
    int top = -1;

    if (!t || !t->root) {
        printf("  [Empty]\n");
        return;
    }

    stack = (BSTNode **)malloc(sizeof(BSTNode *) * (size_t)(t->size + 1));
    if (!stack) {
        fprintf(stderr, "Error: malloc failed for stack.\n");
        return;
    }

    printf("\n  Iterative Preorder\n");
    printf("  Result: ");

    stack[++top] = t->root;
    while (top >= 0) {
        BSTNode *cur = stack[top--];
        printf("%d ", cur->data);

        if (cur->right) {
            stack[++top] = cur->right;
            if (stepByStep) {
                printf("[push %d] ", cur->right->data);
            }
        }
        if (cur->left) {
            stack[++top] = cur->left;
            if (stepByStep) {
                printf("[push %d] ", cur->left->data);
            }
        }
    }

    printf("\n");
    free(stack);
}

void bstLevelOrderStepByStep(BST *t) {
    BSTNode **queue;
    int head = 0;
    int tail = 0;
    int level = 0;

    if (!t || !t->root) {
        printf("  [Empty]\n");
        return;
    }

    queue = (BSTNode **)malloc(sizeof(BSTNode *) * (size_t)t->size);
    if (!queue) {
        fprintf(stderr, "Error: malloc failed for queue.\n");
        return;
    }

    queue[tail++] = t->root;
    printf("\n  Level-Order Step-by-Step\n");

    while (head < tail) {
        int levelSize = tail - head;
        printf("  Level %d: ", level++);

        for (int i = 0; i < levelSize; i++) {
            BSTNode *cur = queue[head++];
            printf("%d ", cur->data);
            if (cur->left) {
                queue[tail++] = cur->left;
            }
            if (cur->right) {
                queue[tail++] = cur->right;
            }
        }

        printf("\n  Queue: ");
        for (int i = head; i < tail; i++) {
            printf("%d ", queue[i]->data);
        }
        printf("\n\n");
    }

    free(queue);
}

int bstHeight(const BST *t) {
    return t ? heightRec(t->root) : 0;
}

int bstNodeCount(const BST *t) {
    return t ? countRec(t->root) : 0;
}

int bstIsValid(const BST *t) {
    return t ? isValidRec(t->root, LLONG_MIN, LLONG_MAX) : 1;
}

void bstStats(const BST *t) {
    int nodes = bstNodeCount(t);
    int height = bstHeight(t);

    printf("\n  BST Stats\n");
    printf("  Nodes  : %d\n", nodes);
    printf("  Height : %d\n", height);
    printf("  Best   : %d\n", optimalHeight(nodes));
    printf("  Valid  : %s\n\n", bstIsValid(t) ? "yes" : "no");
}

void fillVirtualArr(BSTNode *node, int *arr, int idx) {
    if (!node || idx >= VIRT_ARR_SIZE) {
        return;
    }

    arr[idx] = node->data;
    fillVirtualArr(node->left, arr, 2 * idx + 1);
    fillVirtualArr(node->right, arr, 2 * idx + 2);
}

void printBST(BST *t) {
    int arr[VIRT_ARR_SIZE];
    int height;

    if (!t || !t->root) {
        printf("  [Empty BST]\n");
        return;
    }

    height = bstHeight(t);
    if (height > MAX_DISP_HEIGHT) {
        printf("  [Showing first %d levels]\n", MAX_DISP_HEIGHT);
        height = MAX_DISP_HEIGHT;
    }

    for (int i = 0; i < VIRT_ARR_SIZE; i++) {
        arr[i] = EMPTY_SLOT;
    }

    fillVirtualArr(t->root, arr, 0);
    printVirtualTree(arr, height, NULL, 0);
}
