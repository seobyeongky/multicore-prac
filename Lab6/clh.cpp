#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREAD      8
#define NUM_WORK        1000000

int g_counter;

int * tail = NULL;

struct CLHMutex {
    int * x;

    void lock() {
        x = (int*)malloc(sizeof(int));
        *x = 0;
        int * old_one = __sync_lock_test_and_set(&tail, x);

        if (old_one == NULL) {
            return;
        }

        int cnt = 0;
        while (*old_one == 0) {
            if (cnt++ > 100)
            {
                sched_yield();
            }
        }
        free(old_one);
   }

    void unlock() {
        *x = 1;
        __sync_synchronize();
    }
};


void *Work(void *args) {
    CLHMutex mutex;
    for (int i = 0; i < NUM_WORK; i++) {
        mutex.lock();
        g_counter++;
        mutex.unlock();
    }
}

bool monitor_stop = false;
void *Monitor(void *args) {
    while (!monitor_stop) {

        printf("%d\n", g_counter);

        sleep(1);


    }
}

int main(void) {
    pthread_t threads[NUM_THREAD];

    timeval start, end;
    gettimeofday(&start, NULL);

    for (int i = 0; i < NUM_THREAD; i++) {
        pthread_create(&threads[i], 0, Work, 0);
    }

    pthread_t monitor_thread;
    pthread_create(&monitor_thread, 0, Monitor, 0);

    for (int i = 0; i < NUM_THREAD; i++) {
        pthread_join(threads[i], 0);
    }


    gettimeofday(&end, NULL);
    printf("time: %lld\n", (end.tv_sec * 1000000 + end.tv_usec)
            - (start.tv_sec * 1000000 + start.tv_usec));

    printf("g_counter: %d\n", g_counter);

    monitor_stop = true;
    pthread_join(monitor_thread, 0);


    return 0;
}
