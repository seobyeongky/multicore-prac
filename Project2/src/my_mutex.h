#ifndef __PROJECT2_SRC_MY_MUTEX_H__
#define __PROJECT2_SRC_MY_MUTEX_H__

#include <vector>
#include <atomic>

class MyMutex {
public:
    MyMutex();

    void Lock(int thread_id);
    void Unlock(int thread_id);

private:
    int n_;
    int dummy_;
/*    std::atomic<int> counter_;
    int king_;
    std::vector<int> waitings;
    */
};


#endif // __PROJECT2_SRC_MY_MUTEX_H__
