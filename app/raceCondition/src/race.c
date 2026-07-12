#include "stdio.h"
#include <pthread.h>

#define ITERATION 100000

long counter = 0;

void *worker(void* arg)
{
    for (int i = 0; i < ITERATION; i++)
    {
        counter++;
    }
    return NULL;
}

int main(void)
{
    pthread_t t1, t2;

    pthread_create(&t1, NULL, worker, NULL);
    pthread_create(&t2, NULL, worker, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Expected: %d\n", 2 * ITERATION);
    printf("Actual: %ld\n", counter);
    return 0;
}