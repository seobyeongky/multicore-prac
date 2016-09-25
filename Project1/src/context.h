#ifndef __PROJECT1_SRC_CONTEXT_H__
#define __PROJECT1_SRC_CONTEXT_H__

#include "config.h"

#include <vector>
#include <array>
#include <iterator>
#include <assert.h>

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
    AdjList adjlist;
};

#endif // __PROJECT1_SRC_CONTEXT_H__
