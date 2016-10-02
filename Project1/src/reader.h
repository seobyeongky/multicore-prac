/*
 * fds
 *
 * @author Byeongky Seo
 * @since 2016-08-16
 */

#ifndef __PROJECT1_SRC_READER_H__
#define __PROJECT1_SRC_READER_H__

#include "context.h"

#include <vector>

class Reader
{
public:
    /**
     *  Read the problem data from standard input
     *  param[in]   context     context to fill with read data
     */
    void Read(Context * context);
};

#endif // __PROJECT1_SRC_READER_H__
