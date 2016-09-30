/*
 * BFS library header
 *
 * @author Byeongky Seo
 * @since 2016-08-16
 */


#ifndef __PROJECT1_SRC__BFS_H__
#define __PROJECT1_SRC__BFS_H__

#define MAX_BFS_MULTI_THREAD 64

struct BFSResult {
    int min_dist;
    int max_dist;

    BFSResult();
    void Update(BFSResult rhs);
    void Update(int dist);
};

BFSResult BFSSingleThread(int start_house);
BFSResult BFSMultiThread(int start_house, int num_threads);

#endif // __PROJECT1_SRC_BFS_H__
