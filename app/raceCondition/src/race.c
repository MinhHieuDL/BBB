#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

#define ITERATION 100000

atomic_long counter = 0;

void *worker(void* arg)
{
    for (int i = 0; i < ITERATION; i++)
    {
        atomic_fetch_add(&counter, 1);
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