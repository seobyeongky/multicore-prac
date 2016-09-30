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
    void Read(Context * context);

private:
    void ReadNumbersInLine(std::vector<int> * num_list);
};

#endif // __PROJECT1_SRC_READER_H__
