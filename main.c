#include <stdio.h>
#include <stdlib.h>
#define N 100
#define B 4
#define FILE_NAME_MAX_LENGTH 10
#define ITEMS_PER_PAGE 10
#define OUTPUT_BUFFER_CAPACITY 12
#define DATA_PATH "data_large.txt"

void sort();

void writeToRunFile(int outputBuffer[], int *size, int *runNumber);

void insertToHeap(int array[], int *heapSize, int value);

void insertToList(int array[], int *listSize, int value);

void insertToOutputBuffer(int array[], int *size, int value);

int removeFromHeap(int array[], int *heapSize);

void moveListToHeap(int array[], int *heapSize, int *listSize);

void heapify(int array[], int heapSize, int i);

int readPageFromFile(int array[], int *size, FILE *file);

void printArray(int array[], int heapSize);

void cleanUp(int buffer[], int inputBuffer[], int outputBuffer[], int *heapSize, int *listSize, int *inputBufferSize, int *outputBufferSize,
             int *runNumber);

void swap(int *a, int *b);

int main() {
    sort();
    return 0;
}

void sort() {
    int buffer[B];
    int inputBuffer[ITEMS_PER_PAGE];
    int outputBuffer[OUTPUT_BUFFER_CAPACITY];
    int heapSize = 0;
    int listSize = 0;
    int inputBufferSize = 0;
    int outputBufferSize = 0;
    int runNumber = 0;
    int shouldStop = 0;
    int isEndOfFile = 0;
    int i = 0;
    int element;

    FILE *file = fopen(DATA_PATH, "r");

    while (!shouldStop) {
        // If no elements in input buffer, read a page from file
        if (i >= inputBufferSize) {
            isEndOfFile = readPageFromFile(inputBuffer, &inputBufferSize, file);
            i = 0;
        }

        if (isEndOfFile && i >= inputBufferSize - 1) {
            shouldStop = 1;
            cleanUp(buffer, inputBuffer, outputBuffer, &heapSize, &listSize, &inputBufferSize, &outputBufferSize, &runNumber);
        }

        // There is space for next element, insert the element recently read from file into the heap
        else if (heapSize + listSize < B && inputBufferSize) {
            element = inputBuffer[i++];
            if (outputBufferSize == 0 || outputBuffer[outputBufferSize - 1] < element)
                insertToHeap(buffer, &heapSize, element);
            else
                insertToList(buffer, &listSize, element);

        }

        // No space for next element, send first element of the heap to output buffer
        else {
            if (outputBufferSize == 0 || (listSize < B && buffer[listSize] > outputBuffer[outputBufferSize - 1])) {
                insertToOutputBuffer(outputBuffer, &outputBufferSize, removeFromHeap(buffer, &heapSize));
            } else {
                writeToRunFile(outputBuffer, &outputBufferSize, &runNumber);
                printf("RUN  = ", runNumber);
                printArray(outputBuffer, outputBufferSize);
                moveListToHeap(buffer, &heapSize, &listSize);

                // In the end, if still there are elements in the heap, empty heap
                // if (shouldStop && heapSize > 0) {
                //     while (heapSize > 0)
                //         insertToOutputBuffer(outputBuffer, &outputBufferSize, removeFromHeap(buffer, &heapSize));

                //     writeToRunFile(outputBuffer, &outputBufferSize, &runNumber);
                //     printf("RUN  = ", runNumber);
                //     printArray(outputBuffer, outputBufferSize);
                // }
            }
        }

        printf("OUT  = ");
        printArray(outputBuffer, outputBufferSize);
        printf("HEAP = ");
        printArray(&buffer[B - heapSize], heapSize);
        printf("LIST = ");
        printArray(buffer, listSize);
        printf("\n");
    }

    fclose(file);
}

void writeToRunFile(int outputBuffer[], int *size, int *runNumber) {
    FILE *file;
    int i;
    char runFileName[FILE_NAME_MAX_LENGTH];
    sprintf(runFileName, "RUN-%02d.txt", *runNumber);

    file = fopen(runFileName, "w");

    for (i = 0; i < *size; i++)
        fprintf(file, "%d\n", outputBuffer[i]);

    fclose(file);
    (*runNumber)++;
    (*size) = 0;
}

void insertToHeap(int array[], int *heapSize, int value) {
    int i;
    int startOffset = B - (*heapSize) - 1;

    array[startOffset] = value;
    (*heapSize)++;

    if (*heapSize == 1)
        return;

    for (i = (*heapSize) / 2 - 1; i >= 0; i--)
        heapify(&array[startOffset], *heapSize, i);
}

void insertToList(int array[], int *listSize, int value) {
    array[*listSize] = value;
    (*listSize)++;
}

void insertToOutputBuffer(int array[], int *size, int value) {
    array[*size] = value;
    (*size)++;
}

int removeFromHeap(int array[], int *heapSize) {
    int i;
    int startOffset = B - (*heapSize);
    int result = array[startOffset];

    (*heapSize)--;
    startOffset++;

    for (i = (*heapSize) / 2 - 1; i >= 0; i--)
        heapify(&array[startOffset], *heapSize, i);

    return result;
}

void moveListToHeap(int array[], int *heapSize, int *listSize) {
    *heapSize = *listSize;
    *listSize = 0;

    int i;
    int startOffset = B - (*heapSize);

    for (i = (*heapSize) / 2 - 1; i >= 0; i--)
        heapify(&array[startOffset], *heapSize, i);
}

void heapify(int array[], int heapSize, int i) {
    if (heapSize <= 1)
        return;
    int smallest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    if (l < heapSize && array[l] < array[smallest])
        smallest = l;
    if (r < heapSize && array[r] < array[smallest])
        smallest = r;
    if (smallest != i) {
        swap(&array[i], &array[smallest]);
        heapify(array, heapSize, smallest);
    }
}

int readPageFromFile(int array[], int *size, FILE *file) {
    int i = 0;
    *size = 0;
    array;

    while (i < ITEMS_PER_PAGE && !feof(file)) {
        fscanf(file, "%d", &array[i]);
        i++;
        (*size)++;
        if (feof(file))
            return 1;
    }

    return 0;
}

void printArray(int array[], int size) {
    int i;
    for (i = 0; i < size; ++i)
        printf("%d ", array[i]);
    printf("\n");
}

void cleanUp(int buffer[], int inputBuffer[], int outputBuffer[], int *heapSize, int *listSize, int *inputBufferSize, int *outputBufferSize,
             int *runNumber) {
    int i;
    int element;

    // Condition yaz
    while ((*heapSize) > 0 && (*listSize) > 0 && (*inputBufferSize) > 0) {
        // There is space for next element, insert the element recently read from file into the heap
        if ((*heapSize) + (*listSize) < B && (*inputBufferSize)) {
            element = inputBuffer[i++];
            if ((*outputBufferSize) == 0 || outputBuffer[(*outputBufferSize) - 1] < element)
                insertToHeap(buffer, heapSize, element);
            else
                insertToList(buffer, listSize, element);
        }

        // No space for next element, send first element of the heap to output buffer
        else {
            if ((*outputBufferSize) == 0 || ((*listSize) < B && buffer[*listSize] > outputBuffer[(*outputBufferSize) - 1])) {
                insertToOutputBuffer(outputBuffer, outputBufferSize, removeFromHeap(buffer, heapSize));
            } else {
                writeToRunFile(outputBuffer, outputBufferSize, runNumber);
                printf("RUN  = ", *runNumber);
                printArray(outputBuffer, *outputBufferSize);
                moveListToHeap(buffer, heapSize, listSize);

                // In the end, if still there are elements in the heap, empty heap
                // if (shouldStop && heapSize > 0) {
                //     while (heapSize > 0)
                //         insertToOutputBuffer(outputBuffer, &outputBufferSize, removeFromHeap(buffer, &heapSize));

                //     writeToRunFile(outputBuffer, &outputBufferSize, &runNumber);
                //     printf("RUN  = ", runNumber);
                //     printArray(outputBuffer, outputBufferSize);
                // }
            }
        }
    }
}

void swap(int *a, int *b) {
    int temp = *b;
    *b = *a;
    *a = temp;
}