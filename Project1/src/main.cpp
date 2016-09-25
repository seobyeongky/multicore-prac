#include "context.h"
#include "reader.h"
#include "bfs.h"

#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <pthread.h>
#include <algorithm>

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
 
    size_t num_cores = sysconf(_SC_NPROCESSORS_ONLN);

    vector<Worker*> workers;
    for (unsigned int i = 0; i < num_cores; i++) {
        workers.push_back(new Worker(1));
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
