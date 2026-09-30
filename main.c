#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"
#include "bst.h"
#include "display.h"
#include "hashtable.h"
#include "heap.h"
#include "utils.h"

static void clearInput(void) {
    int c;

    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

static void sep(void) {
    printf("\n========================================\n");
}

static void sep2(void) {
    printf("----------------------------------------\n");
}

static int getStepChoice(void) {
    int step = 0;

    printf("  Step-by-step? (1 = yes, 0 = no): ");
    scanf("%d", &step);
    clearInput();
    return step == 1;
}

static int readInt(const char *prompt) {
    int value = 0;

    printf("  %s", prompt);
    scanf("%d", &value);
    clearInput();
    return value;
}

static void heapMenu(int isMaxHeap) {
    Heap *heap = createHeap(DEFAULT_CAPACITY, isMaxHeap);
    int choice;

    printf("\n  %s heap ready.\n", isMaxHeap ? "Max" : "Min");
    do {
        sep();
        printf("  %s HEAP\n", isMaxHeap ? "MAX" : "MIN");
        sep2();
        printf("  1.Insert  2.Extract  3.Peek  4.Search  5.Delete\n");
        printf("  6.Build from array  7.Heap Sort  8.Display\n");
        printf("  9.Clear  10.Stats  0.Back\n");
        sep2();
        printf("  Choice: ");
        scanf("%d", &choice);
        clearInput();

        switch (choice) {
        case 1: {
            int value = readInt("Value: ");
            insert(heap, value, getStepChoice());
            break;
        }
        case 2: {
            int result = extractRoot(heap, getStepChoice());
            if (result != -1) {
                printf("  Extracted: %d\n", result);
            }
            break;
        }
        case 3:
            printf("  Root: %d\n", peekRoot(heap));
            break;
        case 4: {
            int value = readInt("Search: ");
            int index = search(heap, value);
            if (index == -1) {
                printf("  Not found.\n");
            } else {
                printf("  Found at index %d.\n", index);
            }
            break;
        }
        case 5: {
            int value = readInt("Delete: ");
            deleteValue(heap, value, getStepChoice());
            break;
        }
        case 6: {
            int n = readInt("Size: ");
            int *arr;

            if (n <= 0 || n > 1000) {
                printf("  Invalid size.\n");
                break;
            }

            arr = (int *)malloc(sizeof(int) * n);
            if (!arr) {
                printf("  Allocation failed.\n");
                break;
            }

            printf("  Enter %d ints: ", n);
            for (int i = 0; i < n; i++) {
                scanf("%d", &arr[i]);
            }
            clearInput();

            destroyHeap(&heap);
            heap = buildFromArray(arr, n, isMaxHeap, getStepChoice());
            free(arr);
            break;
        }
        case 7: {
            int *copy;

            if (!heap || heap->size == 0) {
                printf("  Empty.\n");
                break;
            }

            copy = (int *)malloc(sizeof(int) * heap->size);
            if (!copy) {
                printf("  Allocation failed.\n");
                break;
            }

            memcpy(copy, heap->data, sizeof(int) * heap->size);
            heapSort(copy, heap->size, getStepChoice());
            free(copy);
            break;
        }
        case 8:
            printf("  %s heap:\n", isMaxHeap ? "Max" : "Min");
            printHeapTree(heap->data, heap->size);
            break;
        case 9:
            clearHeap(heap);
            break;
        case 10:
            printf("  Nodes: %d  Height: %d\n", heap->size, getHeight(heap->size));
            printMemStats();
            break;
        default:
            break;
        }
    } while (choice != 0);

    destroyHeap(&heap);
}

static void bstMenu(void) {
    BST *tree = createBST();
    int choice;

    printf("\n  BST ready.\n");
    do {
        sep();
        printf("  BST MENU\n");
        sep2();
        printf("  1.Insert  2.Delete  3.Search\n");
        printf("  4.Traversals  5.Iterative traversals\n");
        printf("  6.Level-Order step-by-step  7.Display\n");
        printf("  8.Stats  9.Clear  0.Back\n");
        sep2();
        printf("  Choice: ");
        scanf("%d", &choice);
        clearInput();

        switch (choice) {
        case 1:
            bstInsert(tree, readInt("Value: "), getStepChoice());
            break;
        case 2:
            bstDelete(tree, readInt("Value: "), getStepChoice());
            break;
        case 3:
            bstSearch(tree, readInt("Value: "));
            break;
        case 4:
            bstInorder(tree);
            bstPreorder(tree);
            bstPostorder(tree);
            bstLevelOrder(tree);
            break;
        case 5: {
            int kind;

            printf("  1 = Inorder  2 = Preorder: ");
            scanf("%d", &kind);
            clearInput();

            if (kind == 1) {
                bstInorderIterative(tree, getStepChoice());
            } else {
                bstPreorderIterative(tree, getStepChoice());
            }
            break;
        }
        case 6:
            bstLevelOrderStepByStep(tree);
            break;
        case 7:
            printBST(tree);
            break;
        case 8:
            bstStats(tree);
            printMemStats();
            break;
        case 9:
            clearBST(tree);
            break;
        default:
            break;
        }
    } while (choice != 0);

    destroyBST(&tree);
}

static void avlMenu(void) {
    AVL *tree = createAVL();
    int choice;

    printf("\n  AVL ready.\n");
    do {
        sep();
        printf("  AVL MENU\n");
        sep2();
        printf("  1.Insert  2.Delete  3.Search\n");
        printf("  4.Traversals  5.Level-Order step-by-step\n");
        printf("  6.Display  7.Stats  8.Clear  0.Back\n");
        sep2();
        printf("  Choice: ");
        scanf("%d", &choice);
        clearInput();

        switch (choice) {
        case 1:
            avlInsert(tree, readInt("Value: "), getStepChoice());
            break;
        case 2:
            avlDelete(tree, readInt("Value: "), getStepChoice());
            break;
        case 3:
            avlSearch(tree, readInt("Value: "));
            break;
        case 4:
            avlInorder(tree);
            avlPreorder(tree);
            avlPostorder(tree);
            avlLevelOrder(tree);
            break;
        case 5:
            avlLevelOrderStepByStep(tree);
            break;
        case 6:
            printAVL(tree);
            break;
        case 7:
            avlStats(tree);
            printMemStats();
            break;
        case 8:
            clearAVL(tree);
            break;
        default:
            break;
        }
    } while (choice != 0);

    destroyAVL(&tree);
}

static void hashMenu(void) {
    int capacity = readInt("Capacity (for example 11): ");
    HashLP *lp;
    HashChain *chain;
    int choice;

    if (capacity <= 0) {
        capacity = HT_DEFAULT_CAP;
    }

    lp = createHashLP(capacity);
    chain = createHashChain(capacity);
    printf("\n  Hash tables ready (cap = %d).\n", capacity);

    do {
        sep();
        printf("  HASH TABLE MENU\n");
        sep2();
        printf("  [LP]  1.Insert 2.Search 3.Delete 4.Display 5.Stats\n");
        printf("  [CH]  6.Insert 7.Search 8.Delete 9.Display 10.Stats\n");
        printf("  11.Compare same key in both  0.Back\n");
        sep2();
        printf("  Choice: ");
        scanf("%d", &choice);
        clearInput();

        switch (choice) {
        case 1:
            lpInsert(lp, readInt("Key: "), getStepChoice());
            break;
        case 2:
            lpSearch(lp, readInt("Key: "), getStepChoice());
            break;
        case 3:
            lpDelete(lp, readInt("Key: "), getStepChoice());
            break;
        case 4:
            printHashLP(lp);
            break;
        case 5:
            lpStats(lp);
            break;
        case 6:
            chainInsert(chain, readInt("Key: "), getStepChoice());
            break;
        case 7:
            chainSearch(chain, readInt("Key: "), getStepChoice());
            break;
        case 8:
            chainDelete(chain, readInt("Key: "), getStepChoice());
            break;
        case 9:
            printHashChain(chain);
            break;
        case 10:
            chainStats(chain);
            break;
        case 11: {
            int value = readInt("Key: ");
            printf("\n  Linear Probing\n");
            lpInsert(lp, value, 1);
            printf("\n  Separate Chaining\n");
            chainInsert(chain, value, 1);
            break;
        }
        default:
            break;
        }
    } while (choice != 0);

    destroyHashLP(&lp);
    destroyHashChain(&chain);
}

static void compareMode(void) {
    int n = readInt("How many values? ");
    int *arr;
    BST *bst;
    AVL *avl;
    Heap *heap;

    if (n <= 0 || n > 50) {
        printf("  Max 50 values.\n");
        return;
    }

    arr = (int *)malloc(sizeof(int) * n);
    if (!arr) {
        printf("  Allocation failed.\n");
        return;
    }

    printf("  Enter %d integers: ", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }
    clearInput();

    bst = createBST();
    avl = createAVL();
    heap = createHeap(n + 1, 1);

    for (int i = 0; i < n; i++) {
        bstInsert(bst, arr[i], 0);
        avlInsert(avl, arr[i], 0);
        insert(heap, arr[i], 0);
    }

    sep();
    printf("  BST (height = %d)\n", bstHeight(bst));
    printBST(bst);
    printf("  AVL (height = %d)\n", avlHeight(avl));
    printAVL(avl);
    printf("  Max Heap (height = %d)\n", getHeight(heap->size));
    printHeapTree(heap->data, heap->size);

    sep2();
    printf("  %-10s %-8s %-8s\n", "Structure", "Height", "Nodes");
    printf("  %-10s %-8d %-8d\n", "BST", bstHeight(bst), bstNodeCount(bst));
    printf("  %-10s %-8d %-8d\n", "AVL", avlHeight(avl), avlNodeCount(avl));
    printf("  %-10s %-8d %-8d\n", "MaxHeap", getHeight(heap->size), heap->size);

    if (bstHeight(bst) > avlHeight(avl)) {
        printf("\n  AVL saved %d levels compared to BST.\n", bstHeight(bst) - avlHeight(avl));
    } else {
        printf("\n  Both trees have the same height for this input.\n");
    }

    printMemStats();

    free(arr);
    destroyBST(&bst);
    destroyAVL(&avl);
    destroyHeap(&heap);
}

static void worstCaseDemo(void) {
    BST *bst = createBST();
    AVL *avl = createAVL();

    sep();
    printf("  WORST CASE DEMO: sorted input 1..10\n");
    printf("  BST grows like a chain, AVL stays balanced.\n\n");

    for (int i = 1; i <= 10; i++) {
        bstInsert(bst, i, 0);
        avlInsert(avl, i, 0);
    }

    printf("  BST (height = %d)\n", bstHeight(bst));
    printBST(bst);
    printf("  AVL (height = %d)\n", avlHeight(avl));
    printAVL(avl);

    destroyBST(&bst);
    destroyAVL(&avl);
}

int main(void) {
    int choice;

    gMem.bytesAllocated = 0;
    gMem.totalAllocated = 0;
    gMem.allocCount = 0;

    printf("\n");
    printf("  TREE PLAYGROUND\n");
    printf("  BST | AVL | Heap | Hash Table\n");

    do {
        sep();
        printf("  MAIN MENU\n");
        sep2();
        printf("  1.Max Heap   2.Min Heap   3.BST   4.AVL\n");
        printf("  5.Hash Table  6.Compare Mode  7.Worst Case Demo\n");
        printf("  8.Memory Stats  0.Exit\n");
        sep2();
        printf("  Choice: ");
        scanf("%d", &choice);
        clearInput();

        switch (choice) {
        case 1:
            heapMenu(1);
            break;
        case 2:
            heapMenu(0);
            break;
        case 3:
            bstMenu();
            break;
        case 4:
            avlMenu();
            break;
        case 5:
            hashMenu();
            break;
        case 6:
            compareMode();
            break;
        case 7:
            worstCaseDemo();
            break;
        case 8:
            printMemStats();
            break;
        case 0:
            printf("\n  Goodbye!\n\n");
            break;
        default:
            printf("  Invalid choice.\n");
            break;
        }
    } while (choice != 0);

    return 0;
}
