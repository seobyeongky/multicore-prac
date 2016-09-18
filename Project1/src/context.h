#ifndef __PROJECT1_SRC_CONTEXT_H__
#define __PROJECT1_SRC_CONTEXT_H__

#include <vector>
#include <array>
#include <iterator>

template<std::size_t N>
struct EdgeInfo {
    int size;
    std::array<int, N> neighbors;

    EdgeInfo()
        : size(0)
        , neighbors({-1, }) {
    }

    void PushBack(int neighbor) {
        neighbors[size++] = neighbor;
    }
};

typedef std::vector<EdgeInfo<3>> AdjList;
typedef std::vector<std::array<int, 64>> DistMap;

struct Context
{
    int num_vertices;
    std::vector<int> houses;
    std::vector<bool> house_bitmap;
    AdjList adjlist;
    DistMap dist_map; // dist_map[i][j] = minimum distance of house_j at v_i.
};

#endif // __PROJECT1_SRC_CONTEXT_H__
