#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "display.h"

typedef struct {
    int *arr;
    int size;
} HeapView;

typedef struct {
    int *arr;
    int *bf;
    int showBalance;
} VirtualView;

int getHeight(int size) {
    int height = 0;
    int nodes = 1;

    while (size > 0) {
        height++;
        size -= nodes;
        nodes <<= 1;
    }
    return height;
}

void printSpaces(int n) {
    int i;

    for (i = 0; i < n; i++) {
        putchar(' ');
    }
}

static int max2(int a, int b) {
    return a > b ? a : b;
}

static int textWidth(const char *text) {
    return (int)strlen(text);
}

static int intWidth(int value) {
    char buffer[32];

    snprintf(buffer, sizeof(buffer), "%d", value);
    return textWidth(buffer);
}

static int balanceWidth(int value) {
    char buffer[32];

    snprintf(buffer, sizeof(buffer), "%+d", value);
    return textWidth(buffer);
}

static void clearLine(char *line, int width) {
    memset(line, ' ', (size_t)width);
    line[width] = '\0';
}

static void printLine(char *line, int width) {
    while (width > 0 && line[width - 1] == ' ') {
        width--;
    }
    line[width] = '\0';
    printf("%s\n", line);
}

static void putCentered(char *line, int width, int center, const char *text) {
    int len = textWidth(text);
    int start = center - len / 2;
    int i;

    if (start < 0) {
        start = 0;
    }
    if (start + len > width) {
        start = width - len;
    }
    if (start < 0) {
        return;
    }

    for (i = 0; i < len; i++) {
        line[start + i] = text[i];
    }
}

static int heapHasNode(void *ctx, int idx) {
    HeapView *view = (HeapView *)ctx;
    return idx >= 0 && idx < view->size;
}

static void heapValueText(void *ctx, int idx, char *buffer, size_t size) {
    HeapView *view = (HeapView *)ctx;
    snprintf(buffer, size, "%d", view->arr[idx]);
}

static void heapExtraText(void *ctx, int idx, char *buffer, size_t size) {
    (void)ctx;
    (void)idx;
    (void)size;
    buffer[0] = '\0';
}

static int virtualHasNode(void *ctx, int idx) {
    VirtualView *view = (VirtualView *)ctx;
    return idx >= 0 && idx < VIRT_ARR_SIZE && view->arr[idx] != EMPTY_SLOT;
}

static void virtualValueText(void *ctx, int idx, char *buffer, size_t size) {
    VirtualView *view = (VirtualView *)ctx;
    snprintf(buffer, size, "%d", view->arr[idx]);
}

static void virtualExtraText(void *ctx, int idx, char *buffer, size_t size) {
    VirtualView *view = (VirtualView *)ctx;

    if (!view->showBalance || !view->bf || view->arr[idx] == EMPTY_SLOT) {
        buffer[0] = '\0';
        return;
    }

    snprintf(buffer, size, "%+d", view->bf[idx]);
}

static void drawTree(
    void *ctx,
    int height,
    int showExtra,
    int cellWidth,
    int (*hasNode)(void *, int),
    void (*valueText)(void *, int, char *, size_t),
    void (*extraText)(void *, int, char *, size_t)
) {
    int slotWidth;
    int width;
    int levelStart;
    int level;
    char *line;

    if (height <= 0) {
        return;
    }

    slotWidth = cellWidth + 2;
    width = slotWidth << height;
    line = (char *)malloc((size_t)width + 1U);
    if (!line) {
        printf("  [display error]\n");
        return;
    }

    levelStart = 0;
    printf("\n");

    for (level = 0; level < height; level++) {
        int count = 1 << level;
        int segment = width >> level;
        int i;

        clearLine(line, width);
        for (i = 0; i < count; i++) {
            int idx = levelStart + i;
            int center = segment / 2 + i * segment;
            char buffer[32];

            if (!hasNode(ctx, idx)) {
                continue;
            }
            valueText(ctx, idx, buffer, sizeof(buffer));
            putCentered(line, width, center, buffer);
        }
        printLine(line, width);

        if (showExtra) {
            clearLine(line, width);
            for (i = 0; i < count; i++) {
                int idx = levelStart + i;
                int center = segment / 2 + i * segment;
                char buffer[32];

                if (!hasNode(ctx, idx)) {
                    continue;
                }
                extraText(ctx, idx, buffer, sizeof(buffer));
                putCentered(line, width, center, buffer);
            }
            printLine(line, width);
        }

        if (level < height - 1) {
            clearLine(line, width);
            for (i = 0; i < count; i++) {
                int idx = levelStart + i;
                int center = segment / 2 + i * segment;
                int leftChild = 2 * idx + 1;
                int rightChild = 2 * idx + 2;
                int childOffset = segment / 4;
                int leftPos = center - childOffset / 2;
                int rightPos = center + childOffset / 2;

                if (!hasNode(ctx, idx)) {
                    continue;
                }
                if (hasNode(ctx, leftChild) && leftPos >= 0 && leftPos < width) {
                    line[leftPos] = '/';
                }
                if (hasNode(ctx, rightChild) && rightPos >= 0 && rightPos < width) {
                    line[rightPos] = '\\';
                }
            }
            printLine(line, width);
        }

        levelStart += count;
    }

    printf("\n");
    free(line);
}

void printHeapTree(int *arr, int size) {
    HeapView view;
    int i;
    int cellWidth = 1;

    if (!arr || size <= 0) {
        printf("  [Empty Heap]\n\n");
        return;
    }

    for (i = 0; i < size; i++) {
        cellWidth = max2(cellWidth, intWidth(arr[i]));
    }

    view.arr = arr;
    view.size = size;
    drawTree(&view, getHeight(size), 0, cellWidth, heapHasNode, heapValueText, heapExtraText);
}

void printVirtualTree(int *arr, int height, int *bf, int showBalance) {
    VirtualView view;
    int limit, i, cellWidth = 1;
    int actualNodes = 0, dispHeight;

    if (!arr || height <= 0) {
        printf("  [Empty Tree]\n\n");
        return;
    }

    if (height > MAX_DISP_HEIGHT) {
        height = MAX_DISP_HEIGHT;
    }

    limit = (1 << height) - 1;
    if (limit > VIRT_ARR_SIZE) {
        limit = VIRT_ARR_SIZE;
    }

    for (i = 0; i < limit; i++) {
        if (arr[i] == EMPTY_SLOT) continue;
        actualNodes++;
        cellWidth = max2(cellWidth, intWidth(arr[i]));
        if (showBalance && bf)
            cellWidth = max2(cellWidth, balanceWidth(bf[i]));
    }

    /*
     * For sparse / degenerate trees, using the full height causes
     * width = slotWidth * 2^height to grow exponentially even though
     * most slots are empty.  Instead, derive dispHeight from the
     * actual node count: find the smallest h such that 2^h >= n+1,
     * then add one level for visual breathing room.
     */
    dispHeight = 1;
    while ((1 << dispHeight) < actualNodes + 1) dispHeight++;
    dispHeight++;                           /* one extra level        */
    if (dispHeight > height) dispHeight = height;
    if (dispHeight < 1)      dispHeight = 1;

    view.arr = arr;
    view.bf  = bf;
    view.showBalance = showBalance;
    drawTree(&view, dispHeight, showBalance && bf != NULL, cellWidth,
             virtualHasNode, virtualValueText, virtualExtraText);
}
