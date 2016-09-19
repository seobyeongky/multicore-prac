#ifndef __PROJECT1_SRC_CONTEXT_H__
#define __PROJECT1_SRC_CONTEXT_H__

#include <vector>
#include <array>
#include <iterator>
#include <assert.h>

template<std::size_t N>
struct DistInfo {
    std::array<int, N> map;
};

typedef std::vector<std::vector<int>> AdjList;
typedef std::vector<DistInfo<64>> DistMap;

struct Context
{
    int num_vertices;
    std::vector<int> houses;
    std::vector<bool> house_bitmap;
    AdjList adjlist;
    DistMap dist_map; // dist_map[i][j] = minimum distance of house_j at v_i.
};

#endif // __PROJECT1_SRC_CONTEXT_H__
