/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        rx_pool.c
 * Purpose:     Receive packet pool code
 * Requires:
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/higig.h>
#include <soc/property.h>

#include <bcm_int/common/rx.h>
#include <bcm_int/control.h>
#include <bcm/rx.h>
#include <bcm/stack.h>


/****************************************************************
 *
 * RX POOL CODE:
 *
 *     Default allocation and free routines
 *
 * This code sets up a very simple pool of packet buffers for RX.
 * The buffer pool holds a fixed number of fixed size packets.
 * If the pool is exhausted, the allocation returns NULL.
 */

static uint8 *rxp_all_bufs;      /* Allocation pointer */
#if defined(BCM_RXP_DEBUG)
static uint8 *rxp_end_bufs;      /* Points to first byte after all bufs */
#endif
static uint8 *rxp_free_list;     /* Free list pointer */
static int rxp_pkt_size;         /* Base packet size */
static int rxp_pkt_count;        /* How many packets in pool */

static sal_mutex_t rxp_mutex;

int rxp_memory_source_heap = 0; /* Mem source is not heap to begin-with */
static void *bcm_rx_pool_memory_alloc(unsigned int size, char *locus);
static void bcm_rx_pool_memory_free(void *rx_pool_addr);

#ifdef BCM_RX_REFILL_FROM_INTR
/* Must be interrupt-safe in this mode */
#define RXP_LOCK       RX_INTR_LOCK
#define RXP_UNLOCK     RX_INTR_UNLOCK
#else
#define RXP_LOCK       sal_mutex_take(rxp_mutex, sal_mutex_FOREVER)
#define RXP_UNLOCK     sal_mutex_give(rxp_mutex)
#endif

/*
 * Define buffer alignment in physical memory.
 *
 * This is a system-specific parameter, but it is currently not possible
 * to retrieve this information through the SAL or BDE interfaces.
 */
#ifndef BCM_CACHE_LINE_BYTES
#define BCM_CACHE_LINE_BYTES 128 /* Should be fine on most platforms */
#endif
#define RXP_ALIGN(_a) \
    ((_a + (BCM_CACHE_LINE_BYTES - 1)) & ~(BCM_CACHE_LINE_BYTES - 1))

#define RXP_INIT                                                        \
    if (rxp_mutex == NULL &&                                            \
            (rxp_mutex = sal_mutex_create("rx_pool")) == NULL)          \
        return BCM_E_MEMORY

/* Define function interfaces to RX pool memory allocator

   External functions must provide the same interface as
   sal_dma_alloc() and sal_dma_free().

   BCM_RX_POOL_MEM_ALLOC() is called once during bcm_rx_pool_setup()
   to allocate RX buffer memory.

   BCM_RX_POOL_MEM_FREE() is called once during bcm_rx_pool_cleanup()
   to release allocated RX buffer memory if memory was allocated. Once
   allocated memory is freed, BCM_RX_POOL_MEM_FREE() will not be
   called again until after the next call to bcm_rx_pool_setup().

*/
#ifndef BCM_RX_POOL_MEM_ALLOC
#define BCM_RX_POOL_MEM_ALLOC bcm_rx_pool_memory_alloc
#endif

#ifndef BCM_RX_POOL_MEM_FREE
#define BCM_RX_POOL_MEM_FREE bcm_rx_pool_memory_free
#endif

/* Any byte in a buffer may be used to reference that buffer when freeing */

/* Buffer index; use any byte in buffer to get proper index */
#define RXP_BUF_IDX(buf)  \
    ( ((buf) - rxp_all_bufs) / rxp_pkt_size )

/*
 * Get the start/end addresses of a buffer given the index of the pkt
 */

#define RXP_BUF_START(idx)  \
    (rxp_all_bufs + ((idx) * rxp_pkt_size))

/*
 * Get the start address of a buffer given a pointer to any byte in the
 * buffer.
 */

#define RXP_PTR_TO_START(ptr) RXP_BUF_START(RXP_BUF_IDX(ptr))

/*
 * When buffers are not allocated, the first word is used as a pointer
 * for linked lists.  Requires alignment of the buffers.
 */
#define RXP_PKT_NEXT(buf) ( ((uint8 **)(RXP_PTR_TO_START(buf)))[0])

#if defined(BCM_RXP_DEBUG)

/*
 * Does "val" point into the buffer space at all?
 */

#define RXP_IN_BUFFER_SPACE(val) \
    (((val) < rxp_end_bufs) && ((val) >= rxp_all_bufs))

/*
 * Does "val" point to the start of a buffer?
 */

#define RXP_IS_BUF_START(val) \
    (RXP_IN_BUFFER_SPACE(val) &&   \
     (((val) - rxp_all_bufs) % rxp_pkt_size == 0))

/*
 * Keep pointer to "owner" for each packet
 *
 * Buffers are filled with 0xee (empty) when on free list;
 * Filled with 0xaa when allocated
 */
typedef struct rx_pool_debug_s
{
    uint32  buf_size;
    uint32  total_bufs;
    int     curr_alloc_good_cnt;
    int     curr_alloc_fail_cnt;
    int     curr_free_cnt;
    int     tot_in_use_cnt;
    uint64  tot_alloc_good_cnt;
    uint64  tot_alloc_fail_cnt;
    uint64  tot_free_cnt;
} rx_pool_debug_t;

static rx_pool_debug_t rxp_dbg_info;

typedef struct {
    int state;
    const char *owner;
    int alloc;
    int free;
} RXP_TRACK_t;

static RXP_TRACK_t *rxp_tracker;

static int bcm_rx_pool_free_buf_verify(void *buf);
static int bcm_rx_pool_free_verify_p(void);

#endif /* BCM_RXP_DEBUG */

static int rxp_curr_in_use_cnt;

/*
 * Function:
 *      bcm_rx_pool_memory_alloc
 * Purpose:
 *      Allocate memory for RX Pool
 * Parameters:
 *      size   - Number of bytes to be allocated
 *      locus  - User defined memory description
 * Returns:
 *      Valid Address, if allocation successful else NULL.
 * Notes:
 *      The function allocates the memory from heap if directed
 *      so based on the global variable rxp_memory_source_heap.
 */
static void *
bcm_rx_pool_memory_alloc(unsigned int size, char *locus)
{
    return (rxp_memory_source_heap ?
                sal_alloc(size, locus) : sal_dma_alloc(size, locus));
}

/*
 * Function:
 *      bcm_rx_pool_memory_free
 * Purpose:
 *      Deallocate given memory from RX Pool
 * Parameters:
 *      ptr   - Address of the RX pool
 * Returns:
 *      None
 * Notes:
 *      1) Function assumes given address is valid RX pool buffer address
 *      2) It releases memory to heap instead of DMA pool if directed so
 */
static void
bcm_rx_pool_memory_free(void *rx_pool_addr)
{
    rxp_memory_source_heap ?
        sal_free(rx_pool_addr) : sal_dma_free(rx_pool_addr);
}


/*
 * Function:
 *      bcm_rx_pool_setup
 * Purpose:
 *      Setup the RX packet pool
 * Parameters:
 *      pkt_count      - How many pkts in pool.  < 0 ==> default
 *      bytes_per_pkt  - Packet allocation size
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Force alignment of the words so the pointers are safe
 *
 *      The pool cannot be setup up twice without calling cleanup
 *      to deallocate pointers.
 */

int
bcm_rx_pool_setup(int pkt_count, int bytes_per_pkt)
{
    uint8 *buf, *next_buf;
    int i;

    RXP_INIT;

    if (rxp_all_bufs != NULL) {
        return BCM_E_BUSY;
    }

    if (pkt_count < 0) {
        pkt_count = BCM_RX_POOL_COUNT_DEFAULT;
    }
    if (bytes_per_pkt < 0) {
        bytes_per_pkt = BCM_RX_POOL_BYTES_DEFAULT;
    } else {
        /* Add 14 dwords worth extra space for HG and other Module headers */
        bytes_per_pkt += 56;
    }

    /* Force packet alignment */
    bytes_per_pkt = RXP_ALIGN(bytes_per_pkt);
    rxp_pkt_size = bytes_per_pkt;
    rxp_pkt_count = pkt_count;
    rxp_all_bufs = BCM_RX_POOL_MEM_ALLOC(pkt_count * rxp_pkt_size, "bcm_rx_pool");
    if (rxp_all_bufs == NULL) {
        return BCM_E_MEMORY;
    }
    /* Set to all 0xee for RXP debug */
    sal_memset(rxp_all_bufs, 0xee, rxp_pkt_count * rxp_pkt_size);

    RXP_LOCK;
    buf = rxp_all_bufs;
    rxp_free_list = buf;
    for (i = 0; i < pkt_count - 1; i++) {
        next_buf = buf + rxp_pkt_size;
        RXP_PKT_NEXT(buf) = next_buf;
        buf = next_buf;
    }

    RXP_PKT_NEXT(buf) = NULL;

#if defined(BCM_RXP_DEBUG)
    {
        int bytes;
        int idx;
        bytes = sizeof(RXP_TRACK_t) * pkt_count;
        rxp_tracker = sal_alloc(bytes, "rx_pool_dbg");
        for (idx = 0; idx < pkt_count; idx++) {
            rxp_tracker[idx].owner = "";
            rxp_tracker[idx].state = 0;
            rxp_tracker[idx].alloc = 0;
            rxp_tracker[idx].free = 0;
        }
        rxp_dbg_info.curr_alloc_good_cnt = 0;
        rxp_dbg_info.curr_free_cnt = 0;
        rxp_dbg_info.curr_alloc_fail_cnt = 0;
        rxp_dbg_info.total_bufs = pkt_count;
        rxp_dbg_info.buf_size = bytes_per_pkt;
        rxp_end_bufs = rxp_all_bufs + rxp_pkt_count * rxp_pkt_size;
    }
#endif
    rxp_curr_in_use_cnt = 0;
    RXP_UNLOCK;

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_rx_pool_setup_done
 * Purpose:
 *      Indicate if setup is complete
 * Returns:
 *      Boolean:  True if setup is done
 */

int
bcm_rx_pool_setup_done(void)
{
    return rxp_all_bufs != NULL;
}


/*
 * Function:
 *      bcm_rx_pool_cleanup
 * Purpose:
 *      Deallocate the rx buffer pool
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      No checking is done to see if pointers are outstanding.
 */

int
bcm_rx_pool_cleanup(void)
{
    if (rxp_all_bufs == NULL) {
        return BCM_E_NONE;
    }

    RXP_LOCK;

#ifndef BCM_SAND_SUPPORT
    /* If any pkt data block is still in use then return BUSY err code */
    if (rxp_curr_in_use_cnt) {
        RXP_UNLOCK;
        return BCM_E_BUSY;
    }
#endif

    BCM_RX_POOL_MEM_FREE(rxp_all_bufs);

    rxp_all_bufs = NULL;
    rxp_free_list = NULL;

#if defined(BCM_RXP_DEBUG)
    sal_free(rxp_tracker);
    rxp_tracker = NULL;
#endif
    RXP_UNLOCK;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_rx_pool_alloc
 * Purpose:
 *      Allocate a buffer from a fixed pool configured
 *      by bcm_rx_pool_setup.
 * Parameters:
 *      unit          - Ignored
 *      size          - Size of buffer to allocate
 *      flags         - Ignored
 * Returns:
 *      Pointer to buffer if successful, NULL if failure.
 * Notes:
 *      Can fail for:
 *
 *      1.  One size fits all....if size > how the list was set up,
 *          then the function fails.
 *      2.  Not initialized
 *      3.  No free buffers
 *
 *      The first word of the buffer is used as a "next" pointer when
 *      the buffer is in the free list.
 */

int
bcm_rx_pool_alloc(int unit, int size, uint32 flags, void **pool)
{
    uint8 *rv;

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(flags);

    if (rxp_mutex == NULL) {
        *pool = NULL;
        return BCM_E_TIMEOUT;
    }

    if (rxp_pkt_size > 0) {
        size = rxp_pkt_size;
    }

    if (size > rxp_pkt_size) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "bcm_rx_pool_alloc: %d > %d\n"),
                   size, rxp_pkt_size));
        *pool = NULL;
        return BCM_E_MEMORY;
    }

    RXP_LOCK;
    if (rxp_free_list == NULL) {
#if defined(BCM_RXP_DEBUG)
        ++rxp_dbg_info.curr_alloc_fail_cnt;
        COMPILER_64_ADD_32(rxp_dbg_info.tot_alloc_fail_cnt, 1);
#endif
        RXP_UNLOCK;
        *pool = NULL;
        return BCM_E_MEMORY;
    }
    rv = rxp_free_list;
    rxp_free_list = RXP_PKT_NEXT(rxp_free_list);

#if defined(BCM_RXP_DEBUG)
    {
        int idx;
        static const char *track_name = "rx_pool_alloc";

        assert(RXP_IS_BUF_START(rv));
        if (rxp_free_list != NULL) {
            assert(RXP_IS_BUF_START(rxp_free_list));
        }

        idx = RXP_BUF_IDX(rv);
        rxp_tracker[idx].owner = track_name;
        rxp_tracker[idx].state = 1;
        rxp_tracker[idx].alloc++;
        if (bcm_rx_pool_free_buf_verify(rv) != BCM_E_NONE) {
            assert(0);
        }

        /* Fill allocated buffer with 0xaa */
        sal_memset(rv, 0xaa, rxp_pkt_size);
        ++rxp_dbg_info.curr_alloc_good_cnt;
        rxp_dbg_info.tot_in_use_cnt++;
        COMPILER_64_ADD_32(rxp_dbg_info.tot_alloc_good_cnt, 1);
    }
#endif /* BCM_RXP_DEBUG */
    ++rxp_curr_in_use_cnt;

    RXP_UNLOCK;

    *pool = (void *)rv;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_rx_pool_free
 * Purpose:
 *      Return a buffer allocated by bcm_rx_pool_alloc to the free list
 * Parameters:
 *      unit       - Ignored
 *      buf        - The buffer to free
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_rx_pool_free(int unit, void *buf)
{
    uint8 *start;
    uint8 *buf8;

    COMPILER_REFERENCE(unit);

    RXP_LOCK;

    if (rxp_all_bufs == NULL) { /* not running */
        RXP_UNLOCK;
        return BCM_E_MEMORY;
    }

    buf8 = (uint8 *)buf;
    start = RXP_PTR_TO_START(buf8);

#if defined(BCM_RXP_DEBUG)
    {
        int idx, i;
        uint32 *ptr;

        COMPILER_REFERENCE(ptr);
        COMPILER_REFERENCE(i);
        idx = RXP_BUF_IDX(buf8);
        assert((idx >= 0 && idx < rxp_pkt_count));
        rxp_tracker[idx].free++;
        if (rxp_tracker[idx].state == 0) {
            /* Buffers should never be freed more than once, but
               distinguish the error between a freed buffer that gets
               reused vs. one that doesn't. */
            LOG_WARN(BSL_LS_BCM_COMMON,
                     (BSL_META_U(unit,
                                 "Double free of buffer %d\n"),
                      idx));
            if (bcm_rx_pool_free_buf_verify(buf) != BCM_E_NONE) {
                assert(0);
            }
            assert(0);
        }
        rxp_tracker[idx].state = 0;
        /* Fill buffer with 0xee, except for first word */
        sal_memset(RXP_BUF_START(idx) + sizeof(void *), 0xee,
                   rxp_pkt_size - sizeof(void *));
#if 1 /* Turn this on to verify the entire free list */
        if (bcm_rx_pool_free_verify_p() != BCM_E_NONE) {
            LOG_WARN(BSL_LS_BCM_COMMON,
                     (BSL_META_U(unit,
                                 "RX Pool error while checking %d\n"),
                      idx));
            assert(0);
        }
#else
        /* Check head of free list for good measure */
        if (rxp_free_list != NULL) {
            if (bcm_rx_pool_free_buf_verify(rxp_free_list) != BCM_E_NONE) {
                assert(0);
            }
        }
#endif
        ++rxp_dbg_info.curr_free_cnt;
        rxp_dbg_info.tot_in_use_cnt--;
        COMPILER_64_ADD_32(rxp_dbg_info.tot_free_cnt, 1);
    }
#endif /* BCM_RXP_DEBUG */

    rxp_curr_in_use_cnt--;

    RXP_PKT_NEXT(start) = rxp_free_list;
    rxp_free_list = start;
    RXP_UNLOCK;

    return BCM_E_NONE;
}


#if defined(BCM_RXP_DEBUG)

void
bcm_rx_pool_dump(int min, int max)
{
    int i;
    uint32 *ptr;

    if (min < 0) {
        min = 0;
    }
    if (max > rxp_pkt_count) {
        max = rxp_pkt_count;
    }
    for (i = min; i < max; i++) {
        ptr = (uint32 *)RXP_BUF_START(i);
        LOG_WARN(BSL_LS_BCM_COMMON,
                 (BSL_META("Pkt %d: %p. [0] 0x%x [1] 0x%x [2] 0x%x\n"),
                  i,
                  ptr, ptr[0], ptr[1], ptr[2]));
    }
}

/* Check a buffer in the free pool */

#define FREE_STATE 0xeeeeeeee
#define ALLOC_STATE 0xaaaaaaaa

void
bcm_rx_pool_buf_dump(void *buf, int until)
{
    int i;
    uint32 *ptr;

    ptr = ((uint32 *)RXP_PTR_TO_START((uint8 *)buf)) + 1;
    for (i = 0; i < rxp_pkt_size - sizeof(void *); i += sizeof(uint32)) {
        if ((i & 31) == 0) {
            if (i > until && (*ptr == FREE_STATE || *ptr == ALLOC_STATE)) {
                break;
            }
            LOG_ERROR(BSL_LS_BCM_COMMON,
                      (BSL_META("\n%p:"),
                       ptr));
        }
        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META(" %08x"),
                   *ptr));
        ptr++;
    }
    LOG_ERROR(BSL_LS_BCM_COMMON,
              (BSL_META("\n")));
}


static int
bcm_rx_pool_free_buf_verify(void *buf)
{
    int i;
    uint32 *ptr;

    ptr = (uint32 *)(RXP_PTR_TO_START((uint8 *)buf) + sizeof (void *));
    for (i = 0; i < rxp_pkt_size - sizeof(void *); i += sizeof(uint32)) {
        if (*ptr != FREE_STATE) {
            return BCM_E_INTERNAL;
        }
        ptr++;
    }
    return BCM_E_NONE;
}

/* Check the entire free pool and return BCM_E_NONE if OK or BCM_E_INTERNAL if there
   was some sort of inconsistency detected. */
static int
bcm_rx_pool_free_verify_p(void)
{
    uint8 *buf;
    uint8 *last_buf = NULL;
    int idx;
    int rc = BCM_E_NONE;

    for (buf = rxp_free_list; buf != NULL; buf = RXP_PKT_NEXT(buf)) {
        idx = RXP_BUF_IDX(buf);
        if (!(idx >= 0 && idx < rxp_pkt_count)) {
            LOG_WARN(BSL_LS_BCM_COMMON,
                     (BSL_META("RXP BAD BUFFER %p. idx %d\n"),
                      buf, idx));
            if (last_buf != NULL) {
                LOG_WARN(BSL_LS_BCM_COMMON,
                         (BSL_META("RXP BAD BUFFER: Previous buf %p idx %ld\n"),
                          last_buf, RXP_BUF_IDX(last_buf)));
            } else {
                LOG_WARN(BSL_LS_BCM_COMMON,
                         (BSL_META("RXP BAD BUFFER: First elt of free list\n")));
            }
            if (idx >= 0 && idx < rxp_pkt_count) {
                LOG_WARN(BSL_LS_BCM_COMMON,
                         (BSL_META("RXP IDX %d out of range (0,%d)"),
                          idx, rxp_pkt_count));
            }
            return BCM_E_INTERNAL;
        }
        last_buf = buf;
        if ((rc=bcm_rx_pool_free_buf_verify(buf)) != BCM_E_NONE) {
            return rc;
        }
    }
    return rc;
}

/* API call */
void
bcm_rx_pool_free_verify(void)
{
    (void)bcm_rx_pool_free_verify_p();
}

void
bcm_rx_pool_own(void *buf, void *owner)
{
#if defined(LINUX) || defined(__KERNEL__)
    /* Need to fix this */
    return;
#else
    int idx;
    idx = RXP_BUF_IDX((uint8 *)buf);

#if 1  /* Turn this off to give warning rather than assert */
    assert((idx >= 0) && (idx < rxp_pkt_count));
    rxp_tracker[idx].owner = (char *)owner;
#else
    if ((idx < 0) || (idx >= rxp_pkt_count)) {
        LOG_WARN(BSL_LS_BCM_COMMON,
                 (BSL_META("RXP: Buffer %p (idx %d) out of range "
                  "max %d, owner %s\n"), buf, idx,
                  rxp_pkt_count, (char *)owner));
    } else {
        /*    assert((idx > 0) && (idx < rxp_pkt_count)); */
        rxp_tracker[idx].owner = (char *)owner;
    }
#endif
#endif
}

#define _ISALPHA(c) \
    ((c[0] >= 'A' && c[0] <= 'Z') || (c[0] >= 'a' && c[0] <= 'z'))

/* Check that a buffer is in use, or free and not corrupted */
static int rx_pool_buf_ok(int idx)
{
    if (rxp_tracker[idx].state == 0) {
        return bcm_rx_pool_free_buf_verify(RXP_BUF_START(idx));
    }
    return BCM_E_NONE;
}

void
bcm_rx_pool_report(int min, int max)
{
    int i, count = 0;
    const char *str;

    for (i = 0; i < rxp_pkt_count; i++) {
        if (rxp_tracker[i].state != 0) {
            count++;
        }
    }
    LOG_ERROR(BSL_LS_BCM_COMMON,
              (BSL_META(" #  s  alloc    free     owner\n")));
    LOG_ERROR(BSL_LS_BCM_COMMON,
              (BSL_META("--- -  -------- -------- ------------\n")));
    for (i = min; i < max; i++) {
        str = rxp_tracker[i].owner;
        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META("%3d %1d%s %8d %8d %p %s\n"),
                   i,
                   rxp_tracker[i].state,
                   rx_pool_buf_ok(i)? "?" : " ",
                   rxp_tracker[i].alloc,
                   rxp_tracker[i].free,
                   str,
                   _ISALPHA(str) ? str : "?"));
    }

    LOG_ERROR(BSL_LS_BCM_COMMON,
              (BSL_META("  RXPool Current Used %d. Failed-Allocs %d. "
               "Good-Allocs %d. Frees %d. PktSize %d. PktCount %d.\n"),
                rxp_curr_in_use_cnt, rxp_dbg_info.curr_alloc_fail_cnt,
                rxp_dbg_info.curr_alloc_good_cnt, rxp_dbg_info.curr_free_cnt,
                rxp_pkt_size, rxp_pkt_count));
    LOG_ERROR(BSL_LS_BCM_COMMON,
              (BSL_META("  RXPool Total BuffSize %u, BuffCnt %u, Used %d.\n"),
                rxp_dbg_info.buf_size,
                rxp_dbg_info.total_bufs,
                rxp_dbg_info.tot_in_use_cnt));
#ifdef COMPILER_HAS_LONGLONG
    LOG_ERROR(BSL_LS_BCM_COMMON,
              (BSL_META("  RXPool Total "
                "Good-Allocs %llu, Failed-Allocs %llu, Frees %llu.\n"),
                (long long unsigned) rxp_dbg_info.tot_alloc_good_cnt,
                (long long unsigned) rxp_dbg_info.tot_alloc_fail_cnt,
                (long long unsigned) rxp_dbg_info.tot_free_cnt));
#else
    LOG_ERROR(BSL_LS_BCM_COMMON,
              (BSL_META("  RXPool Total "
                "Good-Allocs 0x%x_%x, Failed-Allocs 0x%x_%x, Frees 0x%x_%x.\n"),
                COMPILER_64_HI(rxp_dbg_info.tot_alloc_good_cnt),
                COMPILER_64_LO(rxp_dbg_info.tot_alloc_good_cnt),
                COMPILER_64_HI(rxp_dbg_info.tot_alloc_fail_cnt),
                COMPILER_64_LO(rxp_dbg_info.tot_alloc_fail_cnt),
                COMPILER_64_HI(rxp_dbg_info.tot_free_cnt),
                COMPILER_64_LO(rxp_dbg_info.tot_free_cnt)));
#endif

    if (count != rxp_curr_in_use_cnt) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META("  RXP WARNING:  Alloc %d, counted %d\n"),
                   rxp_curr_in_use_cnt, count));
    }
}
#undef _ISALPHA

#endif /* BCM_RXP_DEBUG */
