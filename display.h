#ifndef DISPLAY_H
#define DISPLAY_H

#define EMPTY_SLOT      (-99999)
#define MAX_DISP_HEIGHT 6
#define VIRT_ARR_SIZE   127

int  getHeight(int size);
void printSpaces(int n);
void printHeapTree(int *arr, int size);
void printVirtualTree(int *arr, int height, int *bf, int showBalance);

#endif
