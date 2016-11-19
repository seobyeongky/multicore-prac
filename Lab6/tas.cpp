#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

#define NUM_THREAD      8
#define NUM_WORK        1000000

int g_counter;
int gap[128]; // to allocate g_counter & object_tas in different cache line
int object_tas;

void lock(int *lock_object) {
    while(__sync_lock_test_and_set(lock_object, 1) == 1) {}
}

void unlock(int *lock_object) {
    *lock_object = 0;
    __sync_synchronize();
}

void *Work(void *args) {
    for (int i = 0; i < NUM_WORK; i++) {
        lock(&object_tas);
        g_counter++;
        unlock(&object_tas);
    }
}

int main(void) {
    pthread_t threads[NUM_THREAD];

    timeval start, end;
    gettimeofday(&start, NULL);

    for (int i = 0; i < NUM_THREAD; i++) {
        pthread_create(&threads[i], 0, Work, 0);
    }
    for (int i = 0; i < NUM_THREAD; i++) {
        pthread_join(threads[i], 0);
    }

    gettimeofday(&end, NULL);
    printf("time: %lld\n", (end.tv_sec * 1000000 + end.tv_usec)
            - (start.tv_sec * 1000000 + start.tv_usec));

    printf("g_counter: %d\n", g_counter);

    return 0;
}
