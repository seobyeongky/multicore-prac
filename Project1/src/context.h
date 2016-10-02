/*
 * Definition of main problem solving global context
 *
 * @author Byeongky Seo
 * @since 2016-09-16
 */


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

// Adjacency information of a vertex
template <typename T, size_t N>
struct AdjInfo {
    std::array<T, N> neighbors;
    int size;

    AdjInfo()
        : neighbors()
        , size(0) {
    }

    void PushBack(int vertex) {
        neighbors[size++] = vertex;
        // neighbors[size] = END_OF_ARR;
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

// This is a main problom solving global context
struct Context
{
    int num_vertices;

    std::vector<int> houses;
    
    // house_bitmap[v] = whether v is a house or not(just crossroad).
    std::vector<char> house_bitmap; 

    // house_index_map[v] = index of house(v) = (0, 1, 2, ... houses.size() - 1)
    std::map<int, int> house_index_map; 

    // Adjacency List
    AdjList adjlist;

    // when visit_bitmap[i] = y than, y & (1 << j) is
    //   visit check bit between house(index i) and house(index j)
    std::array<_64Bit, MAX_HOUSE> visit_bitmap;

    // mutex of visit_bitmap
    pthread_mutex_t visit_bitmap_mutex;
};

#endif // __PROJECT1_SRC_CONTEXT_H__
