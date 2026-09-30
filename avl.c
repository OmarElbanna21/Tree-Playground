#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "avl.h"
#include "display.h"
#include "utils.h"

static int nodeHeight(AVLNode *node) {
    return node ? node->height : 0;
}

static int max2(int a, int b) {
    return a > b ? a : b;
}

static void updateHeight(AVLNode *node) {
    if (node) {
        node->height = 1 + max2(nodeHeight(node->left), nodeHeight(node->right));
    }
}

static int balanceFactor(AVLNode *node) {
    return node ? nodeHeight(node->left) - nodeHeight(node->right) : 0;
}

static AVLNode *newAVLNode(int value) {
    AVLNode *node = (AVLNode *)malloc(sizeof(AVLNode));

    if (!node) {
        fprintf(stderr, "Error: malloc failed for AVLNode.\n");
        return NULL;
    }

    trackAlloc(sizeof(AVLNode));
    node->data = value;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

static AVLNode *rotateRight(AVLNode *root, int stepByStep) {
    AVLNode *left = root->left;
    AVLNode *middle = left->right;

    if (stepByStep) {
        printf("  Right rotation around %d.\n", root->data);
    }

    left->right = root;
    root->left = middle;

    updateHeight(root);
    updateHeight(left);
    return left;
}

static AVLNode *rotateLeft(AVLNode *root, int stepByStep) {
    AVLNode *right = root->right;
    AVLNode *middle = right->left;

    if (stepByStep) {
        printf("  Left rotation around %d.\n", root->data);
    }

    right->left = root;
    root->right = middle;

    updateHeight(root);
    updateHeight(right);
    return right;
}

static AVLNode *rebalance(AVLNode *node, int stepByStep) {
    int bf;

    updateHeight(node);
    bf = balanceFactor(node);

    if (bf > 1 && balanceFactor(node->left) >= 0) {
        return rotateRight(node, stepByStep);
    }
    if (bf > 1 && balanceFactor(node->left) < 0) {
        if (stepByStep) {
            printf("  Left-right case at %d.\n", node->data);
        }
        node->left = rotateLeft(node->left, stepByStep);
        return rotateRight(node, stepByStep);
    }
    if (bf < -1 && balanceFactor(node->right) <= 0) {
        return rotateLeft(node, stepByStep);
    }
    if (bf < -1 && balanceFactor(node->right) > 0) {
        if (stepByStep) {
            printf("  Right-left case at %d.\n", node->data);
        }
        node->right = rotateRight(node->right, stepByStep);
        return rotateLeft(node, stepByStep);
    }

    return node;
}

static AVLNode *insertNode(AVLNode *node, int value, int stepByStep, int *inserted) {
    if (!node) {
        *inserted = 1;
        return newAVLNode(value);
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
    } else {
        if (stepByStep) {
            printf("  %d already exists.\n", value);
        }
        return node;
    }

    return rebalance(node, stepByStep);
}

static AVLNode *minNode(AVLNode *node) {
    while (node && node->left) {
        node = node->left;
    }
    return node;
}

static AVLNode *deleteNode(AVLNode *node, int value, int stepByStep, int *deleted) {
    if (!node) {
        return NULL;
    }

    countComparison();
    if (value < node->data) {
        if (stepByStep) {
            printf("  %d < %d -> left\n", value, node->data);
        }
        node->left = deleteNode(node->left, value, stepByStep, deleted);
    } else if (value > node->data) {
        if (stepByStep) {
            printf("  %d > %d -> right\n", value, node->data);
        }
        node->right = deleteNode(node->right, value, stepByStep, deleted);
    } else {
        *deleted = 1;
        if (!node->left || !node->right) {
            AVLNode *child = node->left ? node->left : node->right;
            trackFree(sizeof(AVLNode));
            free(node);
            return child;
        }

        {
            AVLNode *succ = minNode(node->right);
            if (stepByStep) {
                printf("  Replace %d with successor %d.\n", node->data, succ->data);
            }
            node->data = succ->data;
            node->right = deleteNode(node->right, succ->data, stepByStep, deleted);
        }
    }

    return rebalance(node, stepByStep);
}

static void destroyNodes(AVLNode *node) {
    if (!node) {
        return;
    }

    destroyNodes(node->left);
    destroyNodes(node->right);
    trackFree(sizeof(AVLNode));
    free(node);
}

static void inorderRec(AVLNode *node) {
    if (!node) {
        return;
    }
    inorderRec(node->left);
    printf("%d ", node->data);
    inorderRec(node->right);
}

static void preorderRec(AVLNode *node) {
    if (!node) {
        return;
    }
    printf("%d ", node->data);
    preorderRec(node->left);
    preorderRec(node->right);
}

static void postorderRec(AVLNode *node) {
    if (!node) {
        return;
    }
    postorderRec(node->left);
    postorderRec(node->right);
    printf("%d ", node->data);
}

static int countRec(AVLNode *node) {
    if (!node) {
        return 0;
    }
    return 1 + countRec(node->left) + countRec(node->right);
}

static void fillVirtual(AVLNode *node, int *arr, int *bfArr, int idx) {
    if (!node || idx >= VIRT_ARR_SIZE) {
        return;
    }

    arr[idx] = node->data;
    bfArr[idx] = balanceFactor(node);
    fillVirtual(node->left, arr, bfArr, 2 * idx + 1);
    fillVirtual(node->right, arr, bfArr, 2 * idx + 2);
}

AVL *createAVL(void) {
    AVL *tree = (AVL *)malloc(sizeof(AVL));

    if (!tree) {
        return NULL;
    }

    trackAlloc(sizeof(AVL));
    tree->root = NULL;
    tree->size = 0;
    return tree;
}

void destroyAVL(AVL **t) {
    if (!t || !*t) {
        return;
    }

    destroyNodes((*t)->root);
    trackFree(sizeof(AVL));
    free(*t);
    *t = NULL;
}

void clearAVL(AVL *t) {
    if (!t) {
        return;
    }

    destroyNodes(t->root);
    t->root = NULL;
    t->size = 0;
    printf("  AVL cleared.\n");
}

void avlInsert(AVL *t, int value, int stepByStep) {
    int inserted = 0;

    if (!t) {
        return;
    }

    resetTracker("AVL Insert");
    if (stepByStep) {
        printf("\n  Inserting %d\n", value);
    }

    t->root = insertNode(t->root, value, stepByStep, &inserted);
    if (inserted) {
        t->size++;
        printf("  Inserted %d. Size = %d.\n", value, t->size);
        printAVL(t);
    }

    printStats();
}

int avlDelete(AVL *t, int value, int stepByStep) {
    int deleted = 0;

    if (!t) {
        return 0;
    }

    resetTracker("AVL Delete");
    if (stepByStep) {
        printf("\n  Deleting %d\n", value);
    }

    t->root = deleteNode(t->root, value, stepByStep, &deleted);
    if (deleted) {
        t->size--;
        printf("  Deleted %d. Size = %d.\n", value, t->size);
        printAVL(t);
    } else {
        printf("  %d not found.\n", value);
    }

    printStats();
    return deleted;
}

AVLNode *avlSearch(AVL *t, int value) {
    AVLNode *cur;

    if (!t) {
        return NULL;
    }

    resetTracker("AVL Search");
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

void avlInorder(AVL *t) {
    printf("  Inorder   : ");
    inorderRec(t ? t->root : NULL);
    printf("\n");
}

void avlPreorder(AVL *t) {
    printf("  Preorder  : ");
    preorderRec(t ? t->root : NULL);
    printf("\n");
}

void avlPostorder(AVL *t) {
    printf("  Postorder : ");
    postorderRec(t ? t->root : NULL);
    printf("\n");
}

void avlLevelOrder(AVL *t) {
    AVLNode **queue;
    int head = 0;
    int tail = 0;

    if (!t || !t->root) {
        printf("  [Empty]\n");
        return;
    }

    queue = (AVLNode **)malloc(sizeof(AVLNode *) * (size_t)t->size);
    if (!queue) {
        fprintf(stderr, "Error: malloc failed for queue.\n");
        return;
    }

    queue[tail++] = t->root;
    printf("  Level-Order: ");
    while (head < tail) {
        AVLNode *cur = queue[head++];
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

void avlLevelOrderStepByStep(AVL *t) {
    AVLNode **queue;
    int head = 0;
    int tail = 0;
    int level = 0;

    if (!t || !t->root) {
        printf("  [Empty]\n");
        return;
    }

    queue = (AVLNode **)malloc(sizeof(AVLNode *) * (size_t)t->size);
    if (!queue) {
        fprintf(stderr, "Error: malloc failed for queue.\n");
        return;
    }

    queue[tail++] = t->root;
    printf("\n  Level-Order Step-by-Step\n");

    while (head < tail) {
        int count = tail - head;
        printf("  Level %d: ", level++);

        for (int i = 0; i < count; i++) {
            AVLNode *cur = queue[head++];
            printf("%d(BF%+d) ", cur->data, balanceFactor(cur));
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

int avlHeight(const AVL *t) {
    return t ? nodeHeight(t->root) : 0;
}

int avlNodeCount(const AVL *t) {
    return t ? countRec(t->root) : 0;
}

void avlStats(const AVL *t) {
    printf("\n  AVL Stats\n");
    printf("  Nodes   : %d\n", avlNodeCount(t));
    printf("  Height  : %d\n", avlHeight(t));
    printf("  Root BF : %d\n\n", (t && t->root) ? balanceFactor(t->root) : 0);
}

void printAVL(AVL *t) {
    int arr[VIRT_ARR_SIZE];
    int bfArr[VIRT_ARR_SIZE];
    int height;

    if (!t || !t->root) {
        printf("  [Empty AVL]\n");
        return;
    }

    height = avlHeight(t);
    if (height > MAX_DISP_HEIGHT) {
        printf("  [Showing first %d levels]\n", MAX_DISP_HEIGHT);
        height = MAX_DISP_HEIGHT;
    }

    for (int i = 0; i < VIRT_ARR_SIZE; i++) {
        arr[i] = EMPTY_SLOT;
        bfArr[i] = EMPTY_SLOT;
    }

    fillVirtual(t->root, arr, bfArr, 0);
    printf("  values with balance factors\n");
    printVirtualTree(arr, height, bfArr, 1);
}
