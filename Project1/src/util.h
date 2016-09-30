/*
 * Utilities
 *
 * @author Byeongky Seo
 * @since 2016-08-16
 */

#ifndef __PROJECT1_SRC_UTIL_H__
#define __PROJECT1_SRC_UTIL_H__

#include "config.h"

#include <vector>

typedef unsigned long long _64Bit;

extern _64Bit g_pow2[MAX_HOUSE];
void InitUtil();

std::vector<int> & AllocIntVector();
void FreeIntVector(std::vector<int> & vec);

std::vector<char> & AllocCharVector();
void FreeCharVector(std::vector<char> & vec);

#endif // __PROJECT1_SRC_UTIL_H
