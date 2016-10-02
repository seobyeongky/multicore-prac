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
/**
 *  Initiate utilities
 */
void InitUtil();

/**
 *  Allocate int vector
 *  return  Allocated vector
 */
std::vector<int> & AllocIntVector();

/**
 *  Free allocated int vector
 *  param[in]   vec     Vector to free
 */
void FreeIntVector(std::vector<int> & vec);

/**
 *  Allocate char vector
 *  return  Allocated vector
 */
std::vector<char> & AllocCharVector();

/**
 *  Free char vector
 *  param[in]   vec     Vector to free
 */
void FreeCharVector(std::vector<char> & vec);

#endif // __PROJECT1_SRC_UTIL_H
