#include "context.h"
#include "reader.h"
#include "bfs.h"
#include "util.h"

#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <pthread.h>
#include <algorithm>
#include <math.h>

using namespace std;

Context g_context;
int g_house_index;
pthread_mutex_t g_house_index_mutex = PTHREAD_MUTEX_INITIALIZER;

class Worker {
public:
    Worker(int num_allowed_threads)
        : thread_()
        , result_()
        , num_allowed_threads_(num_allowed_threads) {
    }

    void Work() {
        pthread_create(&thread_, 0, BFSThreadStatic, this);
    }

    void JoinAndUpdateResult(BFSResult & result_to_update) {
        pthread_join(thread_, (void **)nullptr);
        result_to_update.Update(result_);
    }


private:
    pthread_t thread_;
    BFSResult result_;
    int num_allowed_threads_;

    void *BFSThread() {
        while (g_house_index < (int)g_context.houses.size() - 1) {

            pthread_mutex_lock(&g_house_index_mutex);
            int house_index = g_house_index++;
            pthread_mutex_unlock(&g_house_index_mutex);

            if (house_index >= (int)g_context.houses.size() - 1) {
                break;
            }

            int house = g_context.houses[house_index];
            //printf("Begin work : %d\n", house);
            if (num_allowed_threads_ == 1) {
                result_.Update(BFSSingleThread(house));
            } else {
                result_.Update(BFSMultiThread(house, num_allowed_threads_));
            }
        }

        return &result_;
    }

    static void *BFSThreadStatic(void *self) {
        return ((Worker *)self)->BFSThread();
    }
};


int main() {
    InitUtil();
    g_context.visit_bitmap_mutex = PTHREAD_MUTEX_INITIALIZER;

    Reader().Read(&g_context);
    
#ifdef DBG_PRINT
    printf("total %d vertices.\n", g_context.num_vertices);

    for (int i = 1; i <= g_context.num_vertices; i++) {
        printf("%d : ", i);
        for (int neighbor : g_context.adjlist[i]) {
            printf("%d ", neighbor);
        }
        printf("\n");
    }
#endif
 
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    int max_threads_per_worker = ceil((float)num_cores / g_context.houses.size());

    vector<int> num_threads_per_worker_arr;
    if (num_cores <= (int)g_context.houses.size()) {
        num_threads_per_worker_arr.resize(num_cores, 1);
    } else {
        num_threads_per_worker_arr.resize(g_context.houses.size(), max_threads_per_worker);
        int num_threads = max_threads_per_worker * g_context.houses.size();
        for (int i = 0; num_threads > num_cores; i++) {
            num_threads--;
            num_threads_per_worker_arr[i]--;
        }
    }

    vector<Worker*> workers;
    for (int num_threads_per_worker : num_threads_per_worker_arr) {
        //printf("worker : %d\n", num_threads_per_worker);
        workers.push_back(new Worker(num_threads_per_worker));
    }
    
    for (Worker * worker : workers) {
        worker->Work();
    }
    
    BFSResult result;
    for (Worker * worker : workers) {
        worker->JoinAndUpdateResult(result);
    }

    printf("%d\n%d\n", result.min_dist, result.max_dist);

    return 0;
}
