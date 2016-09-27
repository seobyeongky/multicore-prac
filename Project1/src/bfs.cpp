#include "bfs.h"
#include "context.h"
#include "util.h"

#include <pthread.h>
#include <limits.h>
#include <algorithm>
#include <vector>
#include <queue>
#include <utility>

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
            int neighbor;
            for (int * ptr = &g_context.adjlist[v].neighbors[0];
                    (neighbor = *ptr) != END_OF_ARR;
                    ++ptr) {
                if (!check_map[neighbor]) {
                    next_vertices->push_back(neighbor);
                    check_map[neighbor] = 1;
                    if (g_context.house_bitmap[neighbor]) {
                        result.Update(dist + 1);
                        int house_index = g_context.house_index_map[neighbor];
                        pthread_mutex_lock(&g_context.visit_bitmap_mutex);
                        visit_bits |= g_pow2[house_index];
                        g_context.visit_bitmap[house_index] |= g_pow2[start_house_index];
                        pthread_mutex_unlock(&g_context.visit_bitmap_mutex);
                    }
                }
            }
        }
        cur_vertices->clear();
    }
#ifdef DBG_PRINT
    printf("house %d result %d %d\n", start_house, result.min_dist, result.max_dist);
    printf("  dist : %d\n", dist);
#endif

    return result;
}

BFSResult BFSMultiThread(int start_house, int num_threads) {
    BFSResult result;

    return result;
}
