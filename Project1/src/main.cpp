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
    // TODO: possible optimizing
    Worker(const vector<int> & houses, int num_allowed_threads)
        : thread_()
        , houses_(houses)
        , result_()
        , num_allowed_threads_(num_allowed_threads) {
    }

    void Work() {
        pthread_create(&thread_, 0, BFSThreadStatic, this);
    }

    void Join() {
        pthread_join(thread_, (void **)&result_);
    }


private:
    pthread_t thread_;
    vector<int> houses_;
    BFSResult result_;
    int num_allowed_threads_;

    void *BFSThread() {
        for (int house : houses_) {
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

    vector<Worker*> workers(min(num_cores, g_context.houses.size()), nullptr);

    return 0;
}
