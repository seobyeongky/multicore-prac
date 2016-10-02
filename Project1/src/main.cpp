/**
 * Main entry source
 *
 * @author Byeongky Seo
 * @since 2016-09-16
 */

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

// HouseWorker competitively takes a house.
// And find the min/max shortest distance from the house.
class HouseWorker {
public:
    HouseWorker(int num_allowed_threads)
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

            // The number of allowed threads is a main factor of
            // choice of the search algorithm.
            if (num_allowed_threads_ == 1) {
                result_.Update(BFSSingleThread(house));
            } else {
                result_.Update(BFSMultiThread(house, num_allowed_threads_));
            }
        }

        return &result_;
    }

    static void *BFSThreadStatic(void *self) {
        return ((HouseWorker *)self)->BFSThread();
    }
};


/**
 *  Main entry function
 */
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
        // make num_cores * HouseWorkers...
        // ex) cores(4) houses(15) => HouseWorkers(1 1 1 1)
        num_threads_per_worker_arr.resize(num_cores, 1);
    } else {
        // assign appropriate cores to each of the HouseWorkers.
        // ex) cores(36) houses(10) => HouseWorkers(3 3 3 3 4 4 4 4 4 4)
        num_threads_per_worker_arr.resize(g_context.houses.size(), max_threads_per_worker);
        int num_threads = max_threads_per_worker * g_context.houses.size();
        for (int i = 0; num_threads > num_cores; i++) {
            num_threads--;
            num_threads_per_worker_arr[i]--;
        }
    }

    vector<HouseWorker*> workers;
    for (int num_threads_per_worker : num_threads_per_worker_arr) {
        workers.push_back(new HouseWorker(num_threads_per_worker));
    }
    
    for (HouseWorker * worker : workers) {
        worker->Work();
    }
    
    BFSResult result;
    for (HouseWorker * worker : workers) {
        worker->JoinAndUpdateResult(result);
    }

    printf("%d\n%d\n", result.min_dist, result.max_dist);

    return 0;
}
