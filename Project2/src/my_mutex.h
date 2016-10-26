/*
 * MyMutex class header
 *
 * @author Byeongky Seo
 * @since 2016-10-25
 */

#ifndef __PROJECT2_SRC_MY_MUTEX_H__
#define __PROJECT2_SRC_MY_MUTEX_H__

// My mutex class
// Simple implementation of lock-unlocking methodology
class MyMutex {
public:
    MyMutex();

    void Lock(int thread_id);
    void Unlock(int thread_id);

private:
    int throne_;
    int dummy_;

    void TakeTheIronThrone(int thread_id);
};


#endif // __PROJECT2_SRC_MY_MUTEX_H__
