/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	alloc.c
 * Purpose:	Defines sal routines for memory allocation
 */

#ifdef LINUX_SAL_DMA_ALLOC_OVERRIDE
/* Keep local functions, but use a different name */
#define sal_dma_alloc sal_sim_dma_alloc
#define sal_dma_free sal_sim_dma_free
#endif

#include <sys/types.h>
#ifdef PTRS_ARE_64BITS
#include <unistd.h>
#include <sys/mman.h>
#endif
#include <stdlib.h>
#include <assert.h>

#include <sal/types.h>
#include <sal/core/thread.h>
#include <sal/core/memlog.h>
#include <sal/core/libc.h>

#ifdef MEMORY_MEASUREMENT_DIAGNOSTICS
#include <shared/mem_measure_tool.h>
#endif

#ifndef USE_EXTERNAL_MEM_CHECKING
/* { */
#define USE_EXTERNAL_MEM_CHECKING 0
/* } */
#endif
#if USE_EXTERNAL_MEM_CHECKING
/* { */
#define EXT_DEBUG_ALLOC(_sz) do { return malloc(_sz); } while(0)
#define EXT_DEBUG_FREE(_addr) do { free(_addr); return; } while(0)
/* } */
#else
/* { */
#define EXT_DEBUG_ALLOC(_sz)
#define EXT_DEBUG_FREE(_addr)
/* } */
#endif

#ifndef AGGRESSIVE_ALLOC_DEBUG_TESTING
/* { */
#define AGGRESSIVE_ALLOC_DEBUG_TESTING 0
/* } */
#endif

#if AGGRESSIVE_ALLOC_DEBUG_TESTING
/* { */
#include "alloc_debug.h"


static alloc_info Allocs[MAX_ALLOCS];
static int nof_allocs = 0;
static pthread_mutex_t alloc_test_mutex;
static int _test_ininted = 0;

/*
 * Base entry (first entry) to scan in global array 'Allocas' every time
 * an allocation is required.
 * See 'sal_alloc_debug_alloc()'.
 */
static uint32 Aggressive_debug_alloc_base = 0;
static uint32 Aggressive_long_free_search = AGGRESSIVE_LONG_FREE_SEARCH;
static uint32 Aggressive_alloc_debug_testing_keep_order = AGGRESSIVE_ALLOC_DEBUG_TESTING_KEEP_ORDER;
static uint32 Aggressive_num_to_test = AGGRESSIVE_NUM_TO_TEST;

#define AGGR_DEBUG_ALLOC(_p, _sz, _s) sal_alloc_debug_alloc(_p, _sz, _s)
#define AGGR_DEBUG_FREE(_addr) sal_alloc_debug_free(_addr)
#define AGGR_DEBUG_PRINT_BAD_ADDR(_addr) sal_alloc_debug_print_bad_addr(_addr)
/*
 * Below is a list of pairs of procedures (get/set) for the following control
 * parameters (all related to implementation of AGGRESSIVE_ALLOC_DEBUG_TESTING)
 *   aggressive_long_free_search
 *   aggressive_alloc_debug_testing_keep_order
 *   aggressive_num_to_test
 * 'Get' procedures have no input and return the required parameter.
 * 'Set' procedures have the parameter as input and no return value.
 * {
 */
void
set_aggressive_long_free_search(uint32 value)
{
    if (value != 0)
    {
        Aggressive_long_free_search = 1;
    }
    else
    {
        Aggressive_long_free_search = 0;
    }
}
uint32
get_aggressive_long_free_search(void)
{
    return (Aggressive_long_free_search);
}

void
set_aggressive_alloc_debug_testing_keep_order(uint32 value)
{
    if (value != 0)
    {
        Aggressive_alloc_debug_testing_keep_order = 1;
    }
    else
    {
        Aggressive_alloc_debug_testing_keep_order = 0;
    }
}
uint32
get_aggressive_alloc_debug_testing_keep_order(void)
{
    return (Aggressive_alloc_debug_testing_keep_order);
}

void
set_aggressive_num_to_test(uint32 value)
{
    if (value == 0)
    {
        Aggressive_num_to_test = AGGRESSIVE_NUM_TO_TEST;
    }
    else
    {
        Aggressive_num_to_test = value;
    }
}
uint32
get_aggressive_num_to_test(void)
{
    return (Aggressive_num_to_test);
}

static void
enter_mutex(void)
{
    if (!_test_ininted) {
        _test_ininted = 1;
        pthread_mutex_init(&alloc_test_mutex, NULL);
    }
    pthread_mutex_lock(&alloc_test_mutex);
}
/* print all the allocations that were not de-alloced in the given size range */
void print_sal_allocs(int min_size, int max_size)
{
  alloc_info *p_a = Allocs;
  int i, j=0,k;

  enter_mutex();
  for (i = nof_allocs; i; --i, ++p_a) {
    if (p_a->size >= min_size && p_a->size <= max_size) {
      printf("allocation %d 0x%lx %d  from %p", j++, (unsigned long)p_a->addr, p_a->size, p_a->stack[ALLOC_DEBUG_TESTING_NOF_BACKTRACES-1]);
      for (k = 0; k < ALLOC_DEBUG_TESTING_NOF_BACKTRACES - 1; ++k) {
        printf(" -> %p", p_a->stack[ALLOC_DEBUG_TESTING_NOF_BACKTRACES - 2 - k]);
      }
      printf(" Description : %s\n", p_a->desc);
    }
  }
  pthread_mutex_unlock(&alloc_test_mutex);
}

int sal_alloc_debug_nof_allocs_get(void)
{
    int nof_allocs_lcl;
    enter_mutex();
    nof_allocs_lcl = nof_allocs;
    pthread_mutex_unlock(&alloc_test_mutex);
    return nof_allocs_lcl;
}

void sal_alloc_debug_last_allocs_get(alloc_info *array, int nof_last_allocs)
{
    uint32 i;
    int start = 0;

    enter_mutex();
    if(array) {
        if (nof_allocs > nof_last_allocs) {
            start = nof_allocs - nof_last_allocs;
        }
        for(i = start; i < nof_allocs; ++i) {
            *(array++) = Allocs[i];
        }
    }
    pthread_mutex_unlock(&alloc_test_mutex);
}
/*
 * }
 */
void
sal_alloc_debug_alloc(uint32 *p, unsigned int sz, const char* alloc_note)
{
    {
        size_t addr = (size_t)(p+2);
        alloc_info *p_a;
        int ii, kk;
        int alloc_test_base;
        int nof_cycles;

        enter_mutex();
        if (Aggressive_debug_alloc_base >= nof_allocs)
        {
            Aggressive_debug_alloc_base = 0;
        }
        alloc_test_base = Aggressive_debug_alloc_base;
        if ((Aggressive_debug_alloc_base + get_aggressive_num_to_test()) >= nof_allocs)
        {
            nof_cycles = nof_allocs - Aggressive_debug_alloc_base;
        }
        else
        {
            nof_cycles = get_aggressive_num_to_test();
        }
        Aggressive_debug_alloc_base += get_aggressive_num_to_test();

        p_a = &Allocs[alloc_test_base];
        for (ii = nof_cycles; ii ; --ii, ++p_a)
        {
            uint32 *a = (uint32*)(p_a->addr);

            if (a[-1] != 0xaaaaaaaa)
            {
                printf("Wrong header 0x%08lX: Allocation address 0x%08lX, size 0x%lX ",
                       (unsigned long)a[-1], (unsigned long)addr, (unsigned long)(p_a->size));
                for (kk = ALLOC_DEBUG_TESTING_NOF_BACKTRACES - 1; kk > 0; --kk)
                {
                    printf("%p -> ", p_a->stack[kk]);
                }
                printf("%p\n", p_a->stack[0]);
                assert(0);
            }
            assert(a[-2] == p_a->size / 4  && a[a[-2]] == 0xbbbbbbbb);
            if (p_a->addr < addr + sz && addr < p_a->addr + p_a->size) {
                printf("Allocation 0x%lx %d conflicts with previous allocation 0x%lx %d  at ",
                       (unsigned long)addr, sz,
                       (unsigned long)p_a->addr, p_a->size);
                for (kk = ALLOC_DEBUG_TESTING_NOF_BACKTRACES - 1; kk > 0; --kk)
                {
                    printf("%p -> ", p_a->stack[kk]);
                }
                printf("%p\n", p_a->stack[0]);
                assert(0);
            }
        }

        if (nof_allocs >= MAX_ALLOCS) {
            printf ("too many allocations, the testing mechanism will not work properly from now\n");
            assert(0);
        } else {
            void *stack[ALLOC_DEBUG_TESTING_NOF_BACKTRACES + 1];

            sal_memset(stack,0,sizeof(stack));
            backtrace(stack, ALLOC_DEBUG_TESTING_NOF_BACKTRACES + 1);
            Allocs[nof_allocs].addr = addr;
            Allocs[nof_allocs].size = sz;
            Allocs[nof_allocs].desc = alloc_note;
            for (kk = 0; kk < ALLOC_DEBUG_TESTING_NOF_BACKTRACES; ++kk) {
                Allocs[nof_allocs].stack[kk] = stack[kk + 1];
            }
            ++nof_allocs;
        }
        pthread_mutex_unlock(&alloc_test_mutex);
    }
}

void
sal_alloc_debug_free(void *addr)
{
    {
        size_t iaddr = (size_t)addr;
        alloc_info *p_a;
        int ii, kk, found = -1;
        uint32 *a;

        a = (uint32 *)addr;
        assert(a[-1] == 0xaaaaaaaa && a[a[-2]] == 0xbbbbbbbb);

        enter_mutex();
        p_a = &Allocs[nof_allocs - 1];
        for (ii = 0; ii < nof_allocs; ++ii, --p_a) {
            a = (uint32*)(p_a->addr);
            assert(a[-1] == 0xaaaaaaaa && a[-2] == p_a->size / 4  && a[a[-2]] == 0xbbbbbbbb);
            if (iaddr == p_a->addr) {
                /* This is alloc matches the free */
                if (found != -1) {
                    printf("When freeing %p found two matching allocs: 0x%lx %d at ",
                           addr,
                           (unsigned long)Allocs[found].addr, Allocs[found].size);
                    for (kk = ALLOC_DEBUG_TESTING_NOF_BACKTRACES - 1; kk > 0; --kk) {
                        printf("%p -> ", Allocs[found].stack[kk]);
                    }
                    printf("%p and 0x%lx %d at ", Allocs[found].stack[0],
                           (unsigned long)p_a->addr, p_a->size);
                    for (kk = ALLOC_DEBUG_TESTING_NOF_BACKTRACES - 1; kk > 0; --kk) {
                        printf("%p -> ", p_a->stack[kk]);
                    }
                    printf("%p\n", p_a->stack[0]);
                    assert(0);
                } else {
                    found = nof_allocs - 1 - ii;
                    if (get_aggressive_long_free_search() == 0)
                    {
                        break;
                    }
                }
            }
            else if (iaddr >= p_a->addr && iaddr < p_a->addr + p_a->size)
            {
                /* Found an alloc that the address is inside of */
                printf("When freeing 0x%lxi found an alloc 0x%lx %d at ",
                       (unsigned long)iaddr, (unsigned long)p_a->addr, p_a->size);
                for (kk = ALLOC_DEBUG_TESTING_NOF_BACKTRACES - 1; kk > 0; --kk) {
                    printf("%p -> ", p_a->stack[kk]);
                }
                printf("%p that conflicts with the free\n", p_a->stack[0]);
                assert(0);
            }
        }
        if (found == -1)
        {
            printf("Did not find a matching allocation of %p\n", addr);
            assert(0);
        } else {
            if (get_aggressive_alloc_debug_testing_keep_order())
            {
                for(; found + 1 < nof_allocs; ++found)
                {
                    Allocs[found] = Allocs[found + 1];
                }
            }
            else
            {
                p_a = &Allocs[nof_allocs - 1];
                Allocs[found] = *p_a;
            }
            --nof_allocs;
        }
        pthread_mutex_unlock(&alloc_test_mutex);
    }
}

void
sal_alloc_debug_print_bad_addr(void *addr)
{
    printf("sal_free: BAD FREE %p\n", addr);
}
/* } */
#else
/* { */
#define AGGR_DEBUG_ALLOC(_p, _sz, _s)
#define AGGR_DEBUG_FREE(_addr)
#define AGGR_DEBUG_PRINT_BAD_ADDR(addr)
/* } */
#endif /* AGGRESSIVE_ALLOC_DEBUG_TESTING */

/* To do: use real data segment limits for bad pointer detection */
#define BAD_PTR(p)						\
        (PTR_TO_UINTPTR(p) < 0x1000UL ||			\
         PTR_TO_UINTPTR(p) > ~0xfffUL)
#define CORRUPT(p)						\
	(p[-1] != 0xaaaaaaaa ||					\
	 p[p[-2]] != 0xbbbbbbbb)
#define FREED_PTR(p)    ((p[-1] == 0xcccccccc) && (p[p[-2]] == 0xdddddddd))

static unsigned long	sal_alloc_bytes;
static unsigned long	sal_alloc_calls;
static unsigned long	sal_free_bytes;
static unsigned long	sal_free_calls;

static unsigned long    sal_alloc_bytes_main_thr;
static unsigned long    sal_free_bytes_main_thr;

#ifdef BROADCOM_DEBUG
/* { */
#ifdef INCLUDE_BCM_SAL_PROFILE
/* { */
static unsigned int _sal_alloc_max = 0 ;
static unsigned int _sal_alloc_curr = 0 ;

#define SAL_ALLOC_RESOURCE_USAGE_INCR(a_curr, a_max, a_size, ilock)     \
        a_curr += (a_size);                                             \
        a_max = ((a_curr) > (a_max)) ? (a_curr) : (a_max)

#define SAL_ALLOC_RESOURCE_USAGE_DECR(a_curr, a_size, ilock)            \
        a_curr -= (a_size)


/*
 * Function:
 *      sal_alloc_resource_usage_get
 * Purpose:
 *      Provides Current/Maximum memory allocation.
 * Parameters:
 *      alloc_curr - Current memory usage.
 *      alloc_max - Memory usage high water mark
 */

void 
sal_alloc_resource_usage_get(uint32 *alloc_curr, uint32 *alloc_max)
{
    if (alloc_curr != NULL) {
        *alloc_curr = _sal_alloc_curr;
    }
    if (alloc_max != NULL) {
        *alloc_max = _sal_alloc_max;
    }
}
static unsigned int _sal_dma_alloc_max;
static unsigned int _sal_dma_alloc_curr;

#define SAL_DMA_ALLOC_RESOURCE_USAGE_INCR(a_curr, a_max, a_size, ilock) \
        a_curr += (a_size);                                             \
        a_max = ((a_curr) > (a_max)) ? (a_curr) : (a_max)

#define SAL_DMA_ALLOC_RESOURCE_USAGE_DECR(a_curr, a_size, ilock)        \
        a_curr -= (a_size)

/*
 * Function:
 *      sal_dma_alloc_resource_usage_get
 * Purpose:
 *      Provides Current/Maximum memory allocation.
 * Parameters:
 *      alloc_curr - Current memory usage.
 *      alloc_max - Memory usage high water mark
 */

void 
sal_dma_alloc_resource_usage_get(uint32 *alloc_curr, uint32 *alloc_max)
{
    if (alloc_curr != NULL)
    {
        *alloc_curr = _sal_dma_alloc_curr;
    }
    if (alloc_max != NULL)
    {
        *alloc_max = _sal_dma_alloc_max;
    }
}
/* } */
#endif
/*
 * Function:
 *      sal_alloc_stat
 * Purpose:
 *      Dump the current allocations
 * Parameters:
 *      param - integer used to change behavior.
 * Notes:
 *      If the parameter is non-zero, the routine will not
 *      display successive entries with the same description.
 */
void
sal_alloc_stat(void *param)
{
}
/* } */
#endif /* BROADCOM_DEBUG */

/**
 * \brief
 *     Get the main thread allocated and freed bytes counters.
 * \param [out] alloc_bytes_count -
 *     Pointer to alloc bytes counter storage.
 * \param [out] free_bytes_count -
 *     Pointer to free bytes counter storage.
 * \return
 *     (void)
 * \remark
 * * None
 * \see
 * * None
 */
void sal_get_alloc_counters_main_thr(unsigned long *alloc_bytes_count,unsigned long *free_bytes_count)
{
    *alloc_bytes_count = sal_alloc_bytes_main_thr;
    *free_bytes_count = sal_free_bytes_main_thr;
}

/**
 * \brief
 *     Offsets the main thread allocated and freed bytes counters
 * \param [out] alloc_bytes_count_offset -
 *     Value that is substracted from the alloc bytes counter.
 * \param [out] free_bytes_count_offset -
 *     Value that is substracted from the free bytes counter.
 * \return
 *     (void)
 * \remark
 * * Used by sw state internals to offset the sal_alloc and sal_free counters
 * \see
 * * dnxc_sw_state_alloc_plain
 */
void sal_set_alloc_counters_offset_main_thr(unsigned long alloc_bytes_count_offset, unsigned long free_bytes_count_offset)
{
    sal_alloc_bytes_main_thr -= alloc_bytes_count_offset;
    sal_free_bytes_main_thr -= free_bytes_count_offset;
}

/**
 * \brief
 *     Get the allocated and freed bytes counters.
 *     Pointer to alloc bytes counter storage.
 * \param [out] free_bytes_count -
 *     Pointer to free bytes counter storage.
 * \return
 *     (void)
 * \remark
 * * None
 * \see
 * * None
 */
void sal_get_alloc_counters(unsigned long *alloc_bytes_count,unsigned long *free_bytes_count)
{
    *alloc_bytes_count = sal_alloc_bytes;
    *free_bytes_count = sal_free_bytes;
}

/**
 * \brief
 *     Offsets the  allocated and freed bytes counters.
 * \param [out] alloc_bytes_count_offset -
 *     Value that is substracted from the alloc bytes counter.
 * \param [out] free_bytes_count_offset -
 *     Value that is substracted from the free bytes counter.
 * \return
 *     (void)
 * \remark
 * * Used by sw state internals to offset the sal_alloc and sal_free counters
 * \see
 * * dnxc_sw_state_alloc_plain
 */
void sal_set_alloc_counters_offset(unsigned long alloc_bytes_count_offset, unsigned long free_bytes_count_offset)
{
    sal_alloc_bytes -= alloc_bytes_count_offset;
    sal_free_bytes -= free_bytes_count_offset;
}

/*
 * Function:
 *	sal_alloc
 * Purpose:
 *	Allocate general purpose system memory.
 * Parameters:
 *	sz - size of memory block to allocate.
 *	s - optional user description of memory block for debugging.
 * Returns:
 *	Pointer to memory block
 * Notes:
 *	Memory allocated by this routine is not guaranteed to be safe
 *	for hardware DMA read/write.
 */

void *
sal_alloc(unsigned int sz, char *s)
{
    unsigned int orig_sz, alloc_sz;
    uint32	*p;

#ifdef MEMORY_MEASUREMENT_DIAGNOSTICS
    uint32 idx;
#endif
#ifdef INCLUDE_TCL
/* { */
    /*
     * Protects against negative sizes. Does happen with intentionally
     * illegal input (via TCL)
     */
    if ((int)sz < 0)
    {
        return NULL;
    }
/* } */
#endif
    EXT_DEBUG_ALLOC(sz);

    /*
     * Round up size to accommodate corruption detection sentinels.
     * Place sentinels at the beginning and end of the data area to
     * detect memory corruption.  These are verified on free.
     */

    orig_sz = sz;

    sz = (sz + 3) & ~3;

    /* Check for wrap caused by bad input */
    alloc_sz = sz + 12;
    if (alloc_sz < orig_sz) {
        return NULL;
    }

    sal_alloc_calls += 1;

    if ((p = malloc(alloc_sz)) == 0) {
        return p;
    }

    assert(UINTPTR_TO_PTR(PTR_TO_UINTPTR(p)) == p);

    if (sal_thread_self() == sal_thread_main_get())
    {
        sal_alloc_bytes_main_thr += sz;
    }

    sal_alloc_bytes += sz;

    p[0] = sz / 4;
    p[1] = 0xaaaaaaaa;
    p[2 + sz / 4] = 0xbbbbbbbb;

#ifdef MEMORY_MEASUREMENT_DIAGNOSTICS
    MEMORY_MEASUREMENT_INITIALIZE;
    for(idx = 0;idx < memory_measurement_tool.count;idx++) {
        if(memory_measurement_tool.elements[idx].is_active && (memory_measurement_tool.elements[idx].thread_id == sal_thread_self())) {
            memory_measurement_tool.elements[idx].sal_size += sz;
        }
    }
#endif

#ifdef BROADCOM_DEBUG
/* { */
#ifdef INCLUDE_BCM_SAL_PROFILE
/* { */
    SAL_ALLOC_RESOURCE_USAGE_INCR(
        _sal_alloc_curr,
        _sal_alloc_max,
        (sz),
        ilock);

/* } */
#endif
/* } */
#endif /* BROADCOM_DEBUG */

    AGGR_DEBUG_ALLOC(p, sz, s);

    MEMLOG_ALLOC("sal_alloc", (void *)&p[0], orig_sz, s);

    return (void *) &p[2];
}

/*
 * Function:
 *	sal_free
 * Purpose:
 *	Free memory block allocate by sal_alloc
 * Parameters:
 *	addr - Address returned by sal_alloc
 */

void 
sal_free(void *addr)
{
    uint32	*p	= (uint32 *)addr;
    uint32  *ap = p; /* Originally Allocated Pointer */

#ifdef MEMORY_MEASUREMENT_DIAGNOSTICS
    uint32 idx;
#endif

    EXT_DEBUG_FREE(addr);

    /*
     * Verify sentinels on free.  If this assertion fails, it means that
     * memory corruption was detected.
     */

#ifdef SAL_FREE_NULL_IGNORE
/* { */
    if (addr == NULL)
    {
        return;
    }
/* } */
#endif

    AGGR_DEBUG_FREE(addr);

    if (BAD_PTR(p) || FREED_PTR(p) || CORRUPT(p)) {
        AGGR_DEBUG_PRINT_BAD_ADDR(addr);
        assert(!BAD_PTR(p));	/* Use macro to beautify assert message */
        assert(!FREED_PTR(p));  /* Detect double freed memory */
        assert(!CORRUPT(p));	/* Use macro to beautify assert message */
    }

    /* Adjust for Corruption detection Sentinels */
    ap--;
    ap--;

    if (sal_thread_self() == sal_thread_main_get())
    {
        sal_free_bytes_main_thr += ap[0] * 4;
    }

    sal_free_calls += 1;
    sal_free_bytes += ap[0] * 4;

#ifdef MEMORY_MEASUREMENT_DIAGNOSTICS
    for(idx = 0;idx < memory_measurement_tool.count;idx++) {
        if(memory_measurement_tool.elements[idx].is_active && (memory_measurement_tool.elements[idx].thread_id == sal_thread_self())) {
            memory_measurement_tool.elements[idx].sal_size -= (ap[0] * 4);
        }
    }
#endif

#ifdef BROADCOM_DEBUG
/* { */
#ifdef INCLUDE_BCM_SAL_PROFILE
/* { */
    SAL_ALLOC_RESOURCE_USAGE_DECR(
        _sal_alloc_curr,
        (ap[0] * 4),
        ilock);

/* } */
#endif
/* } */
#endif /* BROADCOM_DEBUG */

    MEMLOG_FREE("sal_free", (void *)&p[-2]);

    ap[1] = 0xcccccccc;    /* Detect redundant frees */
    ap[2 + ap[0]] = 0xdddddddd;
    /*    coverity[address_free : FALSE]    */
    free(ap);
}

/*
 * Function:
 *	sal_dma_alloc
 * Purpose:
 *	Allocate memory that can be DMA'd into/out of.
 * Parameters:
 *	sz - number of bytes to allocate
 *	s - string associated with allocate
 * Returns:
 *	Pointer to allocated memory or NULL if out of memory.
 * Notes:
 *	Memory allocated by this routine is not guaranteed to be safe
 *	for hardware DMA read/write. This is for use only on sim platform.
 */

void *
sal_dma_alloc(size_t sz, char *s)
{
    uint32	*p;
    size_t  new_sz;
#if defined(PTRS_ARE_64BITS) && defined(MAP_32BIT)
    long pagesz = sysconf(_SC_PAGESIZE);
#endif

    /*
     * Round up size to accommodate corruption detection sentinels.
     * Place sentinels at the beginning and end of the data area to
     * detect memory corruption.  These are verified on free.
     */
    sz = (sz + 3) & ~3;

    /* Add bytes for holding sentinals */
    new_sz = sz + 12;

#if defined(PTRS_ARE_64BITS) && defined(MAP_32BIT)
    /* For 64 bit SDK simulation image use mmap with MAP_32BIT flag for
     * allocation as we need allocated region to be addressable in 32bit
     * address space. Normal malloc would return memory in 64 bit virtual
     * address space. Also allign the size to a page boundary as mmap
     * allocates memory in pages
     */
    new_sz = (new_sz + (size_t)(pagesz-1)) & ~(pagesz-1);
    p = mmap(NULL,
             new_sz,
             PROT_READ|PROT_WRITE,
             MAP_ANONYMOUS|MAP_SHARED|MAP_32BIT,
             -1,
             0);
    if (MAP_FAILED == p) {
        return NULL;
    }
#else
    if ((p = malloc(new_sz)) == 0) {
        return p;
    }
#endif

    assert(INT_TO_PTR(PTR_TO_INT(p)) == p);

    p[0] = sz / 4;
    p[1] = 0xaaaaaaaa;
    p[2 + sz / 4] = 0xbbbbbbbb;
#ifdef BROADCOM_DEBUG
/* { */
#ifdef INCLUDE_BCM_SAL_PROFILE
/* { */
    SAL_DMA_ALLOC_RESOURCE_USAGE_INCR(
        _sal_dma_alloc_curr,
        _sal_dma_alloc_max,
        (sz),
        ilock);
/* } */
#endif
/* } */
#endif /* BROADCOM_DEBUG */

    MEMLOG_ALLOC("sal_dma_alloc", &p[0], new_sz, s);

    return (void *) &p[2];
}

/*
 * Function:
 *	sal_dma_free
 * Purpose:
 *	Free memory allocated by sal_dma_alloc
 * Parameters:
 *	addr - pointer to memory to free.
 * Returns:
 *	Nothing.
 */

void
sal_dma_free(void *addr)
{
#if defined(PTRS_ARE_64BITS) && defined(MAP_32BIT)
    int org_sz;
    long pagesz = sysconf(_SC_PAGESIZE);
#endif
    uint32	*p	= (uint32 *)addr;
    uint32  *ap = p; /* Originally Allocated Pointer */

    /*
     * Verify sentinels on free.  If this assertion fails, it means that
     * memory corruption was detected.
     */

    /*    coverity[conditional (1): FALSE]    */
    /*    coverity[conditional (2): FALSE]    */
    assert(!BAD_PTR(p));	/* Use macro to beautify assert message */
    /* Detect double freed memory */
    assert(!FREED_PTR(p));
    /*    coverity[conditional (3): FALSE]    */
    /*    coverity[conditional (4): FALSE]    */
    assert(!CORRUPT(p));	/* Use macro to beautify assert message */


    /* Adjust for Corruption detection Sentinels */
    ap--;
    ap--;

#ifdef BROADCOM_DEBUG
/* { */
#ifdef INCLUDE_BCM_SAL_PROFILE
/* { */
    SAL_DMA_ALLOC_RESOURCE_USAGE_DECR(
        _sal_dma_alloc_curr,
        (ap[0] * 4),
        ilock);
/* } */
#endif
/* } */
#endif /* BROADCOM_DEBUG */

    MEMLOG_FREE("sal_dma_free", ap);

    ap[1] = 0xcccccccc;  /* Detect redundant frees */
    ap[2 + ap[0]] = 0xdddddddd;
#if defined(PTRS_ARE_64BITS) && defined(MAP_32BIT)
    /* For 64 bit SDK simulation image use  munmap as the allocation
     * was done using mmap. Also re-adjust the size such that it is
     * page alligned.
     */
    org_sz =  (ap[0] * 4) + 12; /* Add 12 for 3 sentinels */
    org_sz = (org_sz + (size_t)(pagesz-1)) & ~(pagesz-1);
    munmap(ap, org_sz);
#else
    /*    coverity[address_free : FALSE]    */
    free(ap);
#endif
}

/*
 * Function:
 *	sal_dma_flush
 * Purpose:
 *	Ensure modified cache is written out to memory.
 * Parameters:
 *	addr - beginning of address region
 *	len - size of address region
 * Notes:
 *	A region of memory should always be flushed before telling
 *	hardware to start a DMA read from that memory.
 */

void
sal_dma_flush(void *addr, int len)
{
    COMPILER_REFERENCE(addr);
    COMPILER_REFERENCE(len);
}

/*
 * Function:
 *	sal_dma_inval
 * Purpose:
 *	Ensure cache memory is discarded and not written out to memory.
 * Parameters:
 *	addr - beginning of address region
 *	len - size of address region
 * Notes:
 *	A region of memory should always be invalidated before telling
 *	hardware to start a DMA write into that memory.
 */

void
sal_dma_inval(void *addr, int len)
{
    COMPILER_REFERENCE(addr);
    COMPILER_REFERENCE(len);
}

/*
 * Function:
 *	sal_dma_vtop
 * Purpose:
 *	Convert a virtual memory address to physical.
 * Parameters:
 *	addr - address to convert
 * Returns:
 *	Physical address
 */

void *
sal_dma_vtop(void *addr)
{
    return addr;
}

/*
 * Function:
 *	sal_dma_ptov
 * Purpose:
 *	Convert a physical memory address to virtual.
 * Parameters:
 *	addr - address to convert
 * Returns:
 *	Virtual address
 */

void *
sal_dma_ptov(void *addr)
{
    return addr;
}

/*
 * Function:
 *	sal_memory_check
 * Purpose:
 *	Check an address in memory for existence and writability.
 * Parameters:
 *	addr - address to check
 * Returns:
 *	0 if writable, 1 if not
 * Notes:
 *	Should be written to catch SIGBUS and SIGSEGV.
 *	For now, just returns success.
 */

int
sal_memory_check(uint32 addr)
{
    return 0;
}
