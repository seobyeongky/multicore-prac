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

struct Data {
    int version;
    int A;
    int B;

    Data() {}
    
    Data(int version, int A, int B)
        : version(version), A(A), B(B) {}
};

struct ThreadContext {
    int id;
    Node<Data> * data_head;
    int num_updates;
};

struct ActiveThread {
    int id;
    int version;

    ActiveThread() {}
    ActiveThread(int id, int version)
        : id(id), version(version) {}
};

int g_exec_order = 0;
int g_num_threads;
int g_dura;
bool g_over = false;
bool g_verbose = false;
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
Node<ActiveThread> * g_active_thread_head;
vector<ThreadContext> g_contexts;
const int A_PLUS_B = 100;
const int VERBOSE_OUTPUT_INTERVAL = 100;

template <typename T>
inline T Square(T x) {
    return x * x;
}

void GC() {
    int ref_min_version = INT_MAX;
    int thread_id = g_num_threads + 100;

    LOCK(g_mutex);
    //printf("GC AC :");
    for (Node<ActiveThread> *node = g_active_thread_head;
            node != nullptr; node = node->next) {
        if (node->next == nullptr) {
            ref_min_version = node->data.version;
            break;
        }
    }
    //printf("\n");
    UNLOCK(g_mutex);

    for (ThreadContext & cx : g_contexts) { 
        if (cx.data_head && cx.data_head->data.version < ref_min_version) {
            ref_min_version = cx.data_head->data.version;
        }
    }

    //printf("GC under %d\n", ref_min_version);

    for (int i = 0; i < g_num_threads; i++) {
        ThreadContext & cx = g_contexts[i];
        for (Node<Data> * node = cx.data_head; node != nullptr; node = node->next) {
            if (node->data.version < ref_min_version) {
                node = node->next;
                if (node != nullptr) {
                    node = node->next;
                }
                Node<Data> * prev_node = node;
                if (node != nullptr) {
                    node = node->next;
                }
                if (node != nullptr) {
                    //printf("[%d] kill under : %d\n", i, node->data.version);
                }
                while (node != nullptr) {
                    node = NodeErase(prev_node, node);
                }
                break;
            }
        }
    }
}

void * ScheduleGC(void *arg) {
    while (!g_over) {
        GC();
    }

    return nullptr;
}

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
        Node<ActiveThread> * my_node = NodePushFront(&g_active_thread_head, ActiveThread(self.id
                    , my_recent_version));
        Node<ActiveThread> *rv = nullptr;
        //int count = 0;
        for (Node<ActiveThread> *node = g_active_thread_head;
                node != nullptr; node = node->next) {
            //printf("(%d,%d)", node->data.version, count++);
            NodePushFront(&rv, node->data);
        }
        UNLOCK(g_mutex);

        int i = rand() % g_contexts.size();
        bool contains = false;
        int target_version;
        Data data_i;
        
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
        if (contains) {
            for (Node<Data> *data_node = thread_i.data_head;
                    data_node != nullptr; data_node = data_node->next) {
                //printf("[%d] ref %d : %d %d\n", self.id, i, target_version, data_node->data.version);
                assert(data_node->data.version >= target_version);
                if (data_node->data.version == target_version) {
                    data_i = data_node->data;
                    break;
                }
            }
        } else {
            for (Node<Data> *data_node = thread_i.data_head;
                    data_node != nullptr; data_node = data_node->next) {
                //printf("[%d] ref2 %d : %d %d\n", self.id, i, my_recent_version, data_node->data.version);
                if (data_node->data.version < my_recent_version) {
                    data_i = data_node->data;
                    break;
                }
            }
        }

        Data data_x(self.data_head->data);

        NodePushFront(&self.data_head
                , Data(my_version, data_x.A + data_i.A % 100, data_x.B - data_i.A % 100));

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

void PrintTotalThroughput() {
    int sum = 0;
    for (ThreadContext & cx : g_contexts) {
        sum += cx.num_updates;
    }

    cout << "Total throughput : " << ((double)sum / g_dura) << " (updates/sec)" << endl;
}

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

int main(int argc, char * argv []) {
    if (argc < 3) {
        puts("Not enough arguments");
        return -1;
    }

    g_num_threads = atoi(argv[1]);
    g_dura = atoi(argv[2]);
    if (argc >= 4) {
        if (!strcmp(argv[3], "verbose")) {
            g_verbose = true;
        }
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
