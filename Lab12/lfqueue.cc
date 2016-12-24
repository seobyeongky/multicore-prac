#include <atomic>
#include <cstdint>
#include <cstdio>

using namespace std;

/**
 * Concurrent Latch-Free Queue that supports multiple-producers and multiple-consumers.
 * It is a bounded, blocking, wait-free Queue.
 * It uses fixed data structure and it can only store limited number of elements.
 * When a queue is full, enqueue will spinlock until it finds a space.
 * When a queue is empty, dequeue will spinlock until it finds a new element.
 * New enqueue might spinlock if the old element at the same index hasn't been dequeued.
 * This happens if the consumer of the old element preempts in the middle of dequeue. */
template <typename T> 
class lfqueue
{
  private:
    uint32_t capacity;   //capacity of queue
    T * arr;
    atomic<uint32_t> cnsu_counter;
    atomic<uint32_t> prod_counter;

  public:
    lfqueue(const uint32_t capacity)
        : capacity(capacity)
        , arr(new T[capacity])
        , cnsu_counter(capacity - 1)
        , prod_counter(capacity - 1)
    {
    };

    ~lfqueue()
    {
        delete arr;
    };

    void enqueue(const T value)
    {
        prod_counter++;
        cnsu_counter %= capacity;

        int r_prod_counter = prod_counter % capacity;
        while (1) {
            int r_cnsu_counter = cnsu_counter % capacity;
            if (cnsu_counter > prod_counter) {
                arr[prod_counter] = value;
                return;
            }
        }
    };

    T dequeue()
    {
        cnsu_counter++;
        cnsu_counter %= capacity;

        int r_cnsu_counter = cnsu_counter % capacity;
        while (1) {
            int r_prod_counter = prod_counter % capacity;
            printf("%d\n", r_prod_counter >= r_cnsu_counter);
            if (prod_counter >= cnsu_counter) {
                return arr[cnsu_counter];
            }
        }
    };
};

