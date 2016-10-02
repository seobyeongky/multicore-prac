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

    /**
     *  Default constructor of BFSResult
     */
    BFSResult();

    /**
     *  Update the result with another result
     */
    void Update(BFSResult rhs);

    /**
     *  Update the result with given distance
     */
    void Update(int dist);
};

/**
 *  Run BFS with single thread
 *  @param[in]  start_house      house to begin search
 *  @return     result of search
 */
BFSResult BFSSingleThread(int start_house);

/**
 *  Run BFS with multi threads
 *  @param[in]  start_house     house to begin search
 *  @param[out] num_threads     number of allowed threads
 */
BFSResult BFSMultiThread(int start_house, int num_threads);

#endif // __PROJECT1_SRC_BFS_H__
