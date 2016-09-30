/*
 * Utilities
 *
 * @author Byeongky Seo
 * @since 2016-08-16
 */

#include "util.h"
#include "config.h"

_64Bit g_pow2[MAX_HOUSE];

void InitUtil() {
    g_pow2[0] = 1;
    for (int i = 1; i < MAX_HOUSE; i++) {
        g_pow2[i] = g_pow2[i - 1] * 2;
    }
}
