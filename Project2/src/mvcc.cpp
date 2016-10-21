#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <iostream>

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
    list<Data> data_list;
    int num_updates;
};

struct ActiveThread {
    int id;
    int version;

    ActiveThread() {}
    ActiveThread(int id, int version)
        : id(id), version(version) {}
};

int g_k = 0;
int g_num_threads;
int g_dura;
bool g_over = false;
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
list<ActiveThread> g_active_thread_list;
vector<ThreadContext> g_contexts;
const int A_PLUS_B = 100;

template <typename T>
inline T Square(T x) {
    return x * x;
}

void *UpdateAB(void *arg) {
    ThreadContext & self = *((ThreadContext *)arg);
    while (!g_over) {
        pthread_mutex_lock(&g_mutex);
        int my_version = g_k++;
        auto it = g_active_thread_list.insert(g_active_thread_list.end(), ActiveThread());
        list<ActiveThread> RV(g_active_thread_list);
        pthread_mutex_unlock(&g_mutex);

        int i = rand() % g_contexts.size();
        bool contains = false;
        int target_version;
        Data data_i;
        
        for (ActiveThread active_thread : RV) {
            if (active_thread.id == i) {
                contains = true;
                target_version = active_thread.version;
                break;
            }
        }

        ThreadContext & thread_i = g_contexts[i];
        if (contains) {
            for (Data data : thread_i.data_list) {
                if (data.version == target_version) {
                    data_i = data;
                    break;
                }
            }
        }

        Data data_x(self.data_list.front());

        self.data_list.push_front(Data(my_version, data_x.A + data_i.A % 100, data_x.B - data_i.A % 100));

        pthread_mutex_lock(&g_mutex);
        g_active_thread_list.erase(it);
        pthread_mutex_unlock(&g_mutex);

        self.num_updates++;
    }

    return nullptr;
}

void PrintSum() {
    int sum = 0;
    for (int i = 0; i < g_num_threads; i++) {
        ThreadContext * cx = &g_contexts[i];
        Data & data = cx->data_list.front();
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
    int sum = 0;
    int sq_sum = 0;
    for (ThreadContext & cx : g_contexts) {
        sum += cx.num_updates;
        sq_sum += Square(cx.num_updates);
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
    vector<pthread_t> threads(g_num_threads);
    g_contexts.resize(g_num_threads);

    for (int i = 0; i < g_num_threads; i++) {
        ThreadContext * cx = &g_contexts[i];
        cx->id = i;
        Data data;
        data.A = rand() % A_PLUS_B;
        data.B = A_PLUS_B - data.A;
        data.version = g_k++;
        cx->data_list.push_back(data);
        cx->num_updates = 0;
    }

    PrintSum();
    for (int i = 0; i < g_num_threads; i++) {
        pthread_create(&threads[i], 0, UpdateAB, (void *)&g_contexts[i]);
    }

    sleep(g_dura);
    g_over = true;

    for (int i = 0; i < g_num_threads; i++) {
        pthread_join(threads[i], 0);
    }

    cout << "Updating is over." << endl;
    PrintSum();
    PrintTotalThroughput();
    PrintFairness();

    return 0;
}
