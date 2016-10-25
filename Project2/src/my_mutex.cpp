#include "my_mutex.h"

#include <stdio.h>
#include <time.h>

MyMutex::MyMutex()
    : n_(-1) {}

void MyMutex::Lock(int thread_id) {
    int count = 0;
    do {
        __sync_val_compare_and_swap(&n_, -1, thread_id);
        //jif (count++ > 10 * 1024) {
        //}
    } while (n_ != thread_id);
}

void MyMutex::Unlock(int thread_id) {
    n_ = -1;
}


    /*
MyMutex::MyMutex()
    : counter_(0), king_(0) {}



void MyMutex::Lock() {
    int i = counter_++;
    int count = 0;
    timespec time_to_sleep;
    time_to_sleep.tv_sec = 0;

    if (king_ == i) return;

    while (true) {
        __sync_synchronize();
        if (king_ == i) break;

        //printf("[%d] waiting %d\n", i, count);
        int times = 5 * (i - king_);
        for (int j  = 0; j < times; j++) {
            static_cast<void> (0);
        }
        //count++;
        //if (i - king_ <= 5 || true) continue;
        if (count > 5 * 1024) {
            printf("[%d] gonna sleep %d\n", i, 1000 * (i - king_));
            time_to_sleep.tv_nsec = 1000 * (i - king_);
            nanosleep(&time_to_sleep, 0);
        }
    }
}

void MyMutex::Unlock() {
    king_++;
}
*/
