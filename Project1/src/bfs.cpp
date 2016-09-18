#include "bfs.h"
#include "context.h"

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
    vector<bool> check_map(g_context.num_vertices + 1, false);
    vector<int> buf1, buf2;
    vector<int> *cur_vertices = &buf1;
    vector<int> *next_vertices = &buf2;
    int num_houses_to_visit_left = g_context.houses.size() - 1;

    next_vertices->push_back(start_house);
    check_map[start_house] = true;

    for (int dist = 0;
            num_houses_to_visit_left > 0 && next_vertices->size() > 0;
            dist++) {
        swap(cur_vertices, next_vertices);
        for (int v : *cur_vertices) {
            for (int neighbor : g_context.adjlist[v]) {
                if (!check_map[neighbor]) {
                    next_vertices->push_back(neighbor);
                    check_map[neighbor] = true;
                    if (g_context.house_bitmap[neighbor]) {
                        num_houses_to_visit_left--;
                        result.Update(dist + 1);
                    }
                }
            }
        }
        cur_vertices->clear();
    }

#ifdef DBG_PRINT
    printf("house %d result %d %d\n", start_house, result.min_dist, result.max_dist);
#endif

    return result;
}

BFSResult BFSMultiThread(int start_house, int num_threads) {
    BFSResult result;

    return result;
}
