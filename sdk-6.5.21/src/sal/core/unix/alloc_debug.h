/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SAL_ALLOC_DEBUG_H
/* { */
#define _SAL_ALLOC_DEBUG_H

#include <sys/types.h>
#include <stdlib.h>
#include <assert.h>
#include <execinfo.h>
#include <stdio.h>
#include <pthread.h>

#include <sal/types.h>


#ifndef ALLOC_DEBUG_TESTING_NOF_BACKTRACES
/* { */
/*
 * Depth of backtrace stack stored for each required allocation.
 * The deeper we go the more information we get, per 'alloc' operation', but, also,
 * the slower the alloc operation becomes. Recommended value is '15'.
 */
#define ALLOC_DEBUG_TESTING_NOF_BACKTRACES 15
/* } */
#endif

#if ALLOC_DEBUG_TESTING_NOF_BACKTRACES < 3
#undef ALLOC_DEBUG_TESTING_NOF_BACKTRACES
#define ALLOC_DEBUG_TESTING_NOF_BACKTRACES 3
#endif

typedef struct {
    size_t addr;
    int size;
    void* stack[ALLOC_DEBUG_TESTING_NOF_BACKTRACES];
    CONST char *desc;
} alloc_info;

#define BUF_SIZE 1024
#define MAX_ALLOCS 500000

#if AGGRESSIVE_ALLOC_DEBUG_TESTING
/* { */
#if USE_EXTERNAL_MEM_CHECKING
/* { */
#error "USE_EXTERNAL_MEM_CHECKING and AGGRESSIVE_ALLOC_DEBUG_TESTING can\'t be set both."
/* } */
#endif
#ifndef AGGRESSIVE_ALLOC_DEBUG_TESTING_KEEP_ORDER
/* { */
/* The allocations stored in array.
 * When freeing allocation in the middle of the array,
 * the last entry in the array replace it.
 * if this flags is set, all the entries next to the free allocation will be pushed down,
 * and hence, the order will be kept.
 */
#define AGGRESSIVE_ALLOC_DEBUG_TESTING_KEEP_ORDER 0
/* } */
#endif

#ifndef AGGRESSIVE_NUM_TO_TEST
/* { */
/*
 * Number of allocation entries in the global array 'Allocas'
 * to scan every time an allocation is required.
 * See sal_alloc_debug_alloc()
 * This is just initial/default value and may be changed during
 * runtime. See Aggressive_num_to_test
 */
#define AGGRESSIVE_NUM_TO_TEST       200
/* } */
#endif

#ifndef AGGRESSIVE_LONG_FREE_SEARCH
/* { */
/*
 * Control on whether to keep searching Allocs[] after finding first match on 'free'.
 * If AGGRESSIVE_LONG_FREE_SEARCH is set to a zero value then, on the first
 * match of input address (on 'free'), the search is ended.
 */
#define AGGRESSIVE_LONG_FREE_SEARCH 0
/* } */
#endif

void set_aggressive_num_to_test(uint32 value);
void set_aggressive_alloc_debug_testing_keep_order(uint32 value);
void set_aggressive_long_free_search(uint32 value);
int sal_alloc_debug_nof_allocs_get(void);
void sal_alloc_debug_last_allocs_get(alloc_info *array, int nof_last_allocs);
/* } */
#endif

/* } */
#endif /* _SAL_ALLOC_DEBUG_H */
