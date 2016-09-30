#include "bfs.h"
#include "context.h"
#include "util.h"

#include <pthread.h>
#include <limits.h>
#include <algorithm>
#include <vector>
#include <queue>
#include <utility>
#include <array>
#include <list>

using namespace std;

extern Context g_context;

BFSResult::BFSResult()
    : min_dist(INT_MAX)
      , max_dist(0) {
}

void BFSResult::Update(BFSResult rhs) {
    min_dist = min(min_dist, rhs.min_dist);
    max_dist = max(max_dist, rhs.max_dist);
}

void BFSResult::Update(int dist) {
    min_dist = min(min_dist, dist);
    max_dist = max(max_dist, dist);
}

#define VisitVertexM(neighbor) if (!check_map[neighbor]) { \
    next_vertices->push_back(neighbor); \
    check_map[neighbor] = 1; \
        if (g_context.house_bitmap[neighbor]) { \
            result.Update(dist + 1); \
            int house_index = g_context.house_index_map[neighbor]; \
            pthread_mutex_lock(&g_context.visit_bitmap_mutex); \
            visit_bits |= g_pow2[house_index]; \
            g_context.visit_bitmap[house_index] |= g_pow2[start_house_index]; \
            pthread_mutex_unlock(&g_context.visit_bitmap_mutex); \
        } \
    }


BFSResult BFSSingleThread(int start_house) {
    BFSResult result;
    vector<char> check_map(g_context.num_vertices + 1, 0);
    vector<int> buf1, buf2;
    vector<int> *cur_vertices = &buf1;
    vector<int> *next_vertices = &buf2;
    int start_house_index = g_context.house_index_map[start_house];
    _64Bit & visit_bits = g_context.visit_bitmap[start_house_index];
    _64Bit full_visit_bits = 0;

    for (int i = 0; i < (int)g_context.houses.size(); i++) {
        if (i == start_house_index) {
            continue;
        }
        full_visit_bits |= g_pow2[i];
    }

    next_vertices->push_back(start_house);
    check_map[start_house] = 1;

    int dist;
    for (dist = 0;
            visit_bits != full_visit_bits && next_vertices->size() > 0;
            dist++) {
        swap(cur_vertices, next_vertices);
        for (int v : *cur_vertices) {
            //for (int * ptr = &g_context.adjlist[v].neighbors[0];
            //        (neighbor = *ptr) != END_OF_ARR;
            //        ++ptr) {
            auto & adj_info = g_context.adjlist[v];
            switch (adj_info.size) {
                case 10:
                    VisitVertexM(adj_info.neighbors[9]);
                case 9:
                    VisitVertexM(adj_info.neighbors[8]);
                case 8:
                    VisitVertexM(adj_info.neighbors[7]);
                case 7:
                    VisitVertexM(adj_info.neighbors[6]);
                case 6:
                    VisitVertexM(adj_info.neighbors[5]);
                case 5:
                    VisitVertexM(adj_info.neighbors[4]);
                case 4:
                    VisitVertexM(adj_info.neighbors[3]);
                case 3:
                    VisitVertexM(adj_info.neighbors[2]);
                case 2:
                    VisitVertexM(adj_info.neighbors[1]);
                case 1:
                    VisitVertexM(adj_info.neighbors[0]);
                case 0:
                    break;
            }
            //}
        }
        cur_vertices->clear();
    }
#ifdef DBG_PRINT
    printf("house %d result %d %d\n", start_house, result.min_dist, result.max_dist);
    printf("  dist : %d\n", dist);
#endif

    return result;
}


class BFSMultiThreadContext {
public:
    BFSMultiThreadContext(int start_house, int num_threads)
        : start_house_(start_house)
        , start_house_index_(g_context.house_index_map[start_house_])
        , num_free_threads_(num_threads - 1)
        , check_map_(g_context.num_vertices + 1, 0)
        , visit_bits_(g_context.visit_bitmap[start_house_index_])
        , full_visit_bits_(0)
        , result_()
        , result_mutex_(PTHREAD_MUTEX_INITIALIZER)
        , worker_done_bits_buf_({0, 0})
        , worker_done_bits_mutex_(PTHREAD_MUTEX_INITIALIZER)
        , worker_done_full_bits_buf_({0, 0})
        , worker_making_mutex_(PTHREAD_MUTEX_INITIALIZER)
        , done_bits_flag_buf_({0, 0}) {
            for (int i = 0; i < (int)g_context.houses.size(); i++) {
            if (i == start_house_index_) {
                continue;
            }
            full_visit_bits_ |= g_pow2[i];
        }
    }

    BFSResult Run() {
        check_map_[start_house_] = 1;

        for (int i = 0; i < MAX_BFS_MULTI_THREAD; i++) {
            free_worker_ids.push_back(i);
        }

        worker_done_full_bits_buf_[0] = 1;
        BeginWorkAt(start_house_, 0, alloc_worker_id());

        return result_;
    }

private:
    int start_house_;
    int start_house_index_;
    int num_free_threads_;
    vector<char> check_map_;
    _64Bit & visit_bits_;
    _64Bit full_visit_bits_;
    BFSResult result_;
    pthread_mutex_t result_mutex_;
    array<_64Bit, 2> worker_done_bits_buf_;
    pthread_mutex_t worker_done_bits_mutex_;
    array<_64Bit, 2> worker_done_full_bits_buf_;
    int worker_counter_;
    pthread_mutex_t worker_making_mutex_;
    array<char, 2> done_bits_flag_buf_;
    list<int> free_worker_ids;

    struct WorkParam {
        BFSMultiThreadContext * self;
        int start_vertex;
        int dist;
        int worker_id;

        WorkParam()
            : self(), start_vertex(), dist(), worker_id() {}

        WorkParam(BFSMultiThreadContext * self, int start_vertex
                , int dist, int worker_id)
            : self(self), start_vertex(start_vertex), dist(dist)
            , worker_id(worker_id) {}
    };

    typedef pair<WorkParam, pthread_t> ChildWorker;

    int alloc_worker_id() {
        int free_id = free_worker_ids.front();
        free_worker_ids.pop_front();
        return free_id;
    }

    void free_worker_id(int worker_id) {
        free_worker_ids.push_front(worker_id);
    }

    static void *BeginWorkAtStatic(void *param) {
        WorkParam * run_param = (WorkParam *)param;
        (run_param->self)->BeginWorkAt(run_param->start_vertex
                , run_param->dist, run_param->worker_id);
        return 0;
    }
    
    void BeginWorkAt(int start_vertex, int dist, int worker_id) {
        vector<int> buf1, buf2;
        vector<int> *cur_vertices = &buf1;
        vector<int> *next_vertices = &buf2;
        array<ChildWorker, MAX_CHILD_RUN_THREADS> child_workers;
        int num_child_workers = 0;
        //printf("worker(%d) : sapwned... start_vertex(%d) dist(%d)\n", worker_id, start_vertex, dist);

        next_vertices->push_back(start_vertex);

        for (;
                visit_bits_ != full_visit_bits_ && next_vertices->size() > 0;
                dist++) {
            swap(cur_vertices, next_vertices);
            //printf("worker(%d) : begin dist %d\n", worker_id, dist);

            _64Bit & worker_done_bits = worker_done_bits_buf_[dist % 2];
            _64Bit & next_worker_done_bits = worker_done_bits_buf_[1 - dist % 2];
            _64Bit & worker_done_full_bits = worker_done_full_bits_buf_[dist % 2];
            _64Bit & next_worker_done_full_bits = worker_done_full_bits_buf_[1 - dist % 2];
            char & done_bits_flag = done_bits_flag_buf_[dist % 2];
            char & next_done_bits_flag = done_bits_flag_buf_[1 - dist % 2];

            while (num_free_threads_ > 0 && cur_vertices->size() > 1) {
                pthread_mutex_lock(&worker_making_mutex_);
                if (num_free_threads_ <= 0) {
                    pthread_mutex_unlock(&worker_making_mutex_);
                    break;
                }
                --num_free_threads_;
                int last_vertex = cur_vertices->back();
                cur_vertices->pop_back();
                ChildWorker * child_worker = &child_workers[num_child_workers];
                int child_worker_id = alloc_worker_id();
                //printf("worker(%d) : make child worker(%d) free(%d) vsize(%d)\n"
                //        , worker_id, child_worker_id, num_free_threads_, cur_vertices->size());
                child_worker->first = WorkParam(this, last_vertex, dist, child_worker_id);
                worker_done_full_bits |= g_pow2[child_worker_id];
                //printf("worker(%d) : change worker_done_full_bits(%x) to %x\n"
                //        , worker_id, &worker_done_full_bits, worker_done_full_bits);
                pthread_create(&child_worker->second
                        , 0, BeginWorkAtStatic, &child_worker->first);
                num_child_workers++;
                pthread_mutex_unlock(&worker_making_mutex_);
            }

            for (int v : *cur_vertices) {
                VisitNeighborsOf(v, dist, next_vertices);
            }

            cur_vertices->clear();
            pthread_mutex_lock(&worker_done_bits_mutex_);
            worker_done_bits |= g_pow2[worker_id];
            //printf("worker(%d) : done... done_bits | %x = %x\n", worker_id, g_pow2[worker_id], worker_done_bits);
            pthread_mutex_unlock(&worker_done_bits_mutex_);
            int dbg_count = 0;
            //vector<_64Bit> bitbit;
            __sync_synchronize();
            while ((worker_done_bits & worker_done_full_bits) != worker_done_full_bits) {
                // bitbit.push_back(worker_done_bits);
            __sync_synchronize();
                /*if ((dbg_count++ % 100000) == 0 && false) {
                    printf("worker(%d) : waiting...!! %llx %llx full_bits(%x)\n"
                            , worker_id, worker_done_bits, worker_done_full_bits
                            , &worker_done_full_bits);
                }*/
            }
            //for (_64Bit x : bitbit) {
                //printf("worker(%d) : waited (%llx)\n", worker_id, x);
            //}
            //printf("worker(%d) : passed\n", worker_id);
            
            pthread_mutex_lock(&worker_making_mutex_);
            if (!done_bits_flag) {
                next_worker_done_bits = 0;
                next_worker_done_full_bits = worker_done_full_bits;
                done_bits_flag = 1;
                next_done_bits_flag = 0;
                //printf("worker(%d) : clear next big flag\n", worker_id);
            }
            pthread_mutex_unlock(&worker_making_mutex_);
        }

        pthread_mutex_lock(&worker_making_mutex_);
        {
            _64Bit & worker_done_full_bits = worker_done_full_bits_buf_[dist % 2];
            _64Bit & next_worker_done_full_bits = worker_done_full_bits_buf_[1 - dist % 2];
            ++num_free_threads_;
            _64Bit remover = 0xffffffffffffffff - g_pow2[worker_id];
            worker_done_full_bits &= remover;
            next_worker_done_full_bits &= remover;
            free_worker_id(worker_id);
        }
        pthread_mutex_unlock(&worker_making_mutex_);
        //printf("worker(%d) : goodbye ...\n", worker_id);
        for (int i = 0; i < num_child_workers; i++) {
            ChildWorker & child_worker = child_workers[i];
            pthread_join(child_worker.second, (void **)nullptr);
        }
    }

    inline void VisitVertex(int v, int dist, vector<int> * next_vertices) {
        // printf(" ptr : %x size(%d) \n", this, check_map_.size());
        if (check_map_[v]) {
            return;
        }
        check_map_[v] = 1;
        next_vertices->push_back(v);

        // TODO: How about using emplace
        if (g_context.house_bitmap[v]) {
            pthread_mutex_lock(&result_mutex_);
            result_.Update(dist + 1);
            // printf(" update : %d ... %d\n", dist + 1, v);
            pthread_mutex_unlock(&result_mutex_);
            int house_index = g_context.house_index_map[v];
            pthread_mutex_lock(&g_context.visit_bitmap_mutex);
            visit_bits_ |= g_pow2[house_index];
            g_context.visit_bitmap[house_index] |= g_pow2[start_house_index_];
            pthread_mutex_unlock(&g_context.visit_bitmap_mutex);
        }
    }

    inline void VisitNeighborsOf(int v, int dist, vector<int> * next_vertices) {
        auto & adj_info = g_context.adjlist[v];
        switch (adj_info.size) {
            case 10:
                VisitVertex(adj_info.neighbors[9], dist, next_vertices);
            case 9:
                VisitVertex(adj_info.neighbors[8], dist, next_vertices);
            case 8:
                VisitVertex(adj_info.neighbors[7], dist, next_vertices);
            case 7:
                VisitVertex(adj_info.neighbors[6], dist, next_vertices);
            case 6:
                VisitVertex(adj_info.neighbors[5], dist, next_vertices);
            case 5:
                VisitVertex(adj_info.neighbors[4], dist, next_vertices);
            case 4:
                VisitVertex(adj_info.neighbors[3], dist, next_vertices);
            case 3:
                VisitVertex(adj_info.neighbors[2], dist, next_vertices);
            case 2:
                VisitVertex(adj_info.neighbors[1], dist, next_vertices);
            case 1:
                VisitVertex(adj_info.neighbors[0], dist, next_vertices);
            case 0:
                break;
        }
    }
};


BFSResult BFSMultiThread(int start_house, int num_threads) {
    auto res = BFSMultiThreadContext(start_house, num_threads).Run();
    //printf("*** DONE ***\n");
    return res;
}
