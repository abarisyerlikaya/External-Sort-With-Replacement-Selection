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

int readPageFromFile(int array[], int *size, int *offset, FILE *file);

void printArray(int array[], int heapSize);

void cleanUp(int buffer[], int inputBuffer[], int outputBuffer[], int *heapSize, int *listSize, int *inputBufferSize, int *outputBufferSize, int *runNumber);

int popFromInputBuffer(int inputBuffer[], int *inputBufferSize, int *offset);

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
    int inputBufferOffset = 0;
    int outputBufferSize = 0;
    int runNumber = 0;
    int isEndOfFile = 0;
    int element;

    FILE *file = fopen(DATA_PATH, "r");

    // Loop until end of file reached
    while (!isEndOfFile) {
        // There is space in heap, insert to heap from input buffer
        if (heapSize + listSize < 4) {
            // If input buffer is empty and it's not end of file, then read a page from file
            if (inputBufferSize == 0 && !isEndOfFile)
                isEndOfFile = readPageFromFile(inputBuffer, &inputBufferSize, &inputBufferOffset, file);

            // If input buffer is not empty, insert first element of input buffer to heap or list depending on value
            if (inputBufferSize > 0) {
                element = popFromInputBuffer(inputBuffer, &inputBufferSize, &inputBufferOffset);

                // If output buffer is empty or the last element of output buffer is smaller than the element, insert into heap
                if (outputBufferSize == 0 || outputBuffer[outputBufferSize - 1] < element)
                    insertToHeap(buffer, &heapSize, element);

                // Otherwise, insert into list to protect the order
                else
                    insertToList(buffer, &listSize, element);
            }
        }

        // There is no space in the heap, send the root element of the heap to output buffer
        else {
            // If there is an element in heap and the last element of output buffer is not greater than the first element of heap
            if (listSize < B && (outputBufferSize == 0 || outputBuffer[outputBufferSize - 1] <= buffer[listSize]))
                insertToOutputBuffer(outputBuffer, &outputBufferSize, removeFromHeap(buffer, &heapSize));

            // If list is full or the first element of heap is not insertable to output buffer, finish the run
            else {
                printf("RUN%02d = ", runNumber);
                printArray(outputBuffer, outputBufferSize);
                writeToRunFile(outputBuffer, &outputBufferSize, &runNumber);
                moveListToHeap(buffer, &heapSize, &listSize);
            }
        }

        // Print current status
        printf("OUT   = ");
        printArray(outputBuffer, outputBufferSize);
        printf("HEAP  = ");
        printArray(&buffer[B - heapSize], heapSize);
        printf("LIST  = ");
        printArray(buffer, listSize);
        printf("IN    = ");
        printArray(&inputBuffer[inputBufferOffset], inputBufferSize);
        printf("\n");
    }

    // Loop until heap, list, inputBuffer and outputBuffer is empty to clean up remaining elements after reached end of file
    while (heapSize > 0 || listSize > 0 || inputBufferSize > 0 || outputBufferSize > 0) {
        // There is space in heap and input buffer is not empty, insert to heap from input buffer
        if (heapSize + listSize < 4 && inputBufferSize > 0) {
            element = popFromInputBuffer(inputBuffer, &inputBufferSize, &inputBufferOffset);

            // If output buffer is empty or the last element of output buffer is smaller than the element, insert into heap
            if (outputBufferSize == 0 || outputBuffer[outputBufferSize - 1] < element)
                insertToHeap(buffer, &heapSize, element);

            // Otherwise, insert into list to protect the order
            else
                insertToList(buffer, &listSize, element);
        }

        // There is no space in the heap, send the root element of the heap to output buffer
        else {
            // If there is an element in heap and the last element of output buffer is not greater than the first element of heap
            if (listSize < B && (outputBufferSize == 0 || outputBuffer[outputBufferSize - 1] <= buffer[listSize]))
                insertToOutputBuffer(outputBuffer, &outputBufferSize, removeFromHeap(buffer, &heapSize));

            // If list is full or the first element of heap is not insertable to output buffer, finish the run
            else {
                printf("RUN%02d = ", runNumber);
                printArray(outputBuffer, outputBufferSize);
                writeToRunFile(outputBuffer, &outputBufferSize, &runNumber);
                moveListToHeap(buffer, &heapSize, &listSize);
            }
        }

        // Print current status
        printf("OUT   = ");
        printArray(outputBuffer, outputBufferSize);
        printf("HEAP  = ");
        printArray(&buffer[B - heapSize], heapSize);
        printf("LIST  = ");
        printArray(buffer, listSize);
        printf("IN    = ");
        printArray(&inputBuffer[inputBufferOffset], inputBufferSize);
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

int readPageFromFile(int array[], int *size, int *offset, FILE *file) {
    int i = 0;
    (*size) = 0;
    (*offset) = 0;

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

void cleanUp(int buffer[], int inputBuffer[], int outputBuffer[], int *heapSize, int *listSize, int *inputBufferSize, int *outputBufferSize, int *runNumber) {
}

int popFromInputBuffer(int *inputBuffer, int *inputBufferSize, int *inputBufferOffset) {
    if ((*inputBufferSize) == 0)
        return -1;

    int element = inputBuffer[*inputBufferOffset];

    (*inputBufferOffset)++;
    (*inputBufferSize)--;

    return element;
}

void swap(int *a, int *b) {
    int temp = *b;
    *b = *a;
    *a = temp;
}
