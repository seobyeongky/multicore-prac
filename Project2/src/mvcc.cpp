/*
 * MVCC source
 *
 * @author Byeongky Seo
 * @since 2016-10-21
 */


// Weather use MyMutex or pthread_mutex for the mutex.
#define MY_MUTEX

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <iostream>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "node.h"

using namespace std;

// Data structure for A, B, version
struct Data {
    int version;
    int A;
    int B;

    Data() {}
    
    Data(int version, int A, int B)
        : version(version), A(A), B(B) {}
};

// Thread working context structure
struct ThreadContext {
    int id;
    Node<Data> * data_head;
    int num_updates;
};

// Node data for globally shared active thread list
struct ActiveThread {
    int id;
    int version;

    ActiveThread() {}
    ActiveThread(int id, int version)
        : id(id), version(version) {}
};

int g_exec_order = 0; // Global execution order
int g_num_threads; // Number of threads
int g_dura; // Duration parameter input
bool g_over = false; // Timeover flag
bool g_verbose = false; // Verbose paramter input flag
#ifdef MY_MUTEX
#include "my_mutex.h"
MyMutex g_mutex;
#define LOCK(x) x.Lock(thread_id)
#define UNLOCK(x) x.Unlock(thread_id)
#else
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
#define LOCK(x) pthread_mutex_lock(&x)
#define UNLOCK(x) pthread_mutex_unlock(&x)
#endif
Node<ActiveThread> * g_active_thread_head; // Globally shared active thread list
vector<ThreadContext> g_contexts; // Thread working context container
const int A_PLUS_B = 100; // Constant for initializing A, B
const int VERBOSE_OUTPUT_INTERVAL = 100; // Verbose output interval constant (ms)

/*
 * Square utility function
 */
template <typename T>
inline T Square(T x) {
    return x * x;
}

/*
 * Collect the garabages
 */
void GC() {
    int ref_min_version = INT_MAX;
    int thread_id = g_num_threads + 100;

    LOCK(g_mutex);
    for (Node<ActiveThread> *node = g_active_thread_head;
            node != nullptr; node = node->next) {
        if (node->next == nullptr) {
            ref_min_version = node->data.version;
            break;
        }
    }
    UNLOCK(g_mutex);

    for (ThreadContext & cx : g_contexts) { 
        if (cx.data_head && cx.data_head->data.version < ref_min_version) {
            ref_min_version = cx.data_head->data.version;
        }
    }

    for (int i = 0; i < g_num_threads; i++) {
        ThreadContext & cx = g_contexts[i];
        for (Node<Data> * node = cx.data_head; node != nullptr; node = node->next) {
            if (node->data.version < ref_min_version) {
                // Delete garbage...
                // Because the data two steps before ref_min_version can be referenced,
                // We should go further two steps
                node = node->next;
                if (node != nullptr) {
                    node = node->next;
                }
                Node<Data> * prev_node = node;
                if (node != nullptr) {
                    node = node->next;
                }
                while (node != nullptr) {
                    node = NodeErase(prev_node, node);
                }
                break;
            }
        }
    }
}

/*
 * GC Scheduler
 */
void * ScheduleGC(void *arg) {
    while (!g_over) {
        GC();
    }

    return nullptr;
}

/*
 * A, B Updater (main simulation logic)
 */
void *UpdateAB(void *arg) {
    ThreadContext & self = *((ThreadContext *)arg);
    int thread_id = self.id;
    FILE * out;

    if (g_verbose) {
        char filename[256];
        sprintf(filename, "verbose_thread_%d.txt", self.id);
        out = fopen(filename, "w");
    }

    timespec prev_clock;
    int acc_time = 0;
    int verbose_output_count = 0;
    if (g_verbose) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &prev_clock);
    }

    while (!g_over) {

        LOCK(g_mutex);
        int my_version = g_exec_order++;
        int my_recent_version = self.data_head->data.version;
        // Push my node to the active thread list
        Node<ActiveThread> * my_node = NodePushFront(&g_active_thread_head, ActiveThread(self.id
                    , my_recent_version));
        // Copy the Read-View
        Node<ActiveThread> *rv = nullptr;
        for (Node<ActiveThread> *node = g_active_thread_head;
                node != nullptr; node = node->next) {
            NodePushFront(&rv, node->data);
        }
        UNLOCK(g_mutex);

        // Select thread_i randomly
        int i = rand() % g_contexts.size();
        bool contains = false;
        int target_version;
        Data data_i;
        
        // First, we find thread_i is in the active thread list
        for (Node<ActiveThread> * ac_node = rv;
                ac_node != nullptr; ac_node = ac_node->next) {
            ActiveThread & active_thread = ac_node->data;
            if (active_thread.id == i) {
                contains = true;
                target_version = active_thread.version;
                break;
            }
        }

        ThreadContext & thread_i = g_contexts[i];
        bool found = false;
        if (contains) {
            // If thread_i is in the active thread list,
            // Find the the data of target version from thread_i's data list
            for (Node<Data> *data_node = thread_i.data_head;
                    data_node != nullptr; data_node = data_node->next) {
                assert(data_node->data.version >= target_version);
                if (data_node->data.version == target_version) {
                    data_i = data_node->data;
                    found = true;
                    break;
                }
            }
        } else {
            // If thread_i is not in the active thread list
            // Find the data of version_k just under the recent_version
            for (Node<Data> *data_node = thread_i.data_head;
                    data_node != nullptr; data_node = data_node->next) {
                if (data_node->data.version < my_version) {
                    data_i = data_node->data;
                    found = true;
                    break;
                }
            }
        }

        if (!found) {
            printf("NOT FOUND!\n");
            printf("contains : %d\n", contains);
            printf("%d -> %d\n", self.id, i);
            for (Node<Data> *data_node = thread_i.data_head;
                    data_node != nullptr; data_node = data_node->next) {
                printf("%d %d\n", data_node->data.version, my_version);
            }
        }

        assert(found);

        Data data_x(self.data_head->data);

        // Push new data node
        NodePushFront(&self.data_head
                , Data(my_version, data_x.A + data_i.A % 100, data_x.B - data_i.A % 100));

        // Erase my node from active thread list
        LOCK(g_mutex);
        NodeErase(&g_active_thread_head, my_node);
        UNLOCK(g_mutex);

        if (g_verbose) {
            timespec now_clock;
            clock_gettime(CLOCK_MONOTONIC_RAW, &now_clock);

            int dt = (now_clock.tv_sec * 1000 + now_clock.tv_nsec / 1000000
                    - prev_clock.tv_sec * 1000 - prev_clock.tv_nsec / 1000000);
            prev_clock = now_clock;
            acc_time += dt;
            if (acc_time >= VERBOSE_OUTPUT_INTERVAL) {
                acc_time -= VERBOSE_OUTPUT_INTERVAL;
                verbose_output_count++;

                int rv_size = 0;
                for (Node<ActiveThread> *node = rv;
                        node != nullptr; node = node->next) {
                    rv_size++;
                }
                fprintf(out, "%d\n", rv_size);
                for (Node<ActiveThread> *node = rv;
                        node != nullptr; node = node->next) {
                    fprintf(out, "%d ", node->data.version);
                }
                fprintf(out, "\n");
                fprintf(out, "%d\n", data_i.version);
                fprintf(out, "\n");
            }
        }

        DestroyNodes(&rv);

        self.num_updates++;
    }

    if (g_verbose) {
        fclose(out);
    }

    return nullptr;
}

/*
 * Print the summation of A, B datas
 */
void PrintSum() {
    int sum = 0;
    for (int i = 0; i < g_num_threads; i++) {
        ThreadContext * cx = &g_contexts[i];
        Data & data = cx->data_head->data;
        sum += data.A;
        sum += data.B;
    }

    cout << "sum : " << sum << endl;
}

/*
 * Print the total throughput of the threads
 */
void PrintTotalThroughput() {
    int sum = 0;
    for (ThreadContext & cx : g_contexts) {
        sum += cx.num_updates;
    }

    cout << "Total throughput : " << ((double)sum / g_dura) << " (updates/sec)" << endl;
}

/*
 * Print the fairness of the threads
 */
void PrintFairness() {
    long long int sum = 0;
    long long int sq_sum = 0;
    for (ThreadContext & cx : g_contexts) {
        sum += cx.num_updates;
        sq_sum += Square((long long int)cx.num_updates);
    }

    double fairness = (double)Square(sum) / (g_num_threads * sq_sum);

    cout << "Fairness : " << fairness << endl;
}

/*
 * Main entry function
 */
int main(int argc, char * argv []) {
    if (argc < 3) {
        puts("Not enough arguments");
        return -1;
    }

    g_num_threads = atoi(argv[1]);
    g_dura = atoi(argv[2]);
    if (argc >= 4 && !strcmp(argv[3], "verbose")) {
        g_verbose = true;
    }

    vector<pthread_t> threads(g_num_threads);
    g_contexts.resize(g_num_threads);

    for (int i = 0; i < g_num_threads; i++) {
        ThreadContext * cx = &g_contexts[i];
        cx->id = i;
        Data data;
        data.A = rand() % A_PLUS_B;
        data.B = A_PLUS_B - data.A;
        data.version = g_exec_order++;
        NodePushFront(&cx->data_head, data);
        cx->num_updates = 0;
    }

    PrintSum();
    for (int i = 0; i < g_num_threads; i++) {
        pthread_create(&threads[i], 0, UpdateAB, (void *)&g_contexts[i]);
    }

    pthread_t gc_thread;
    pthread_create(&gc_thread, 0, ScheduleGC, 0);

    sleep(g_dura);
    g_over = true;

    pthread_join(gc_thread, 0);

    for (int i = 0; i < g_num_threads; i++) {
        pthread_join(threads[i], 0);
    }

    cout << "Updating is over." << endl;
    PrintSum();
    PrintTotalThroughput();
    PrintFairness();

    return 0;
}
