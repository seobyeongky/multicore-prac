#define PRINT_GRAPH


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

class Worker {
public:
    Worker(int house, int num_allowed_threads)
        : thread_()
        , house_(house)
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
    int house_;
    BFSResult result_;
    int num_allowed_threads_;

    void *BFSThread() {
        if (num_allowed_threads_ == 1) {
            result_.Update(BFSSingleThread(house_));
        } else {
            result_.Update(BFSMultiThread(house_, num_allowed_threads_));
        }

        return &result_;
    }

    static void *BFSThreadStatic(void *self) {
        return ((Worker *)self)->BFSThread();
    }
};


int main() {
    Reader().Read(&g_context);
    
#ifdef PRINT_GRAPH
    printf("total %d vertices.\n", g_context.num_vertices);

    for (int i = 1; i <= g_context.num_vertices; i++) {
        printf("%d : ", i);
        for (int neighbor : g_context.adjlist[i]) {
            printf("%d ", neighbor);
        }
        printf("\n");
    }
#endif // PRINT_GRAPH
    
    size_t num_cores = sysconf(_SC_NPROCESSORS_ONLN);

    vector<Worker*> workers;
    for (unsigned int i = 0; i < g_context.houses.size(); i++) {
        workers.push_back(new Worker(g_context.houses[i], 1));
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
