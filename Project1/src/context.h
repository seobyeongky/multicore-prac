#ifndef __PROJECT1_SRC_CONTEXT_H__
#define __PROJECT1_SRC_CONTEXT_H__

#include "config.h"
#include "util.h"

#include <vector>
#include <array>
#include <iterator>
#include <assert.h>
#include <pthread.h>
#include <map>

#define END_OF_ARR -1
template <typename T, size_t N>
struct AdjInfo {
    std::array<T, N + 1> neighbors;
    int size;

    AdjInfo()
        : neighbors({END_OF_ARR})
        , size(0) {
    }

    void PushBack(int vertex) {
        neighbors[size++] = vertex;
        neighbors[size] = END_OF_ARR;
        /*
        int * ptr = &neighbors[0];
        while ((*ptr) != -1) {
            ++ptr;
        }
        *ptr = vertex;
        *(++ptr) = -1;
        */
    }
};

typedef std::vector<AdjInfo<int, MAX_EDGES_PER_VERTEX>> AdjList;

struct Context
{
    int num_vertices;
    std::vector<int> houses;
    std::vector<char> house_bitmap;
    std::map<int, int> house_index_map;
    AdjList adjlist;
    std::array<_64Bit, MAX_HOUSE> visit_bitmap;
    pthread_mutex_t visit_bitmap_mutex;
};

#endif // __PROJECT1_SRC_CONTEXT_H__
