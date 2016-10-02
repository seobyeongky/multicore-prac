/*
 * Utilities
 *
 * @author Byeongky Seo
 * @since 2016-08-16
 */

#include "util.h"
#include "config.h"

#include <list>
#include <pthread.h>

using namespace std;

_64Bit g_pow2[MAX_HOUSE];

void InitUtil() {
    g_pow2[0] = 1;
    for (int i = 1; i < MAX_HOUSE; i++) {
        g_pow2[i] = g_pow2[i - 1] * 2;
    }
}

template <typename T>
class VectorPool {
public:
    VectorPool()
        : free_vector_list_()
        , mutex_(PTHREAD_MUTEX_INITIALIZER) {}

    /**
     *  Allocate the vector
     *  return      Allocated vector
     */
    vector<T> & Alloc() {
        if (free_vector_list_.empty()) {
            return *(new vector<T>());
        }

        pthread_mutex_lock(&mutex_);
        vector<T> * ret = free_vector_list_.front();
        free_vector_list_.pop_front();
        pthread_mutex_unlock(&mutex_);
        return *ret;
    }

    /**
     *  Free the vector
     *  param[in]   vec     Vector to free
     */
    void Free(vector<T> & vec) {
        pthread_mutex_lock(&mutex_);
        free_vector_list_.push_back(&vec);
        pthread_mutex_unlock(&mutex_);
    }

private:
    list<vector<T> *> free_vector_list_;
    pthread_mutex_t mutex_;
};

VectorPool<int> g_int_vector_pool;
VectorPool<char> g_char_vector_pool;

vector<int> & AllocIntVector() {
    return g_int_vector_pool.Alloc();
}

void FreeIntVector(vector<int> & vec) {
    g_int_vector_pool.Free(vec);
}

vector<char> & AllocCharVector() {
    return g_char_vector_pool.Alloc();
}

void FreeCharVector(vector<char> & vec) {
    g_char_vector_pool.Free(vec);
}
