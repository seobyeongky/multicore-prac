#ifndef __PROJ1_READER_H__
#define __PROJ1_READER_H__

#include "context.h"

#include <vector>

class Reader
{
public:
    void Read(Context * context);

private:
    void ReadNumbersInLine(std::vector<int> * num_list);
};

#endif // __PROJ1_READER_H__
