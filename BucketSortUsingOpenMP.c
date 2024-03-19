s#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define ARRAY_SIZE 10
#define NUM_BUCKETS 4

void bucketSort(int arr[], int n, int num_buckets) {
   
    int max_val = arr[0];
    int min_val = arr[0];
    for (int i = 1; i < n; ++i) {
        if (arr[i] > max_val)
            max_val = arr[i];
        if (arr[i] < min_val)
            min_val = arr[i];
    }


    int range = (max_val - min_val) / num_buckets + 1;

    int **buckets = (int **)malloc(num_buckets * sizeof(int *));
    int *bucket_sizes = (int *)calloc(num_buckets, sizeof(int));

    for (int i = 0; i < num_buckets; i++) {
        buckets[i] = (int *)malloc(n * sizeof(int));
    }

    #pragma omp parallel for shared(arr, buckets, bucket_sizes, min_val, range, n, num_buckets) default(none)
    for (int i = 0; i < n; ++i) {
        int bucket_index = (arr[i] - min_val) / range;
        #pragma omp atomic
        bucket_sizes[bucket_index]++;
        int local_index = bucket_sizes[bucket_index] - 1;
        buckets[bucket_index][local_index] = arr[i];
    }

    #pragma omp parallel for shared(buckets, bucket_sizes, num_buckets) default(none)
    for (int i = 0; i < num_buckets; ++i) {
        for (int j = 0; j < bucket_sizes[i]; ++j) {
            for (int k = 0; k < bucket_sizes[i] - j - 1; ++k) {
                if (buckets[i][k] > buckets[i][k + 1]) {
                    int temp = buckets[i][k];
                    buckets[i][k] = buckets[i][k + 1];
                    buckets[i][k + 1] = temp;
                }
            }
        }
    }


    int index = 0;
    for (int i = 0; i < num_buckets; ++i) {
        for (int j = 0; j < bucket_sizes[i]; ++j) {
            arr[index++] = buckets[i][j];
        }
    }

    for (int i = 0; i < num_buckets; i++) {
        free(buckets[i]);
    }
    free(buckets);
    free(bucket_sizes);
}

int main() {
    int arr[ARRAY_SIZE] = {29, 25, 3, 49, 9, 37, 21, 43, 19, 7};
    bucketSort(arr, ARRAY_SIZE, NUM_BUCKETS);

    printf("Sorted array: ");
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}
