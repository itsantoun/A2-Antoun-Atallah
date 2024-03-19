#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ARRAY_SIZE 100
#define NUM_THREADS 4

typedef struct {
    int start_index;
    int end_index;
    int *array;
} ThreadData;

int compare_integers(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

void* sort_segment(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    qsort(data->array + data->start_index, data->end_index - data->start_index + 1, sizeof(int), compare_integers);
    return NULL;
}

void merge(int* array, int start1, int end1, int start2, int end2) {
    int temp[end2 - start1 + 1];
    int i = start1, j = start2, k = 0;
    while (i <= end1 && j <= end2) {
        if (array[i] < array[j])
            temp[k++] = array[i++];
        else
            temp[k++] = array[j++];
    }
    while (i <= end1)
        temp[k++] = array[i++];
    while (j <= end2)
        temp[k++] = array[j++];
    for (i = start1, k = 0; i <= end2; i++, k++)
        array[i] = temp[k];
}

int main() {
    int array[ARRAY_SIZE] = {7, 3, 1, 8, 5, 2, 9, 4, 6, 0};
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    int segment_size = ARRAY_SIZE / NUM_THREADS;
    
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].array = array;
        thread_data[i].start_index = i * segment_size;
        thread_data[i].end_index = (i == NUM_THREADS - 1) ? ARRAY_SIZE - 1 : (i + 1) * segment_size - 1;
        pthread_create(&threads[i], NULL, sort_segment, (void*)&thread_data[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);
    
    for (int i = 1; i < NUM_THREADS; i++)
        merge(array, 0, segment_size * i - 1, segment_size * i, (i == NUM_THREADS - 1) ? ARRAY_SIZE - 1 : (i + 1) * segment_size - 1);
    
    printf("Sorted Array: ");
    for (int i = 0; i < ARRAY_SIZE; i++)
        printf("%d ", array[i]);
    printf("\n");
    
    return 0;
}
