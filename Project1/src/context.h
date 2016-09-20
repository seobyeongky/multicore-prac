#ifndef __PROJECT1_SRC_CONTEXT_H__
#define __PROJECT1_SRC_CONTEXT_H__

#include <vector>
#include <array>
#include <iterator>
#include <assert.h>

typedef std::vector<std::vector<int>> AdjList;

struct Context
{
    int num_vertices;
    std::vector<int> houses;
    std::vector<bool> house_bitmap;
    AdjList adjlist;
};

#endif // __PROJECT1_SRC_CONTEXT_H__
