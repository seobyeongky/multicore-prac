/*
 * MyMutex class implementation source
 *
 * @author Byeongky Seo
 * @since 2016-10-25
 */

#include "my_mutex.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

const int EMPTY = -1;

MyMutex::MyMutex()
    : throne_(EMPTY), dummy_(2016) {}

/*
 * Lock
 */
void MyMutex::Lock(int thread_id) {
    TakeTheIronThrone(thread_id);
    if (throne_ == thread_id) return; // fast hi-pass for free situation

    int count = 0;
    do {
        // So we wait
        for (int j  = 0; j < 64; j++) {
            dummy_ += 398;
            dummy_ ^= 0x0018262;
        }

        if (count++ > 10) {
            // There is no hope for my turn => just get some sleep
            timespec time_to_sleep = {0, 1000000};
            nanosleep(&time_to_sleep, nullptr);
        }

        TakeTheIronThrone(thread_id); // Try again     
    // Check weather my try was hit.
    // This is the unique passing condition.
    } while (throne_ != thread_id); 
}

/*
 * Unlock
 */
void MyMutex::Unlock(int thread_id) {
    throne_ = EMPTY;
}

/*
 * Take the Iron Throne if empty
 */
inline void MyMutex::TakeTheIronThrone(int thread_id) {
    __sync_val_compare_and_swap(&throne_, EMPTY, thread_id);
}
