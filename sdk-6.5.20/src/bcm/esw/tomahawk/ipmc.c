/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ipmc.c
 * Purpose:     Tomahawk multicast replication implementation.
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(BCM_TOMAHAWK_SUPPORT) && defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/mem.h>

#include <bcm/types.h>
#include <bcm/error.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/ipmc.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/firebolt.h>

#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/l3.h>
#include <soc/tomahawk.h>
#include <soc/trident3.h>

#ifdef BCM_TOMAHAWK3_SUPPORT
#include <bcm_int/esw/tomahawk3.h>
#include <soc/tomahawk3.h>
#include <bcm_int/esw/multicast.h>
#endif
#if defined(BCM_HGPROXY_COE_SUPPORT)
#include <bcm_int/esw/xgs5.h>
#endif /* BCM_HGPROXY_COE_SUPPORT */

#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/multicast.h>
#include <bcm_int/common/multicast.h>
#endif

#if defined(BCM_FIREBOLT6_SUPPORT)
#include <bcm_int/esw/firebolt6.h>
#include <soc/firebolt6.h>
#endif

#if defined(BCM_FIREBOLT6_SUPPORT)
/* The member bitmap length in increased to handle subports. */
#define BCM_REPL_HEAD_MEMBER_MAX            512
#define BCM_REPL_HEAD_MEMBER_WORD_MAX    \
    ((BCM_REPL_HEAD_MEMBER_MAX + _SHR_PBMP_WORD_WIDTH-1) / _SHR_PBMP_WORD_WIDTH)
#else
#define BCM_REPL_HEAD_MEMBER_MAX            _SHR_PBMP_PORT_MAX
#define BCM_REPL_HEAD_MEMBER_WORD_MAX       SOC_PBMP_WORD_MAX
#endif



/* Structures for managing REPL_HEAD table resources */

typedef struct _th_repl_head_free_block_s {
    int index; /* Starting index of a free block of REPL_HEAD table entries */
    int size;  /* Number of entries in the free block */
    struct _th_repl_head_free_block_s *next; /* Pointer to next free block */
} _th_repl_head_free_block_t;

typedef struct _th_repl_head_info_s {
    _th_repl_head_free_block_t **free_list_array;
                              /* Array of lists of free blocks */
    int array_size;           /* Number of lists in the array */
} _th_repl_head_info_t;

STATIC _th_repl_head_info_t *_th_repl_head_info[BCM_MAX_NUM_UNITS];

#define REPL_HEAD_FREE_LIST(_u_, _pipe_, _index_) \
    _th_repl_head_info[_u_][_pipe_].free_list_array[_index_]

/* Structures for managing REPL_LIST table resources */

typedef struct _th_repl_list_entry_info_s {
    SHR_BITDCL *bitmap_entries_used; /* A bitmap indicating which REPL_LIST
                                        entries are used */
    int num_entries; /* Total number of entries in REPL_LIST table */
} _th_repl_list_entry_info_t;

STATIC _th_repl_list_entry_info_t \
           *_th_repl_list_entry_info[BCM_MAX_NUM_UNITS];

#define REPL_LIST_ENTRY_USED_GET(_u_, _pipe_, _i_) \
        SHR_BITGET(_th_repl_list_entry_info[_u_][_pipe_].bitmap_entries_used, _i_)
#define REPL_LIST_ENTRY_USED_SET(_u_, _pipe_, _i_) \
        SHR_BITSET(_th_repl_list_entry_info[_u_][_pipe_].bitmap_entries_used, _i_)
#define REPL_LIST_ENTRY_USED_CLR(_u_, _pipe_, _i_) \
        SHR_BITCLR(_th_repl_list_entry_info[_u_][_pipe_].bitmap_entries_used, _i_)

#define TH_IPMC_NO_SRC_CHECK_PORT      0xFF

/* Structures containing replication info */

typedef struct _th_repl_port_info_s {
    int *intf_count; /* Array of interface counts, one per replication group */
} _th_repl_port_info_t;

typedef struct _th_repl_info_s {
    int num_pipes;       /* Number of pipelines */
    int num_repl_groups; /* Number of replication groups on this device */
    uint32 num_intf;     /* Number of interfaces on this device */
    _bcm_repl_list_info_t **repl_list_info_array;
                         /* Array of linked list of replication list info */
/*    _th_repl_port_info_t *port_info[SOC_MAX_NUM_PORTS];*/
    _th_repl_port_info_t **port_info;
                         /* Per port replication info */
    int *l3_intf_next_hop_ipmc; /* Array of next hop indices, one for each
                            L3 interface that's added to an IPMC group */ 
    int *l3_intf_next_hop_trill; /* Array of next hop indices, one for each
                            L3 interface that's added to a Trill group */ 
} _th_repl_info_t;

static _th_repl_info_t *_th_repl_info[BCM_MAX_NUM_UNITS];
static int mmu_port_stride[BCM_MAX_NUM_UNITS];
#ifdef BCM_TOMAHAWK3_SUPPORT
static _bcm_repl_list_info_t *_repl_list_recycle_array[BCM_MAX_NUM_UNITS];
#endif

#define REPL_LOCK(_u_)                         \
    {                                          \
        (SOC_IS_FIREBOLT6(_u_) ?\
        (soc_mem_lock(_u_, MMU_REPL_LIST_TBL_SPLIT0m)) : \
        (soc_mem_lock(_u_, MMU_REPL_LIST_TBLm))); \
    }
#define REPL_UNLOCK(_u_)                         \
    {                                          \
        (SOC_IS_FIREBOLT6(_u_) ?\
        (soc_mem_unlock(_u_, MMU_REPL_LIST_TBL_SPLIT0m)) : \
        (soc_mem_unlock(_u_, MMU_REPL_LIST_TBLm))); \
    }

#define REPL_INIT(_u_) \
        if (_th_repl_info[_u_] == NULL) { return BCM_E_INIT; }

#define NUM_PIPES(_u_) _th_repl_info[_u_]->num_pipes

#define REPL_GROUP_ID(_u_, _id_) \
        if ((_id_ < 0) || (_id_ >= _th_repl_info[_u_]->num_repl_groups)) \
            { return BCM_E_PARAM; }

#define REPL_INTF_TOTAL(_u_) _th_repl_info[_u_]->num_intf

#define REPL_LIST_INFO(_u_, _pipe_) \
    _th_repl_info[_u_]->repl_list_info_array[_pipe_]

#define REPL_PORT_GROUP_INFO(_u_, _port_) \
    _th_repl_info[_u_]->port_info[_port_]

#define REPL_PORT_GROUP_INTF_COUNT(_u_, _port_, _group_id_) \
    _th_repl_info[_u_]->port_info[_port_]->intf_count[_group_id_]

#define REPL_L3_INTF_NEXT_HOP_IPMC(_u_, _intf_) \
    _th_repl_info[_u_]->l3_intf_next_hop_ipmc[_intf_]
#define REPL_L3_INTF_NEXT_HOP_TRILL(_u_, _intf_) \
    _th_repl_info[_u_]->l3_intf_next_hop_trill[_intf_]

#define REPL_PORT_CHECK(_u_, _port_) \
    if (!IS_PORT(_u_, _port_) && !IS_AXP_PORT(_u_, _port_)) { \
        return BCM_E_PARAM; \
    }

#define MMU_DQS_REPL_PORT_AGG_MAP_PIPE(_u_, _pipe_)           \
    (SOC_MEM_UNIQUE_ACC(_u_, MMU_DQS_REPL_PORT_AGG_MAPm)[_pipe_])

#define MMU_RQE_REPL_PORT_AGG_MAP_PIPE(_u_, _pipe_)           \
    (SOC_MEM_UNIQUE_ACC(_u_, MMU_RQE_REPL_PORT_AGG_MAPm)[_pipe_])

#define REPL_HEAD_TBL_PIPE(_u_, _pipe_)                                 \
    ((SOC_IS_TOMAHAWK3(_u_)) ? MMU_REPL_HEAD_TBLm : \
    (SOC_MEM_UNIQUE_ACC(_u_, MMU_REPL_HEAD_TBLm)[_pipe_]))

#define REPL_HEAD_TBL(_u_)  (MMU_REPL_HEAD_TBLm)

#define REPL_HEAD_TBL_SPLIT_PIPE(_u_, _split, _pipe_)                   \
    ((_split == 0) ?                                                    \
     SOC_MEM_UNIQUE_ACC(_u_, MMU_REPL_HEAD_TBL_SPLIT0m)[_pipe_] :       \
     ((_split == 1) ?                                                   \
     SOC_MEM_UNIQUE_ACC(_u_, MMU_REPL_HEAD_TBL_SPLIT1m)[_pipe_] :       \
     ((_split == 2) ?                                                   \
     SOC_MEM_UNIQUE_ACC(_u_, MMU_REPL_HEAD_TBL_SPLIT2m)[_pipe_] :       \
     SOC_MEM_UNIQUE_ACC(_u_, MMU_REPL_HEAD_TBL_SPLIT3m)[_pipe_])))

#define REPL_HEAD_TBL_SPLIT(_u_, _split)                                \
    ((_split == 0) ? MMU_REPL_HEAD_TBL_SPLIT0m : \
    ((_split == 1) ? MMU_REPL_HEAD_TBL_SPLIT1m : \
    ((_split == 2) ? MMU_REPL_HEAD_TBL_SPLIT2m : \
     MMU_REPL_HEAD_TBL_SPLIT3m)))

#define REPL_LIST_TBL_SPLIT_PIPE(_u_, _split, _pipe_)                   \
    ((_split == 0) ?                                                    \
     (SOC_MEM_UNIQUE_ACC(_u_, MMU_REPL_LIST_TBL_SPLIT0m)[_pipe_]) :     \
     (SOC_MEM_UNIQUE_ACC(_u_, MMU_REPL_LIST_TBL_SPLIT1m)[_pipe_]))

#define REPL_LIST_TBL_PIPE(_u_, _pipe_)                                 \
    (SOC_MEM_UNIQUE_ACC(_u_, MMU_REPL_LIST_TBLm)[_pipe_])

#define REPL_LIST_TBL(_u_)                                              \
        (soc_feature(_u_, soc_feature_split_2_repl_list_table) ?       \
        REPL_LIST_TBL_SPLIT_PIPE(_u_, 0, 0) : MMU_REPL_LIST_TBLm)

#define REPL_GROUP_INITIAL_COPY_COUNT_TBL_SLICE(_u_, _slice_)           \
    (SOC_IS_FIREBOLT6(unit) ? \
        (SOC_MEM_UNIQUE_ACC(_u_, MMU_REPL_HEAD_ICCm)[_slice_]) : \
        (SOC_MEM_UNIQUE_ACC(_u_, MMU_REPL_GROUP_INITIAL_COPY_COUNTm)[_slice_]))

#define REPL_GROUP_INITIAL_COPY_COUNT_TBL(_u_)                          \
    (MMU_REPL_GROUP_INITIAL_COPY_COUNTm)

#define REPL_GROUP_INFO_TBL_PIPE(_u_, _pipe_)                           \
    (SOC_MEM_UNIQUE_ACC(_u_, MMU_REPL_GROUP_INFO_TBLm)[_pipe_])

#define REPL_GROUP_INFO_TBL(_u_)    (MMU_REPL_GROUP_INFO_TBLm)

#define REPL_NH_INDEX_UNALLOCATED -1
#define REPL_NH_INDEX_L3_EGRESS_ALLOCATED -2

STATIC void _bcm_th_repl_head_info_deinit(int unit);

STATIC int
_bcm_th_repl_list_entry_alloc(int unit, int pipe, int *entry_index);

STATIC int
_bcm_th_repl_list_free(int unit, int pipe, int start_ptr);

STATIC int
_bcm_th_repl_list_entry_free(int unit, int pipe, int entry_index);

#ifdef BCM_TOMAHAWK3_SUPPORT

#define _TH3_IPMC_MIN_INTF_COUNT 4

#define _TH3_IPMC_UINT32_LEN 32
#define _TH3_NUM_NH_SPARSE_MODE 4
#define _TH3_NUM_REPL_QUEUE 9
#define _TH3_REPL_LIST_ENTRY_UINT32_TOTAL 128

static uint8 reserve_multicast_resources[BCM_MAX_NUM_UNITS] = {0};
/*
 * Function:
 *      bcm_th3_ipmc_repl_port_attach
 * Purpose:
 *      Initialize replication per-port configuration in the runtime.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - local port number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th3_ipmc_repl_port_attach(int unit, bcm_port_t port)
{
    int rv = BCM_E_NONE;
    int alloc_size;
    uint32 regval;
    soc_reg_t reg = MMU_RQE_REPL_PORT_AGG_MAPr;
    if (_th_repl_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    /* Detach the IPMC REPL in case it is already attached. */
    SOC_IF_ERROR_RETURN
        (bcm_th3_ipmc_repl_port_detach(unit, port));

    _th_repl_info[unit]->port_info[port] =
                    sal_alloc(sizeof(_th_repl_port_info_t), "repl port info");
    if (NULL == _th_repl_info[unit]->port_info[port]) {
        return BCM_E_MEMORY;
    }
    sal_memset(_th_repl_info[unit]->port_info[port], 0,
                                                sizeof(_th_repl_port_info_t));

    alloc_size = sizeof(int) * _th_repl_info[unit]->num_repl_groups;
    _th_repl_info[unit]->port_info[port]->intf_count =
                                sal_alloc(alloc_size, "repl port intf count");
    if (NULL == _th_repl_info[unit]->port_info[port]->intf_count) {
        sal_free(_th_repl_info[unit]->port_info[port]);
        return BCM_E_MEMORY;
    }
    sal_memset(_th_repl_info[unit]->port_info[port]->intf_count, 0, alloc_size);

    /* configure device port to repl aggregateId map */
    regval = 0;
    soc_reg_field_set(unit, reg, &regval, L3MC_PORT_AGG_IDf,
                            BCM_MC_PER_TRUNK_REPL_MODE(unit) ?
                                             TH3_AGG_ID_HW_INVALID : port);
    rv = soc_reg32_set(unit, reg, port, 0, regval);
    if (BCM_FAILURE(rv)) {
        (void)bcm_th3_ipmc_repl_port_detach(unit, port);
    }

    return rv;
}

/*
 * Function:
 *      bcm_th3_ipmc_repl_port_detach
 * Purpose:
 *      Clear replication per-port configuration in the runtime.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - local port number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th3_ipmc_repl_port_detach(int unit, bcm_port_t port)
{
    if (_th_repl_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (_th_repl_info[unit]->port_info[port] != NULL) {
        if (_th_repl_info[unit]->port_info[port]->intf_count != NULL) {
            sal_free(_th_repl_info[unit]->port_info[port]->intf_count);
            _th_repl_info[unit]->port_info[port]->intf_count = NULL;
        }
        sal_free(_th_repl_info[unit]->port_info[port]);
        _th_repl_info[unit]->port_info[port] = NULL;
    }

    /* Reset L3MC_PORT_AGG_IDf to HW invalid */
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, MMU_RQE_REPL_PORT_AGG_MAPr, port, 0,
                    TH3_AGG_ID_HW_INVALID));

    return BCM_E_NONE;
}

STATIC void
_bcm_th3_repl_list_recycle_candidate_deinit(int unit)
{
    _bcm_repl_list_info_t *rli_current, *rli_free;
    rli_current = _repl_list_recycle_array[unit];
    while(rli_current != NULL) {
        rli_free = rli_current;
        rli_current = rli_current->next;
        sal_free(rli_free);
    }
    _repl_list_recycle_array[unit] = NULL;
}

STATIC int
_bcm_th3_repl_list_hw_in_use_check(int unit,
                                SHR_BITDCL *entry_vec, int *in_use)
{

    int i, j, list_ptr_used;
    uint32 entry_done, list_ptr;
    mmu_repl_state_tbl_entry_t entry;
    soc_mem_t repl_q_mem[] = {MMU_REPL_STATE_TBL_ITM0m, MMU_REPL_STATE_TBL_ITM1m};
    soc_info_t *si = &SOC_INFO(unit);
    uint32 itm_map = si->itm_map;

    for(i = 0; i < NUM_ITM(unit); i++) {
        if (itm_map & (1U << i)) {
            for(j = 0; j < _TH3_NUM_REPL_QUEUE; j++) {
                list_ptr_used = 0;
                SOC_IF_ERROR_RETURN(soc_mem_read(unit, repl_q_mem[i],
                                MEM_BLOCK_ANY, j, &entry));
                entry_done = soc_MMU_REPL_STATE_TBLm_field32_get(unit, &entry,
                                        ENTRY_DONEf);
                if(entry_done) {
                    continue;
                } else {
                    list_ptr = soc_MMU_REPL_STATE_TBLm_field32_get(unit,
                                                        &entry, LIST_PTRf);
                    if(SHR_BITGET(entry_vec, list_ptr)) {
                        list_ptr_used = 1;
                    }

                }
                if(list_ptr_used) {
                    *in_use = list_ptr_used;
                    return BCM_E_NONE;
                }
            }
        }
    }
    *in_use = 0;
    return BCM_E_NONE;
}

STATIC int
_bcm_th3_repl_list_entry_vec_construct(int unit, SHR_BITDCL *entry_vec,
                                            int start_ptr)
{
    int prev_repl_entry_ptr, repl_entry_ptr;
    soc_mem_t repl_list_table;
    mmu_repl_list_tbl_entry_t repl_list_entry;

    prev_repl_entry_ptr = -1;
    repl_entry_ptr = start_ptr;
    repl_list_table = MMU_REPL_LIST_TBLm;
    /* get the entry vector based on the current candidate list */
    while (repl_entry_ptr != prev_repl_entry_ptr) {
        SHR_BITSET(entry_vec, repl_entry_ptr);
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, repl_list_table, MEM_BLOCK_ANY,
                          repl_entry_ptr, &repl_list_entry));

        prev_repl_entry_ptr = repl_entry_ptr;
        repl_entry_ptr = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                &repl_list_entry, NEXTPTRf);
    }
    return BCM_E_NONE;

}

STATIC int
_bcm_th3_repl_list_recycle(int unit, int *entry_id)
{

    _bcm_repl_list_info_t *rli_current, *rli_prev;
    int in_use;

    rli_current = _repl_list_recycle_array[unit]->next;
    rli_prev = NULL;
    while (rli_current != NULL) {

        SHR_BITDCL entry_vec[_TH3_REPL_LIST_ENTRY_UINT32_TOTAL] = {0};

        /* get the entry vector based on the current candidate list */
        BCM_IF_ERROR_RETURN
            (_bcm_th3_repl_list_entry_vec_construct(unit, entry_vec,
                                               rli_current->index));

        /* check if this list is still being used by HW */
        SOC_IF_ERROR_RETURN(_bcm_th3_repl_list_hw_in_use_check
                                (unit, entry_vec, &in_use));
        if(!in_use) {
            /* recycle list */
            SOC_IF_ERROR_RETURN(_bcm_th_repl_list_free(unit, 0,
                            rli_current->index));
            *entry_id = rli_current->index;
            if(rli_prev == NULL) {
                _repl_list_recycle_array[unit]->next = rli_current->next;
            } else {
                rli_prev->next = rli_current->next;
            }
            (_repl_list_recycle_array[unit]->list_size)--;
            sal_free(rli_current);
            return BCM_E_NONE;
        }
        rli_prev = rli_current;
        rli_current = rli_current->next;
    }
    *entry_id = -1;
    return BCM_E_RESOURCE;
}
/*
 * Function:
 *      _bcm_th3_repl_head_info_init
 * Purpose:
 *      Initialize replication head info.
 * Parameters:
 *      unit - (IN)SOC unit number.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th3_repl_head_info_init(int unit)
{
    int alloc_size;
    int repl_head_tbl_size;
    int max_array_index = 0;
    soc_mem_t repl_head_table = MMU_REPL_HEAD_TBLm;
    int num_ports = 0;


    mmu_port_stride[unit] = SOC_TH3_MMU_PORT_STRIDE;

    /* REPL_HEAD table is pipeline independent */
    alloc_size = sizeof(_th_repl_head_info_t);
    if (NULL == _th_repl_head_info[unit]) {
        _th_repl_head_info[unit] = sal_alloc(alloc_size, "repl_head_info");
        if (NULL == _th_repl_head_info[unit]) {
            _bcm_th_repl_head_info_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_th_repl_head_info[unit], 0, alloc_size);


    if (NULL == _th_repl_head_info[unit][0].free_list_array) {
        /* Each element of the array is a linked list of free blocks.
         * Array element N is a linked list of free blocks of size N.
         * When allocating a block of REPL_HEAD table entries, the max
         * number of entries needed is equal to the max number of members
         * in a replication group. This will also be the max index of the
         * array. Of course, the REPL_HEAD table may contain bigger blocks
         * of free entries. Array element 0 wil be a linked list of free
         * blocks with size greater than the max number of members in a
         * replication group.
         */
        max_array_index = soc_mem_field_length(unit,
                MMU_REPL_GROUP_INFO_TBLm, MEMBER_BMPf);
        alloc_size = (max_array_index + 1) *
            sizeof(_th_repl_head_free_block_t *);
        _th_repl_head_info[unit][0].free_list_array =
            sal_alloc(alloc_size, "repl head free list array");
        if (NULL == _th_repl_head_info[unit][0].free_list_array) {
            _bcm_th_repl_head_info_deinit(unit);
            return BCM_E_MEMORY;
        }
        _th_repl_head_info[unit][0].array_size = max_array_index + 1;
    }
    sal_memset(_th_repl_head_info[unit][0].free_list_array, 0,
            alloc_size);

    REPL_HEAD_FREE_LIST(unit, 0, 0) =
        sal_alloc(sizeof(_th_repl_head_free_block_t), "repl head free block");
    if (NULL == REPL_HEAD_FREE_LIST(unit, 0, 0)) {
        _bcm_th_repl_head_info_deinit(unit);
        return BCM_E_MEMORY;
    }

    /* Initially, there is only one free block, starting at entry 1 of
     * REPL_HEAD table and ending at the last entry. Entry 0 is reserved.
     * Entries from (soc_mem_index_max(unit,MMU_REPL_HEAD_TBLm) - 
     * count + 1) to  soc_mem_index_max(unit,MMU_REPL_HEAD_TBLm) 
     * are reserved for swapping the REPL_HEADm of a replication group 
     * when there is no resource for deleting a port 
     * from the replication group.
     */
    reserve_multicast_resources[unit] =
        soc_property_get(unit, spn_RESERVE_MULTICAST_RESOURCES, 0) ? 1 : 0;
    REPL_HEAD_FREE_LIST(unit, 0, 0)->index = 1;
    repl_head_tbl_size = 1 << soc_mem_field_length(unit,
            MMU_REPL_GROUP_INFO_TBLm, BASE_PTRf);
    if (reserve_multicast_resources[unit]) {
        num_ports =
            soc_mem_field_length(unit, MMU_REPL_GROUP_INFO_TBLm, MEMBER_BMPf);
    }
    REPL_HEAD_FREE_LIST(unit, 0, 0)->size = repl_head_tbl_size - 1 - num_ports;
    REPL_HEAD_FREE_LIST(unit, 0, 0)->next = NULL;

    /* Clear entry 0 of REPL_HEAD table */
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, repl_head_table, MEM_BLOCK_ALL, 0,
                       soc_mem_entry_null(unit, repl_head_table)));

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_repl_list_compare(int unit, int pipe, int start_ptr,
                          SHR_BITDCL *intf_vec)
{

    soc_mem_t   repl_list_table;
    uint32  hw_msb;
    uint32  hw_ls_bits[2];
    mmu_repl_list_tbl_entry_t repl_list_entry;
    int i, repl_entry_ptr, prev_repl_entry_ptr;
    SHR_BITDCL *hw_intf_vec = NULL;
    int alloc_size;
    int max_msb = -1;
    uint32 repl_mode, mode_bitmap;
    int rv = BCM_E_NONE;
    soc_field_t nh_fields[_TH3_NUM_NH_SPARSE_MODE] = {
                                NEXT_HOP_INDEX_0f, NEXT_HOP_INDEX_1f,
                                NEXT_HOP_INDEX_2f, NEXT_HOP_INDEX_3f
                               };
    repl_list_table = MMU_REPL_LIST_TBLm;
    prev_repl_entry_ptr = -1;
    repl_entry_ptr = start_ptr;

    alloc_size = SHR_BITALLOCSIZE(REPL_INTF_TOTAL(unit));
    hw_intf_vec = sal_alloc(alloc_size, "HW repl interface vector");
    if(hw_intf_vec == NULL) {
        return BCM_E_MEMORY;
    }
    if(start_ptr < 0 ) {
        rv = BCM_E_PARAM;
        goto exit;
    }
    sal_memset(hw_intf_vec, 0, alloc_size);
    /* first read out the entire repl list */
    while(repl_entry_ptr != prev_repl_entry_ptr) {
        rv = soc_mem_read(unit, repl_list_table, MEM_BLOCK_ANY,
                              repl_entry_ptr, &repl_list_entry);
        if (rv != BCM_E_NONE) {
            goto exit;
        }
        repl_mode = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                    &repl_list_entry, MODEf);
        if(repl_mode) {
            /* sparse mode */
            mode_bitmap = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                    &repl_list_entry, MODE_1_BITMAPf);
            for(i = 0; i < _TH3_NUM_NH_SPARSE_MODE; i++) {
                if(mode_bitmap & (1U << i)) {
                    uint32 temp_nh = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                                        &repl_list_entry, nh_fields[i]);
                    SHR_BITSET(hw_intf_vec, temp_nh);
                    max_msb = (max_msb > (temp_nh / 64)) ? max_msb :
                                            (temp_nh / 64);
                }
            }
        } else {
            /* dense mode */
            hw_msb = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                                        &repl_list_entry, MSB_VLANf);
            max_msb = max_msb > hw_msb ? max_msb : hw_msb;
            soc_MMU_REPL_LIST_TBLm_field_get(unit, &repl_list_entry,
                    LSB_VLAN_BMf, hw_ls_bits);
            hw_intf_vec[2 * hw_msb + 0] |= hw_ls_bits[0];
            hw_intf_vec[2 * hw_msb + 1] |= hw_ls_bits[1];
        }
        prev_repl_entry_ptr = repl_entry_ptr;
        repl_entry_ptr = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                                            &repl_list_entry, NEXTPTRf);

    }
    /* compare two lists */
    for(i = 0; i < max_msb; i++) {
        if((hw_intf_vec[2 * i] != intf_vec[2 * i])
                ||(hw_intf_vec[2 * i + 1] != intf_vec[2 * i + 1])) {
            rv = BCM_E_NOT_FOUND;
            goto exit;
        }
    }
exit:
    sal_free(hw_intf_vec);
    return rv;
}

/* return the pos of first bit 1 within an unsigned integer */
STATIC int
_bcm_th3_uint32_first_bit_get(int unit, uint32 val) {
    int i;
    for(i = 0; i < 32; i++) {
        if(val & (1U << i)) {
            return i;
        }
    }
    return -1;
}

STATIC void
_bcm_th3_nh_list_get(int unit,  mmu_repl_list_tbl_entry_t *entry, int max_count,
        uint32 *nh_list, uint32 *count)
{
    uint32 l_count = 0;
    uint32 ls_bits[2];
    uint32 msb, mode, mode_1_bm;
    int i, ls_pos;
    soc_field_t nh_fields[_TH3_NUM_NH_SPARSE_MODE] = {
                                NEXT_HOP_INDEX_0f, NEXT_HOP_INDEX_1f,
                                NEXT_HOP_INDEX_2f, NEXT_HOP_INDEX_3f
                               };
    mode = soc_MMU_REPL_LIST_TBLm_field32_get(unit, entry, MODEf);
    if(mode) {
        /* sparse mode */
        mode_1_bm = soc_MMU_REPL_LIST_TBLm_field32_get(unit, entry,
                                        MODE_1_BITMAPf);
        for(i = 0; i < _TH3_NUM_NH_SPARSE_MODE; i++) {
            if(mode_1_bm & (1U << i)) {
                nh_list[l_count++] = soc_MMU_REPL_LIST_TBLm_field32_get
                                        (unit, entry, nh_fields[i]);
            }
            if(l_count >= max_count) {
                break;
            }
        }

    } else {
        /* dense mode */
        soc_MMU_REPL_LIST_TBLm_field_get(unit, entry, LSB_VLAN_BMf, ls_bits);
        msb = soc_MMU_REPL_LIST_TBLm_field32_get(unit, entry,
                MSB_VLANf);
        for (ls_pos = 0; ls_pos < 64; ls_pos++) {
            if (ls_bits[ls_pos / 32] & (1 << (ls_pos % 32))) {

                nh_list[l_count++] = msb * 64 + ls_pos;
            }
            if(l_count >= max_count) {
                break;
            }
        }
    }
    *count = l_count;
}

STATIC void
_bcm_th3_ls_bit_pos_get(int unit, uint32 ls_bits[2], int *ls_bit_pos)
{
    *ls_bit_pos = -1;
    if (ls_bits[0] != 0) {
        *ls_bit_pos = _bcm_th3_uint32_first_bit_get(unit,
                                        ls_bits[0]);
    }
    else if (ls_bits[1] != 0) {
        *ls_bit_pos = _TH3_IPMC_UINT32_LEN
                + _bcm_th3_uint32_first_bit_get(unit,
                                            ls_bits[1]);
    }
}
/*
 * Function:
 *  _bcm_th3_repl_list_write
 * Purpose:
 *  Write the replication list contained in intf_vec into REPL_LIST table.
 *      Return the start_ptr and total interface count.
 * Parameters:
 *  unit       - StrataSwitch PCI device unit number.
 *  pipe       - Pipeline number.
 *  start_ptr  - (OUT) Replication list's start pointer to
 *	             REPL_LIST table.
 *      intf_count - (OUT) Interface count.
 *  intf_vec   - (IN) Vector of interfaces.
 * Returns:
 *  BCM_E_XXX
 */
STATIC int
_bcm_th3_repl_list_write(int unit, int pipe, int *start_ptr, int *intf_count,
                        SHR_BITDCL *intf_vec)
{
    int i;
    int remaining_count;
    int prev_repl_entry_ptr, repl_entry_ptr;
    uint32 msb_max, msb;
    uint32 ls_bits[2];
    int rv = BCM_E_NONE;
    int no_more_free_repl_entries;
    mmu_repl_list_tbl_entry_t repl_list_entry;
    soc_mem_t repl_list_table;
    int max_rem_count = 31;
    int ls_bit_pos, nh_id;
    uint32 nh_list[_TH3_IPMC_MIN_INTF_COUNT];
    uint32 prev_count = 0;
    soc_field_t nh_fields[_TH3_NUM_NH_SPARSE_MODE] = {
                                NEXT_HOP_INDEX_0f, NEXT_HOP_INDEX_1f,
                                NEXT_HOP_INDEX_2f, NEXT_HOP_INDEX_3f
                               };

    *intf_count = 0;
    for (i = 0; i < _SHR_BITDCLSIZE(REPL_INTF_TOTAL(unit)); i++) {
        *intf_count += _shr_popcount(intf_vec[i]);
    }

    if (*intf_count == 0) {
        return BCM_E_NONE;
    }
    repl_list_table = MMU_REPL_LIST_TBLm;
    pipe = 0;
    remaining_count = *intf_count;
    prev_repl_entry_ptr = -1;
    msb_max = _SHR_BITDCLSIZE(REPL_INTF_TOTAL(unit)) / 2; /* 32 -> 64 */
    for (msb = 0; msb < msb_max; msb++) {
        ls_bits[0] = intf_vec[2 * msb + 0];
        ls_bits[1] = intf_vec[2 * msb + 1];
        if ((ls_bits[0] != 0) || (ls_bits[1] != 0)) {
            rv = _bcm_th_repl_list_entry_alloc(unit, pipe, &repl_entry_ptr);
            if (rv == BCM_E_RESOURCE) {
                no_more_free_repl_entries = TRUE;
            } else if (BCM_FAILURE(rv)) {
                return rv;
            } else {
                no_more_free_repl_entries = FALSE;
            }
            if (prev_repl_entry_ptr == -1) {
                if (no_more_free_repl_entries) {
                    return BCM_E_RESOURCE;
                }
                *start_ptr = repl_entry_ptr;
            } else {
                if (no_more_free_repl_entries) {
                    /* Terminate replication list */
                    soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                            NEXTPTRf, prev_repl_entry_ptr);
                } else {
                    /* check the intf count in the previous list */
                    uint32 mode_bitmap = 0;
                    _bcm_th3_nh_list_get(unit, &repl_list_entry,
                            _TH3_IPMC_MIN_INTF_COUNT, nh_list, &prev_count);

                    if (prev_count < _TH3_IPMC_MIN_INTF_COUNT) {
                        /* convert to sparse mode  when count < 4 */
                        /* rewrite previous entry */
                        for(i = 0; i < prev_count; i++) {
                            soc_MMU_REPL_LIST_TBLm_field32_set(unit,
                                 &repl_list_entry, nh_fields[i], nh_list[i]);
                            mode_bitmap |= (1U << i);
                        }
                        /*  borrow NH ids from the current list */
                        for(i = prev_count; i < _TH3_NUM_NH_SPARSE_MODE; i++) {
                            _bcm_th3_ls_bit_pos_get(unit, ls_bits, &ls_bit_pos);
                            if(ls_bit_pos == -1) {
                                /* no more NH id in the current list */
                                break;
                            }
                            nh_id = msb * TH3_IPMC_MAX_INTF_COUNT_PER_ENTRY
                                                            + ls_bit_pos;

                            mode_bitmap |= (1U << i);
                            soc_MMU_REPL_LIST_TBLm_field32_set(unit,
                                 &repl_list_entry, nh_fields[i], nh_id);
                            /* remove borrowed NH id from current lists */
                            ls_bits[ls_bit_pos / 32] &=
                                                ~(1U << (ls_bit_pos % 32));

                        }

                        soc_MMU_REPL_LIST_TBLm_field32_set(unit,
                                 &repl_list_entry, MODE_1_BITMAPf, mode_bitmap);
                        soc_MMU_REPL_LIST_TBLm_field32_set(unit,
                                 &repl_list_entry, MODEf, 0x1);
                        /*update remaining count */
                        remaining_count = remaining_count - (i - prev_count);
                        if ( remaining_count > max_rem_count) {
                        /* Set the RMNG_REPS field to all zeroes */
                             soc_MMU_REPL_LIST_TBLm_field32_set(unit,
                                        &repl_list_entry, RMNG_REPSf, 0);
                        } else if(remaining_count > 0){
                                soc_MMU_REPL_LIST_TBLm_field32_set(unit,
                                        &repl_list_entry,
                                            RMNG_REPSf, remaining_count);
                        } else {
                            break;
                        }
                        if(i < _TH3_NUM_NH_SPARSE_MODE) {
                            /* no NH id in the current list */
                            _bcm_th_repl_list_entry_free(unit, pipe, repl_entry_ptr);
                            continue;
                        }

                    }
                    soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                            NEXTPTRf, repl_entry_ptr);

                }
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, repl_list_table, MEM_BLOCK_ALL,
                                   prev_repl_entry_ptr, &repl_list_entry));
                if (no_more_free_repl_entries) {
                    /* Free the list already written */
                    _bcm_th_repl_list_free(unit, pipe, *start_ptr);
                    return BCM_E_RESOURCE;
                }
            }
            prev_repl_entry_ptr = repl_entry_ptr;
            sal_memset(&repl_list_entry, 0, sizeof(repl_list_entry));
            soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                    MSB_VLANf, msb);
            soc_MMU_REPL_LIST_TBLm_field_set(unit, &repl_list_entry,
                    LSB_VLAN_BMf, ls_bits);

            remaining_count -= (_shr_popcount(ls_bits[0]) +
                    _shr_popcount(ls_bits[1]));
            if (remaining_count > max_rem_count) {
                /* Set the RMNG_REPS field to all zeroes */
                soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                        RMNG_REPSf, 0);
            } else if (remaining_count > 0) {
                soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                        RMNG_REPSf, remaining_count);
            } else { /* No more intefaces left to be written */
                break;
            }
        }
    }

    if (prev_repl_entry_ptr > 0) {
        /* Write final entry */
        soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry, NEXTPTRf,
                prev_repl_entry_ptr);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, repl_list_table, MEM_BLOCK_ALL,
                           prev_repl_entry_ptr, &repl_list_entry));
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_sparse_mode_egress_intf_get(int unit, int repl_entry_ptr,
                                int nh_id, bcm_if_t *if_array, int *if_count)
{
    soc_mem_t mem = MMU_REPL_LIST_TBLm;
    int entry_type;
    uint32 mode_bitmap, nh_val, l3_intf;
    mmu_repl_list_tbl_entry_t repl_list_entry;
    egr_l3_next_hop_2_entry_t egr_nh2;
    egr_l3_next_hop_entry_t egr_nh;
    soc_field_t nh_fields[4] = {NEXT_HOP_INDEX_0f, NEXT_HOP_INDEX_1f,
                                NEXT_HOP_INDEX_2f, NEXT_HOP_INDEX_3f};

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                          repl_entry_ptr, &repl_list_entry));

    mode_bitmap = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                            &repl_list_entry, MODE_1_BITMAPf);
    if(mode_bitmap & (1U << nh_id)) {
        nh_val = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                    &repl_list_entry, nh_fields[nh_id]);
        if_array[*if_count] = nh_val + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);

        SOC_IF_ERROR_RETURN(
                READ_EGR_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY,
                                nh_val, &egr_nh));
        entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                            &egr_nh, ENTRY_TYPEf);

        SOC_IF_ERROR_RETURN(
                    READ_EGR_L3_NEXT_HOP_2m(unit, MEM_BLOCK_ANY,
                                            nh_val, &egr_nh2));

        l3_intf = soc_EGR_L3_NEXT_HOP_2m_field32_get(unit,
                        &egr_nh2, INTF_NUMf);
        if(entry_type == 0) {
            if (REPL_L3_INTF_NEXT_HOP_TRILL(unit, l3_intf) ==
                                nh_val) {
                    if_array[*if_count] = l3_intf;
            }
        } else if (entry_type == 3) {

            if (REPL_L3_INTF_NEXT_HOP_IPMC(unit, l3_intf) == nh_val) {
                if_array[*if_count] = l3_intf;
            }

        }
        (*if_count)++;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_ipmc_egress_intf_add_for_trunk(int unit, int repl_group,
    bcm_trunk_t tgid, int encap_id, int is_l3)
{
#if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
    bcm_port_t      trunk_local_member_ports[SOC_MAX_NUM_PP_PORTS] = {0};
    int             max_num_ports = SOC_MAX_NUM_PP_PORTS;
#else
    bcm_port_t      trunk_local_member_ports[SOC_MAX_NUM_PORTS] = {0};
    int             max_num_ports = SOC_MAX_NUM_PORTS;
#endif

    int idx;
    int member_count, port_iter;
    int aggid;
    int old_intf_count, new_intf_count, diff_count = 0;
    BCM_IF_ERROR_RETURN(
        _bcm_esw_trunk_local_members_get(unit, tgid, max_num_ports,
                                         trunk_local_member_ports,
                                         &member_count));

    for (idx = 0; idx < member_count; idx++) {
        port_iter = trunk_local_member_ports[idx];
        if (REPL_PORT_GROUP_INTF_COUNT(unit, port_iter, repl_group) == 0) {
            BCM_IF_ERROR_RETURN(
                bcm_th_port_aggid_add(unit, port_iter, tgid, &aggid));
        }
        old_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, port_iter, repl_group);

        if(idx == 0) {
            /* all ports within the trunk should mapped to the same AGG_ID
             * encap ID only needs to be inserted once
 */
            BCM_IF_ERROR_RETURN(
                _bcm_th_ipmc_egress_intf_add(
                    unit, repl_group, port_iter, encap_id, is_l3));
            new_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, port_iter, repl_group);
            diff_count = new_intf_count - old_intf_count;
        } else {
            new_intf_count = old_intf_count + diff_count;
            REPL_PORT_GROUP_INTF_COUNT(unit, port_iter, repl_group) =
                                                        new_intf_count;
        }

     }

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_ipmc_egress_intf_del_for_trunk(int unit, int repl_group,
    bcm_trunk_t tgid, int if_max, int encap_id, int is_l3)
{
#if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
    bcm_port_t      trunk_local_member_ports[SOC_MAX_NUM_PP_PORTS] = {0};
    int             max_num_ports = SOC_MAX_NUM_PP_PORTS;
#else
    bcm_port_t      trunk_local_member_ports[SOC_MAX_NUM_PORTS] = {0};
    int             max_num_ports = SOC_MAX_NUM_PORTS;
#endif

    int idx;
    int member_count, port_iter;
    int old_intf_count, new_intf_count, diff_count = 0;

    BCM_IF_ERROR_RETURN(
        _bcm_esw_trunk_local_members_get(unit, tgid, max_num_ports,
                                         trunk_local_member_ports,
                                         &member_count));

    for (idx = 0; idx < member_count; idx++) {
        port_iter = trunk_local_member_ports[idx];
        old_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, port_iter, repl_group);
        if (idx == 0) {
            BCM_IF_ERROR_RETURN(
                _bcm_th_ipmc_egress_intf_delete(
                     unit, repl_group, trunk_local_member_ports[idx],
                     if_max, encap_id, is_l3));
            new_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, port_iter, repl_group);
            diff_count = old_intf_count - new_intf_count;
        } else {
            new_intf_count = old_intf_count - diff_count;
            REPL_PORT_GROUP_INTF_COUNT(unit, port_iter, repl_group) =
                                                            new_intf_count;
        }
        if (new_intf_count == 0 && old_intf_count > 0) {
                BCM_IF_ERROR_RETURN(
                    bcm_th_port_aggid_del(unit, port_iter));
        }

    }

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_ipmc_egress_intf_set_for_same_trunk_member(
    int unit, int repl_group, bcm_port_t port,
    bcm_port_t first_port, bcm_trunk_t tgid)
{
    int aggid;
    int old_intf_count, new_intf_count;

    old_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group);
    new_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, first_port, repl_group);
    if (old_intf_count == new_intf_count) {
        return BCM_E_NONE;
    }

    if ((old_intf_count == 0) && (new_intf_count > 0)) {
        BCM_IF_ERROR_RETURN(bcm_th_port_aggid_add(unit, port, tgid, &aggid));
    } else if ((old_intf_count > 0) && (new_intf_count == 0)) {
        BCM_IF_ERROR_RETURN(bcm_th_port_aggid_del(unit, port));
    }

    REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group) = new_intf_count;

    return BCM_E_NONE;
}
#endif

/* --------------------------------------------------------------
 * The following set of routines manage REPL_HEAD table resource.
 * --------------------------------------------------------------
 */

/*
 * Function:
 *      _bcm_th_repl_head_info_deinit
 * Purpose:
 *      De-initialize replication head info.
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      None
 */
STATIC void 
_bcm_th_repl_head_info_deinit(int unit)
{
    int pipe, i;
    _th_repl_head_free_block_t *block_ptr;
    _th_repl_head_free_block_t *next_block_ptr;

    if (NULL != _th_repl_head_info[unit]) {
        for (pipe = 0; pipe < NUM_PIPES(unit); pipe++) {
            if (NULL != _th_repl_head_info[unit][pipe].free_list_array) {
                for (i = 0; i < _th_repl_head_info[unit][pipe].array_size; i++) {
                    block_ptr = REPL_HEAD_FREE_LIST(unit, pipe, i);
                    while (NULL != block_ptr) {
                        next_block_ptr = block_ptr->next;
                        sal_free(block_ptr);
                        block_ptr = next_block_ptr;
                    }
                    REPL_HEAD_FREE_LIST(unit, pipe, i) = NULL;
                }
                sal_free(_th_repl_head_info[unit][pipe].free_list_array);
                _th_repl_head_info[unit][pipe].free_list_array = NULL;
                _th_repl_head_info[unit][pipe].array_size = 0;
            }

        }
        sal_free(_th_repl_head_info[unit]);
        _th_repl_head_info[unit] = NULL;
    }
}

/*
 * Function:
 *      _bcm_th_repl_head_info_init
 * Purpose:
 *      Initialize replication head info.
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      BCM_E_xxx
 */
STATIC int 
_bcm_th_repl_head_info_init(int unit)
{
    int alloc_size;
    int pipe;
    int max_array_index = 0;
    int per_pipe_repl_head_tbl_size; 
    soc_mem_t repl_head_table;

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
#ifdef BCM_FIREBOLT6_SUPPORT
        if (SOC_IS_FIREBOLT6(unit)) {
            mmu_port_stride[unit] = _FB6_MMU_REPL_PORTS_PER_DEV;
        } else
#endif /* BCM_FIREBOLT6_SUPPORT */
        {
            mmu_port_stride[unit] = _TD3_TDM_MMU_PORTS_PER_DEV;
        }
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
    {
        mmu_port_stride[unit] = SOC_TH_MMU_PORT_STRIDE;
    }

    /* Each pipeline has its own REPL_HEAD table. */
    alloc_size = NUM_PIPES(unit) * sizeof(_th_repl_head_info_t);
    if (NULL == _th_repl_head_info[unit]) {
        _th_repl_head_info[unit] = sal_alloc(alloc_size, "repl_head_info");
        if (NULL == _th_repl_head_info[unit]) {
            _bcm_th_repl_head_info_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_th_repl_head_info[unit], 0, alloc_size);

    for (pipe = 0; pipe < NUM_PIPES(unit); pipe++) {

        if (NULL == _th_repl_head_info[unit][pipe].free_list_array) {
            /* Each element of the array is a linked list of free blocks.
             * Array element N is a linked list of free blocks of size N.
             * When allocating a block of REPL_HEAD table entries, the max
             * number of entries needed is equal to the max number of members
             * in a replication group. This will also be the max index of the
             * array. Of course, the REPL_HEAD table may contain bigger blocks
             * of free entries. Array element 0 wil be a linked list of free
             * blocks with size greater than the max number of members in a
             * replication group.
             */ 
            max_array_index = soc_mem_field_length(unit,
                    MMU_REPL_GROUP_INFO_TBLm, PIPE_MEMBER_BMPf);
            alloc_size = (max_array_index + 1) *
                sizeof(_th_repl_head_free_block_t *);
            _th_repl_head_info[unit][pipe].free_list_array =
                sal_alloc(alloc_size, "repl head free list array");
            if (NULL == _th_repl_head_info[unit][pipe].free_list_array) {
                _bcm_th_repl_head_info_deinit(unit);
                return BCM_E_MEMORY;
            }
            _th_repl_head_info[unit][pipe].array_size = max_array_index + 1;
        }
        sal_memset(_th_repl_head_info[unit][pipe].free_list_array, 0,
                alloc_size);

        REPL_HEAD_FREE_LIST(unit, pipe, 0) = 
            sal_alloc(sizeof(_th_repl_head_free_block_t), "repl head free block");
        if (NULL == REPL_HEAD_FREE_LIST(unit, pipe, 0)) {
            _bcm_th_repl_head_info_deinit(unit);
            return BCM_E_MEMORY;
        }

        /* Initially, there is only one free block, starting at entry 1 of 
         * REPL_HEAD table and ending at the last entry. Entry 0 is reserved.
         */
        REPL_HEAD_FREE_LIST(unit, pipe, 0)->index = 1;
        per_pipe_repl_head_tbl_size = 1 << soc_mem_field_length(unit,
                MMU_REPL_GROUP_INFO_TBLm, PIPE_BASE_PTRf);
        REPL_HEAD_FREE_LIST(unit, pipe, 0)->size =
            per_pipe_repl_head_tbl_size - 1;
        REPL_HEAD_FREE_LIST(unit, pipe, 0)->next = NULL;

        if (soc_feature(unit, soc_feature_split_repl_head_table)) {
            repl_head_table = REPL_HEAD_TBL_SPLIT_PIPE(unit, 0, pipe);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, repl_head_table, MEM_BLOCK_ALL, 0,
                               soc_mem_entry_null(unit, repl_head_table)));
            repl_head_table = REPL_HEAD_TBL_SPLIT_PIPE(unit, 1, pipe);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, repl_head_table, MEM_BLOCK_ALL, 0,
                               soc_mem_entry_null(unit, repl_head_table)));

#if defined (BCM_FIREBOLT6_SUPPORT)
            if (soc_feature(unit, soc_feature_split_4_repl_head_table)) {
                repl_head_table = REPL_HEAD_TBL_SPLIT_PIPE(unit, 2, pipe);
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, repl_head_table, MEM_BLOCK_ALL, 0,
                               soc_mem_entry_null(unit, repl_head_table)));
                repl_head_table = REPL_HEAD_TBL_SPLIT_PIPE(unit, 3, pipe);
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, repl_head_table, MEM_BLOCK_ALL, 0,
                               soc_mem_entry_null(unit, repl_head_table)));
            }
#endif
        } else {
            repl_head_table = REPL_HEAD_TBL_PIPE(unit, pipe);
            /* Clear entry 0 of REPL_HEAD table */
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, repl_head_table, MEM_BLOCK_ALL, 0,
                               soc_mem_entry_null(unit, repl_head_table)));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_repl_head_block_free
 * Purpose:
 *      Free a block of REPL_HEAD table entries.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      pipe  - (IN) Pipeline number. 
 *      index - (IN) Index of the first entry of the block.
 *      size  - (IN) Size of the block.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int 
_bcm_th_repl_head_block_free(int unit, int pipe, int index, int size)
{
    int i;
    int block_index, block_size;
    int coalesced_index, coalesced_size;
    _th_repl_head_free_block_t *block_ptr;
    _th_repl_head_free_block_t *prev_block_ptr;
    _th_repl_head_free_block_t *next_block_ptr;
    _th_repl_head_free_block_t *coalesced_block_ptr;

    if (size <= 0) {
        return BCM_E_INTERNAL;
    }

    /* First, coalesce the block with any existing free blocks
     * that are contiguous with the block.
     */ 
    coalesced_index = index;
    coalesced_size = size;
    for (i = 0; i < _th_repl_head_info[unit][pipe].array_size; i++) {
        block_ptr = REPL_HEAD_FREE_LIST(unit, pipe, i);
        prev_block_ptr = NULL;
        while (NULL != block_ptr) {
            block_index = block_ptr->index;
            block_size = block_ptr->size;
            next_block_ptr = block_ptr->next;
            if ((block_index + block_size) == coalesced_index) {
                coalesced_index = block_index;
                coalesced_size += block_size;
                if (block_ptr == REPL_HEAD_FREE_LIST(unit, pipe, i)) {
                    REPL_HEAD_FREE_LIST(unit, pipe, i) = next_block_ptr;
                } else {
                    /* 
                     * In the following line of code, Coverity thinks the
                     * prev_block_ptr may still be NULL when dereferenced.
                     * This situation will never occur because 
                     * if block_ptr is not pointing to the head of the 
                     * linked list, prev_block_ptr would not be NULL.
                     */
                    /* coverity[var_deref_op : FALSE] */
                    prev_block_ptr->next = next_block_ptr;
                } 
                sal_free(block_ptr);
            } else if ((coalesced_index + coalesced_size) == block_index) {
                coalesced_size += block_size;
                if (block_ptr == REPL_HEAD_FREE_LIST(unit, pipe, i)) {
                    REPL_HEAD_FREE_LIST(unit, pipe, i) = next_block_ptr;
                } else {
                    /* 
                     * In the following line of code, Coverity thinks the
                     * prev_block_ptr may still be NULL when dereferenced.
                     * This situation will never occur because 
                     * if block_ptr is not pointing to the head of the 
                     * linked list, prev_block_ptr would not be NULL.
                     */
                    /* coverity[var_deref_op : FALSE] */
                    prev_block_ptr->next = next_block_ptr;
                } 
                sal_free(block_ptr);
            } else {
                prev_block_ptr = block_ptr;
            }
            block_ptr = next_block_ptr;
        }
    }

    /* Insert coalesced free block */
    coalesced_block_ptr = sal_alloc(sizeof(_th_repl_head_free_block_t),
            "coalesced repl head free block");
    if (NULL == coalesced_block_ptr) {
        return BCM_E_MEMORY;
    }
    coalesced_block_ptr->index = coalesced_index;
    coalesced_block_ptr->size = coalesced_size;
    if (coalesced_size > (_th_repl_head_info[unit][pipe].array_size - 1)) {
        /* Insert into free list 0 */
        coalesced_block_ptr->next = REPL_HEAD_FREE_LIST(unit, pipe, 0);
        REPL_HEAD_FREE_LIST(unit, pipe, 0) = coalesced_block_ptr;
    } else {
        coalesced_block_ptr->next = REPL_HEAD_FREE_LIST(unit, pipe,
                coalesced_size);
        REPL_HEAD_FREE_LIST(unit, pipe, coalesced_size) = coalesced_block_ptr;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_repl_head_block_alloc
 * Purpose:
 *      Allocate a free block of REPL_HEAD table entries.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      pipe - (IN) Pipeline number. 
 *      size - (IN) Size of free block requested.
 *      index - (OUT) Index of the first entry of the free block.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int 
_bcm_th_repl_head_block_alloc(int unit, int pipe, int size, int *index)
{
    int max_array_index;
    int i;
    int block_index, block_size;
    _th_repl_head_free_block_t *next_block_ptr;

    if (size == 0) {
        return BCM_E_PARAM;
    }
    if (NULL == index) {
        return BCM_E_PARAM;
    }

    max_array_index = _th_repl_head_info[unit][pipe].array_size - 1;
    for (i = size; i <= max_array_index; i++) {
        if (NULL != REPL_HEAD_FREE_LIST(unit, pipe, i)) {
           block_index = REPL_HEAD_FREE_LIST(unit, pipe, i)->index; 
           block_size = REPL_HEAD_FREE_LIST(unit, pipe, i)->size; 
           next_block_ptr = REPL_HEAD_FREE_LIST(unit, pipe, i)->next; 
           sal_free(REPL_HEAD_FREE_LIST(unit, pipe, i));
           REPL_HEAD_FREE_LIST(unit, pipe, i) = next_block_ptr;

           /* If the obtained free block contains more entries
            * than requested, insert the remainder back into
            * the free list array.
            */
           if (block_size > size) {
               BCM_IF_ERROR_RETURN(_bcm_th_repl_head_block_free(unit,
                           pipe, block_index + size, block_size - size));
           }

           *index = block_index;
           return BCM_E_NONE;
        }
    }

    /* Get free block from free list 0 */
    if (NULL != REPL_HEAD_FREE_LIST(unit, pipe, 0)) {
        block_index = REPL_HEAD_FREE_LIST(unit, pipe, 0)->index;
        block_size = REPL_HEAD_FREE_LIST(unit, pipe, 0)->size;
        next_block_ptr = REPL_HEAD_FREE_LIST(unit, pipe, 0)->next;
        if (block_size < size) {
            /* Free blocks on list 0 should never be
             * smaller than requested size.
             */
            return BCM_E_INTERNAL;
        } 

        sal_free(REPL_HEAD_FREE_LIST(unit, pipe, 0));
        REPL_HEAD_FREE_LIST(unit, pipe, 0) = next_block_ptr;

        /* If the obtained free block contains more entries
         * than requested, insert the remainder back into
         * the free list array.
         */
        if (block_size > size) {
            BCM_IF_ERROR_RETURN(_bcm_th_repl_head_block_free(unit,
                        pipe, block_index + size, block_size - size));
        }

        *index = block_index;
        return BCM_E_NONE;
    }

    /* No free block of sufficient size can be found */
    return BCM_E_RESOURCE;
}

#ifdef BCM_WARM_BOOT_SUPPORT

/*
 * Function:
 *      _bcm_th_ipmc_repl_l3_intf_scache_size_get
 * Purpose:
 *      Get the required scache size for storing a bitmap
 *      of L3 interfaces.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      size - (OUT) Number of bytes
 * Returns:
 *      BCM_E_xxx
 */
int 
_bcm_th_ipmc_repl_l3_intf_scache_size_get(
    int unit, 
    uint32 *size)
{
    int num_l3_intf;

    *size = 0;
    num_l3_intf = soc_mem_index_count(unit, EGR_L3_INTFm);
    *size += SHR_BITALLOCSIZE(num_l3_intf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ipmc_repl_l3_intf_sync
 * Purpose:
 *      Record L3 interface bitmap into the scache.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_xxx
 */
int 
_bcm_th_ipmc_repl_l3_intf_sync(
    int unit, 
    uint8 **scache_ptr)
{
    int num_l3_intf, i;
    SHR_BITDCL *l3_intf_bitmap = NULL;

    REPL_INIT(unit);
    /* Create a bitmap of L3 interfaces */
    num_l3_intf = soc_mem_index_count(unit, EGR_L3_INTFm);
    l3_intf_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_l3_intf),
            "L3 interface bitmap");
    if (NULL == l3_intf_bitmap) {
        return BCM_E_MEMORY;
    }
    sal_memset(l3_intf_bitmap, 0, SHR_BITALLOCSIZE(num_l3_intf));

    /* For the L3 interfaces that were allocated next hop indices by
     * bcm_l3_egress_create API, set the bit in the bitmap.
     */
    for (i = 0; i < num_l3_intf; i++) {
        if (REPL_L3_INTF_NEXT_HOP_IPMC(unit, i) ==
                REPL_NH_INDEX_L3_EGRESS_ALLOCATED) {
            SHR_BITSET(l3_intf_bitmap, i);
        }
    }

    /* Store the bitmap into scache */
    sal_memcpy((*scache_ptr), l3_intf_bitmap, SHR_BITALLOCSIZE(num_l3_intf));
    (*scache_ptr) += SHR_BITALLOCSIZE(num_l3_intf);

    sal_free(l3_intf_bitmap);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ipmc_repl_l3_intf_scache_recover
 * Purpose:
 *      Recover L3 interface bitmap from scache.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_ipmc_repl_l3_intf_scache_recover(int unit, uint8 **scache_ptr)
{
    int num_l3_intf, i;
    SHR_BITDCL *l3_intf_bitmap = NULL;

    /* Recover L3 interface bitmap */
    num_l3_intf = soc_mem_index_count(unit, EGR_L3_INTFm);
    l3_intf_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_l3_intf),
            "L3 interface bitmap");
    if (NULL == l3_intf_bitmap) {
        return BCM_E_MEMORY;
    }
    sal_memcpy(l3_intf_bitmap, (*scache_ptr), SHR_BITALLOCSIZE(num_l3_intf));
    (*scache_ptr) += SHR_BITALLOCSIZE(num_l3_intf);

    for (i = 0; i < num_l3_intf; i++) {
        if (SHR_BITGET(l3_intf_bitmap, i)) {
            REPL_L3_INTF_NEXT_HOP_IPMC(unit, i) =
                REPL_NH_INDEX_L3_EGRESS_ALLOCATED;
        }
    }

    sal_free(l3_intf_bitmap);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ipmc_repl_l3_intf_nh_map_scache_size_get
 * Purpose:
 *      Get the required scache size for storing map from L3 interfaces
 *      to SDK managed next hop.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      size - (OUT) Number of bytes
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_th_ipmc_repl_l3_intf_nh_map_scache_size_get(
    int unit,
    uint32 *size)
{
    int num_l3_intf;

    *size = 0;
    num_l3_intf = soc_mem_index_count(unit, EGR_L3_INTFm);
    *size += (num_l3_intf * sizeof(int));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ipmc_repl_l3_intf_nh_map_sync
 * Purpose:
 *      Record L3 interface map to SDK managed nexthop into the scache.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_th_ipmc_repl_l3_intf_nh_map_sync(
    int unit,
    uint8 **scache_ptr)
{
    int num_l3_intf, i;

    REPL_INIT(unit);

    num_l3_intf = soc_mem_index_count(unit, EGR_L3_INTFm);

    for (i = 0; i < num_l3_intf; i++) {
        *(int *) (*scache_ptr) = REPL_L3_INTF_NEXT_HOP_IPMC(unit, i);
        (*scache_ptr) += sizeof(int);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ipmc_repl_l3_intf_nh_map_scache_recover
 * Purpose:
 *      Recover L3 interface nexthop map from scache.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_ipmc_repl_l3_intf_nh_map_scache_recover(int unit, uint8 **scache_ptr)
{
    int  num_l3_intf, i;

    num_l3_intf = soc_mem_index_count(unit, EGR_L3_INTFm);

    for (i = 0; i < num_l3_intf; i++) {
        REPL_L3_INTF_NEXT_HOP_IPMC(unit, i) = *(int *)(*scache_ptr);
        (*scache_ptr) += sizeof(int);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ipmc_repl_l3_intf_trill_nh_map_scache_size_get
 * Purpose:
 *      Get the required scache size for storing map from L3 interfaces
 *      to SDK managed trill next hop.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      size - (OUT) Number of bytes
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_th_ipmc_repl_l3_intf_trill_nh_map_scache_size_get(
    int unit,
    uint32 *size)
{
    int num_l3_intf;

    *size = 0;
    num_l3_intf = soc_mem_index_count(unit, EGR_L3_INTFm);
    *size += (num_l3_intf * sizeof(int));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ipmc_repl_l3_intf_trill_nh_map_sync
 * Purpose:
 *      Record L3 interface map to SDK managed trill nexthop into the scache.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_th_ipmc_repl_l3_intf_trill_nh_map_sync(
    int unit,
    uint8 **scache_ptr)
{
    int num_l3_intf, i;

    REPL_INIT(unit);

    num_l3_intf = soc_mem_index_count(unit, EGR_L3_INTFm);

    for (i = 0; i < num_l3_intf; i++) {
        *(int *) (*scache_ptr) = REPL_L3_INTF_NEXT_HOP_TRILL(unit, i);
        (*scache_ptr) += sizeof(int);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ipmc_repl_l3_intf_trill_nh_map_scache_recover
 * Purpose:
 *      Recover L3 interface trill nexthop map from scache.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_ipmc_repl_l3_intf_trill_nh_map_scache_recover(int unit, uint8 **scache_ptr)
{
    int  num_l3_intf, i;

    num_l3_intf = soc_mem_index_count(unit, EGR_L3_INTFm);

    for (i = 0; i < num_l3_intf; i++) {
        REPL_L3_INTF_NEXT_HOP_TRILL(unit, i) = *(int *)(*scache_ptr);
        (*scache_ptr) += sizeof(int);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_repl_head_block_insert
 * Purpose:
 *      Insert a free block of REPL_HEAD table entries into free array list.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      pipe - (IN) Pipeline number. 
 *      index - (IN) Index of the first entry of the block.
 *      size - (IN) Size of block.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int 
_bcm_th_repl_head_block_insert(int unit, int pipe, int index, int size)
{
    _th_repl_head_free_block_t *block_ptr;

    block_ptr = sal_alloc(sizeof(_th_repl_head_free_block_t),
            "repl head free block");
    if (NULL == block_ptr) {
        return BCM_E_MEMORY;
    }
    block_ptr->index = index;
    block_ptr->size = size;
    if (size > (_th_repl_head_info[unit][pipe].array_size - 1)) {
        /* Insert into free list 0 */
        block_ptr->next = REPL_HEAD_FREE_LIST(unit, pipe, 0);
        REPL_HEAD_FREE_LIST(unit, pipe, 0) = block_ptr;
    } else {
        block_ptr->next = REPL_HEAD_FREE_LIST(unit, pipe, size);
        REPL_HEAD_FREE_LIST(unit, pipe, size) = block_ptr;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_repl_head_block_used_set
 * Purpose:
 *      Mark a block of REPL_HEAD table entries as used.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      pipe - (IN) Pipeline number. 
 *      index - (IN) Index of the first entry of the block.
 *      size - (IN) Size of block.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int 
_bcm_th_repl_head_block_used_set(int unit, int pipe, int index, int size)
{
    int i;
    _th_repl_head_free_block_t *prev_block_ptr;
    _th_repl_head_free_block_t *block_ptr;
    int block_index, block_size, sub_block_size;

    for (i = 0; i < _th_repl_head_info[unit][pipe].array_size; i++) {
        block_ptr = REPL_HEAD_FREE_LIST(unit, pipe, i);
        prev_block_ptr = NULL;
        while (NULL != block_ptr) {
            block_index = block_ptr->index;
            block_size = block_ptr->size;
            if ((index >= block_index) &&
                    ((index + size) <= (block_index + block_size))) {

                /* This free block contains the block to be marked as used.
                 * Remove this free block from linked list.
                 */
                if (block_ptr == REPL_HEAD_FREE_LIST(unit, pipe, i)) {
                    REPL_HEAD_FREE_LIST(unit, pipe, i) = block_ptr->next;
                } else {
                    /* 
                     * In the following line of code, Coverity thinks the
                     * prev_block_ptr may still be NULL when dereferenced.
                     * This situation will never occur because 
                     * if block_ptr is not pointing to the head of the 
                     * linked list, prev_block_ptr would not be NULL.
                     */
                    /* coverity[var_deref_op : FALSE] */
                    prev_block_ptr->next = block_ptr->next;
                }
                sal_free(block_ptr);

                /* This free block contains up to 3 sub-blocks: the sub-block
                 * to be marked as used, and the sub-blocks before and after
                 * it. The sub-blocks before and after the used sub-block
                 * need to be inserted back into the free list array.
                 */

                /* Insert the sub-block before the used sub-block back into
                 * the free list array.
                 */
                sub_block_size = index - block_index;
                if (sub_block_size > 0) {
                    BCM_IF_ERROR_RETURN(_bcm_th_repl_head_block_insert(unit,
                                pipe, block_index, sub_block_size));
                }

                /* Insert the sub-block after the used sub-block back into
                 * the free list array.
                 */
                sub_block_size = (block_index + block_size) - (index + size);
                if (sub_block_size > 0) {
                    BCM_IF_ERROR_RETURN(_bcm_th_repl_head_block_insert(unit,
                                pipe, index + size, sub_block_size));
                }

                return BCM_E_NONE;
            }

            prev_block_ptr = block_ptr;
            block_ptr = block_ptr->next;
        }
    }

    /* The block to be marked used is not found among the free blocks. */
    return BCM_E_NOT_FOUND;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/* --------------------------------------------------------------
 * The following set of routines manage REPL_LIST table resource.
 * --------------------------------------------------------------
 */

/*
 * Function:
 *      _bcm_th_repl_list_entry_info_deinit
 * Purpose:
 *      De-initialize replication list entry info.
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      None
 */
STATIC void 
_bcm_th_repl_list_entry_info_deinit(int unit)
{
    int pipe;

    if (NULL != _th_repl_list_entry_info[unit]) {
        for (pipe = 0; pipe < NUM_PIPES(unit); pipe++) {
            if (NULL != _th_repl_list_entry_info[unit][pipe].bitmap_entries_used) {
                sal_free(_th_repl_list_entry_info[unit][pipe].bitmap_entries_used);
                _th_repl_list_entry_info[unit][pipe].bitmap_entries_used = NULL;
            }
        }
        sal_free(_th_repl_list_entry_info[unit]);
        _th_repl_list_entry_info[unit] = NULL;
    }
}

/*
 * Function:
 *      _bcm_th_repl_list_entry_info_init
 * Purpose:
 *      Initialize replication list entry info.
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      BCM_E_xxx
 */
STATIC int 
_bcm_th_repl_list_entry_info_init(int unit)
{
    int alloc_size;
    int pipe;
    soc_mem_t repl_list_table;

    alloc_size = NUM_PIPES(unit) * sizeof(_th_repl_list_entry_info_t);

    if (NULL == _th_repl_list_entry_info[unit]) {
        _th_repl_list_entry_info[unit] = sal_alloc(alloc_size,
                "repl_list_entry_info");
        if (NULL == _th_repl_list_entry_info[unit]) {
            _bcm_th_repl_list_entry_info_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_th_repl_list_entry_info[unit], 0, alloc_size);

    for (pipe = 0; pipe < NUM_PIPES(unit); pipe++) {
        soc_mem_t repl_head_tbl = REPL_HEAD_TBL(unit);
        if (soc_feature(unit, soc_feature_split_repl_head_table)) {
            repl_head_tbl = REPL_HEAD_TBL_SPLIT(unit, 0);
        }
        if(SOC_IS_TOMAHAWK3(unit)) {
            repl_list_table = MMU_REPL_LIST_TBLm;
        } else  if (soc_feature(unit, soc_feature_split_2_repl_list_table)) {
            repl_list_table = REPL_LIST_TBL_SPLIT_PIPE(unit, 0, pipe);
        } else {
            repl_list_table = REPL_LIST_TBL_PIPE(unit, pipe);
        }
        _th_repl_list_entry_info[unit][pipe].num_entries = 1 <<
            soc_mem_field_length(unit, repl_head_tbl, HEAD_PTRf);
        alloc_size = SHR_BITALLOCSIZE(_th_repl_list_entry_info[unit][pipe].num_entries);
        if (NULL == _th_repl_list_entry_info[unit][pipe].bitmap_entries_used) {
            _th_repl_list_entry_info[unit][pipe].bitmap_entries_used =
                sal_alloc(alloc_size, "repl list bitmap_entries_used");
            if (NULL == _th_repl_list_entry_info[unit][pipe].bitmap_entries_used) {
                _bcm_th_repl_list_entry_info_deinit(unit);
                return BCM_E_MEMORY;
            }
        }
        sal_memset(_th_repl_list_entry_info[unit][pipe].bitmap_entries_used, 0,
                alloc_size);

        /* Reserve REPL_LIST table entry 0 */
        REPL_LIST_ENTRY_USED_SET(unit, pipe, 0);

        /* Clear entry 0 of REPL_LIST table */
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, repl_list_table, MEM_BLOCK_ALL,
                           0, soc_mem_entry_null(unit, REPL_LIST_TBL(unit))));

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_repl_list_entry_free
 * Purpose:
 *      Free a REPL_LIST table entry.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      pipe - (IN) Pipeline number. 
 *      entry_index - (IN) Index of the entry to be freed.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th_repl_list_entry_free(int unit, int pipe, int entry_index)
{
    if (!REPL_LIST_ENTRY_USED_GET(unit, pipe, entry_index)) {
        return BCM_E_INTERNAL;
    }

    REPL_LIST_ENTRY_USED_CLR(unit, pipe, entry_index);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_repl_list_entry_alloc
 * Purpose:
 *      Allocate a free entry from REPL_LIST table.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      pipe - (IN) Pipeline number. 
 *      entry_index - (OUT) Index of a free entry.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int 
_bcm_th_repl_list_entry_alloc(int unit, int pipe, int *entry_index)
{
    int i;
#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
#define _TH3_IPMC_MAX_RECYCLE_POOL_SIZE 18
        /* try to reclycle from candidate pool if pool_size > 18
         * in the worst case, 18 replicatoin list will be used
         */
        if(_repl_list_recycle_array[unit]->list_size > _TH3_IPMC_MAX_RECYCLE_POOL_SIZE) {
                int rv;
                rv = _bcm_th3_repl_list_recycle(unit, entry_index);
                if (rv == BCM_E_NONE) {
                    REPL_LIST_ENTRY_USED_SET(unit, pipe, *entry_index);
                    return rv;
                }
        }
#undef _TH3_IPMC_MAX_RECYCLE_POOL_SIZE
    }
#endif
    for (i = 0; i < _th_repl_list_entry_info[unit][pipe].num_entries; i++) {
        if (!REPL_LIST_ENTRY_USED_GET(unit, pipe, i)) {
            *entry_index = i;
            REPL_LIST_ENTRY_USED_SET(unit, pipe, i);
            return BCM_E_NONE;
        }
    }

#ifdef BCM_TOMAHAWK3_SUPPORT
    /* try to do the recylce again before returning BCM_E_RESOURCE */
    if(SOC_IS_TOMAHAWK3(unit) && _repl_list_recycle_array[unit]->list_size != 0) {
        int rv;
        rv = _bcm_th3_repl_list_recycle(unit, entry_index);
        if (BCM_FAILURE(rv)) {
            *entry_index = -1;
            return BCM_E_RESOURCE;
        } else {
            REPL_LIST_ENTRY_USED_SET(unit, pipe, *entry_index);
            return BCM_E_NONE;
        }
    }
#endif
    *entry_index = -1;
    return BCM_E_RESOURCE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_th_repl_list_entry_used_set
 * Purpose:
 *      Mark the REPL_LIST table entry as used.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      pipe - (IN) Pipeline number. 
 *      entry_index - (IN) Index of the entry to be marked.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th_repl_list_entry_used_set(int unit, int pipe, int entry_index)
{
    REPL_LIST_ENTRY_USED_SET(unit, pipe, entry_index);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_repl_list_entry_used_get
 * Purpose:
 *      Get used status of a REPL_LIST table entry.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      pipe - (IN) Pipeline number. 
 *      entry_index - (IN) Index of the entry.
 *      used - (OUT) Used status.
 * Returns:
 *      TRUE if used, else FALSE.
 */
STATIC int
_bcm_th_repl_list_entry_used_get(int unit, int pipe, int entry_index)
{
    if (REPL_LIST_ENTRY_USED_GET(unit, pipe, entry_index)) {
        return TRUE;
    }
    return FALSE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/* --------------------------------------------------------------
 * The following set of routines manage replication lists.
 * --------------------------------------------------------------
 */
/*
 * Function:
 *      bcm_th_ipmc_repl_detach
 * Purpose:
 *      Deinitialize replication.
 * Parameters:
 *      unit - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_ipmc_repl_detach(int unit)
{
    int pipe, num_ports;
    _bcm_repl_list_info_t *rli_current, *rli_free;
    bcm_port_t port;

    _bcm_th_repl_list_entry_info_deinit(unit);

    _bcm_th_repl_head_info_deinit(unit);
#ifdef BCM_TOMAHAWK3_SUPPORT
    _bcm_th3_repl_list_recycle_candidate_deinit(unit);
#endif
    if (_th_repl_info[unit] != NULL) {
        if (_th_repl_info[unit]->repl_list_info_array != NULL) {
            for (pipe = 0; pipe < NUM_PIPES(unit); pipe++) {
                rli_current = _th_repl_info[unit]->repl_list_info_array[pipe];
                while (rli_current != NULL) {
                    rli_free = rli_current;
                    rli_current = rli_current->next;
                    sal_free(rli_free);
                }
            }
            sal_free(_th_repl_info[unit]->repl_list_info_array);
        }

#ifdef BCM_MAVERICK2_SUPPORT
        if (SOC_IS_MAVERICK2(unit)) {
            /* MV2 is single pipe TD3, but regsfile has same
             port count as TD3 for this memory.Hence dividing by 2*/
            num_ports = (soc_mem_index_count(unit, ING_DEVICE_PORTm) - 1)/2;
        } else
#endif
        if (SOC_IS_TRIDENT3X(unit)) {
            num_ports = soc_mem_index_count(unit, ING_DEVICE_PORTm) - 1;
        } else {
            num_ports = soc_mem_index_count(unit, PORT_TABm) - 1;
        }
        for (port = 0; port < num_ports; port++) {
            if (_th_repl_info[unit]->port_info[port] != NULL) {
                if (_th_repl_info[unit]->
                        port_info[port]->intf_count != NULL) {
                    sal_free(_th_repl_info[unit]->
                            port_info[port]->intf_count);
                    _th_repl_info[unit]->port_info[port]->intf_count = NULL;
                }
                sal_free(_th_repl_info[unit]->port_info[port]);
                _th_repl_info[unit]->port_info[port] = NULL;
            }
        }
        if (_th_repl_info[unit]->l3_intf_next_hop_ipmc != NULL) {
            sal_free(_th_repl_info[unit]->l3_intf_next_hop_ipmc);
        }
        if (_th_repl_info[unit]->l3_intf_next_hop_trill != NULL) {
            sal_free(_th_repl_info[unit]->l3_intf_next_hop_trill);
        }

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
        /* Detach the replication ports. */
        if (soc_feature(unit, soc_feature_channelized_switching)) {
            BCM_IF_ERROR_RETURN(
                bcmi_ipmc_chn_repl_port_detach(unit));
        }
#endif

        if (_th_repl_info[unit]->port_info) {
            sal_free(_th_repl_info[unit]->port_info);
            _th_repl_info[unit]->port_info = NULL;
        }

        sal_free(_th_repl_info[unit]);
        _th_repl_info[unit] = NULL;

        if (soc_property_get(unit, spn_MULTICAST_PER_TRUNK_REPLICATION, 0)) {
            BCM_IF_ERROR_RETURN(bcm_th_aggid_info_detach(unit));
        }
    }

    return BCM_E_NONE;
}
#ifdef BCM_TOMAHAWK3_SUPPORT

STATIC int
_bcm_th3_repl_init(int unit)
{
    bcm_port_t port;
    uint32 regval;
    soc_reg_t reg = MMU_RQE_REPL_PORT_AGG_MAPr;
    _bcm_repl_list_info_t *recycle_head = NULL;
    /* init repl head */
    BCM_IF_ERROR_RETURN(_bcm_th3_repl_head_info_init(unit));

    /* init repl list entry*/
    BCM_IF_ERROR_RETURN(_bcm_th_repl_list_entry_info_init(unit));

    /* enable per trunk replication mode */
    if (soc_property_get(unit, spn_MULTICAST_PER_TRUNK_REPLICATION, 0)) {
        BCM_MC_PER_TRUNK_REPL_MODE(unit) = TRUE;
        BCM_IF_ERROR_RETURN(bcm_th_aggid_info_init(unit));
    } else {
        BCM_MC_PER_TRUNK_REPL_MODE(unit) = FALSE;
        BCM_IF_ERROR_RETURN(bcm_th_aggid_info_detach(unit));
    }
    /* init repl list recyclye candidate pool with a dummy node*/
    recycle_head = (_bcm_repl_list_info_t *)
                                sal_alloc(sizeof(_bcm_repl_list_info_t), "recycle head");

    if (NULL == recycle_head) {
        return BCM_E_MEMORY;
    }
    sal_memset(recycle_head, 0, sizeof(_bcm_repl_list_info_t));
    recycle_head->list_size = 0;
    _repl_list_recycle_array[unit] = recycle_head;
    /* map device port to agg_id */
    PBMP_ITER(PBMP_ALL(unit), port) {
        regval = 0;
        soc_reg_field_set(unit, reg, &regval, L3MC_PORT_AGG_IDf,
                            BCM_MC_PER_TRUNK_REPL_MODE(unit) ?
                                             TH3_AGG_ID_HW_INVALID : port);
        SOC_IF_ERROR_RETURN(
            soc_reg32_set(unit, reg, port, 0, regval));

    }
    if (!SOC_WARM_BOOT(unit)) {
        /* Only clear if not cold boot */
        if (!SOC_HW_RESET(unit) || SAL_BOOT_BCMSIM) {
            /* clear  */
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, MMU_REPL_MEMBER_ICCm, MEM_BLOCK_ALL, 0));

            SOC_IF_ERROR_RETURN
                 (soc_mem_clear(unit, MMU_REPL_GROUP_INFO_TBLm, MEM_BLOCK_ALL, 0));

            SOC_IF_ERROR_RETURN
                    (soc_mem_clear(unit, MMU_REPL_HEAD_TBLm, MEM_BLOCK_ALL, 0));

            SOC_IF_ERROR_RETURN
                    (soc_mem_clear(unit, MMU_REPL_LIST_TBLm, MEM_BLOCK_ALL, 0));

        }
    }
    return BCM_E_NONE;
}

#endif

/*
 * Function:
 *      bcm_th_ipmc_repl_init
 * Purpose:
 *      Initialize replication.
 * Parameters:
 *      unit - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_ipmc_repl_init(int unit)
{
    int rv = BCM_E_NONE;
    int alloc_size;
    int i;
    bcm_port_t port;
    int phy_port, mmu_port;
    int port_speed, count_width;
    uint32 regval;
    soc_mem_t ipmc_mem_inst;
    soc_info_t *si;
    soc_reg_t reg = MMU_DQS_REPL_PORT_AGG_MAPr;
    int pipe;
    int slice;
    int num_ports = SOC_MAX_NUM_PORTS;

    bcm_th_ipmc_repl_detach(unit);

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    /*
     * In case of COE, the access ports on line card connected
     * to this device are not included in this device local front
     * panel port bitmap. Therefore we need to iterate over the local
     * front panel ports as well as the line card access ports for
     *  multicast membership related operations.
     */
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        num_ports = FB6_TOTAL_NUM_SUBPORT;
    }
#endif

    if (NULL == _th_repl_info[unit]) {
       _th_repl_info[unit] = sal_alloc(sizeof(_th_repl_info_t), "repl info");
       if (NULL == _th_repl_info[unit]) {
           bcm_th_ipmc_repl_detach(unit);
           return BCM_E_MEMORY;
       }
    }
    sal_memset(_th_repl_info[unit], 0, sizeof(_th_repl_info_t));

    /* There are 4 pipelines in Tomahawk. Each pipeline has its own 
     * multicast replication tables.
     */
    _th_repl_info[unit]->num_pipes = NUM_PIPE(unit);

#ifdef BCM_TOMAHAWK3_SUPPORT
    /* There are 8 pipelines in Tomahawk3, but
     * multicast replication tables are pipeline independent.
     */
    if(SOC_IS_TOMAHAWK3(unit)) {
        _th_repl_info[unit]->num_pipes = 1;
    }
#endif
    _th_repl_info[unit]->num_repl_groups = soc_mem_index_count(unit, L3_IPMCm);

    /* The total number of replication interfaces equals to the total
     * number of next hops. Unlike previous XGS3 devices, each L3 inteface
     * will be associated with a next hop index.
     */
    _th_repl_info[unit]->num_intf = soc_mem_index_count(unit,
            EGR_L3_NEXT_HOPm);

    alloc_size = _th_repl_info[unit]->num_pipes *
        sizeof(_bcm_repl_list_info_t *);
    if (NULL == _th_repl_info[unit]->repl_list_info_array) {
        _th_repl_info[unit]->repl_list_info_array = sal_alloc(alloc_size,
                "array of linked list of replication list info");
        if (NULL == _th_repl_info[unit]->repl_list_info_array) {
            bcm_th_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_th_repl_info[unit]->repl_list_info_array, 0, alloc_size);

    /* Allocate group software state. */
    _th_repl_info[unit]->port_info = (_th_repl_port_info_t **)
        sal_alloc((num_ports *
            sizeof(_th_repl_port_info_t*)),
            "replication state for each port.");

    if (_th_repl_info[unit]->port_info == NULL) {
        return BCM_E_MEMORY;
    }

    sal_memset(_th_repl_info[unit]->port_info, 0,
        num_ports * sizeof(_th_repl_port_info_t*));


    PBMP_ITER(PBMP_ALL(unit), port) {
        /* coverity[overrun-local : FALSE] */
        if (NULL == _th_repl_info[unit]->port_info[port]) {
            _th_repl_info[unit]->port_info[port] =
                sal_alloc(sizeof(_th_repl_port_info_t), "repl port info");
            if (NULL == _th_repl_info[unit]->port_info[port]) {
                bcm_th_ipmc_repl_detach(unit);
                return BCM_E_MEMORY;
            }
        }
        sal_memset(_th_repl_info[unit]->port_info[port], 0,
                sizeof(_th_repl_port_info_t));

        alloc_size = sizeof(int) * _th_repl_info[unit]->num_repl_groups;
        if (NULL == _th_repl_info[unit]->port_info[port]->intf_count) {
            _th_repl_info[unit]->port_info[port]->intf_count = 
                sal_alloc(alloc_size, "repl port intf count");
            if (NULL == _th_repl_info[unit]->port_info[port]->intf_count) {
                bcm_th_ipmc_repl_detach(unit);
                return BCM_E_MEMORY;
            }
        }
        sal_memset(_th_repl_info[unit]->port_info[port]->intf_count, 0,
                alloc_size);
    }

    if (NULL == _th_repl_info[unit]->l3_intf_next_hop_ipmc) {
        _th_repl_info[unit]->l3_intf_next_hop_ipmc =
            sal_alloc(sizeof(int) * soc_mem_index_count(unit, EGR_L3_INTFm),
                    "l3_intf_next_hop_ipmc");
        if (NULL == _th_repl_info[unit]->l3_intf_next_hop_ipmc) {
            bcm_th_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
    }
    for (i = 0; i < soc_mem_index_count(unit, EGR_L3_INTFm); i++) {
        _th_repl_info[unit]->l3_intf_next_hop_ipmc[i] = -1;
    }

    if (NULL == _th_repl_info[unit]->l3_intf_next_hop_trill) {
        _th_repl_info[unit]->l3_intf_next_hop_trill =
            sal_alloc(sizeof(int) * soc_mem_index_count(unit, EGR_L3_INTFm),
                    "l3_intf_next_hop_trill");
        if (NULL == _th_repl_info[unit]->l3_intf_next_hop_trill) {
            bcm_th_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
    }
    for (i = 0; i < soc_mem_index_count(unit, EGR_L3_INTFm); i++) {
        _th_repl_info[unit]->l3_intf_next_hop_trill[i] = -1;
    }
#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        rv = _bcm_th3_repl_init(unit);
        if(BCM_FAILURE(rv)) {
            bcm_th_ipmc_repl_detach(unit);
        }
        return BCM_E_NONE;
    }
#endif
    rv = _bcm_th_repl_head_info_init(unit);
    if (BCM_FAILURE(rv)) {
        bcm_th_ipmc_repl_detach(unit);
        return rv;
    }

    rv = _bcm_th_repl_list_entry_info_init(unit);
    if (BCM_FAILURE(rv)) {
        bcm_th_ipmc_repl_detach(unit);
        return rv;
    }

    /* enable per trunk replication mode */
    if (soc_property_get(unit, spn_MULTICAST_PER_TRUNK_REPLICATION, 0)) {
        BCM_MC_PER_TRUNK_REPL_MODE(unit) = TRUE;
        BCM_IF_ERROR_RETURN(bcm_th_aggid_info_init(unit));
    } else {
        BCM_MC_PER_TRUNK_REPL_MODE(unit) = FALSE;
    }

    if (!SOC_WARM_BOOT(unit)) {
        si = &SOC_INFO(unit);

        for (slice = 0; slice < NUM_SLICE(unit); slice++) {
            if (!si->sc_epipe_map[slice]) {
                continue;
            }
            /* Clear replication group initial count table */
            ipmc_mem_inst = REPL_GROUP_INITIAL_COPY_COUNT_TBL_SLICE(unit, slice);
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, ipmc_mem_inst, MEM_BLOCK_ALL, 0));
#if defined(BCM_TOMAHAWK2_SUPPORT)
            if (SOC_IS_TOMAHAWK2(unit)) {
                ipmc_mem_inst =
                    SOC_MEM_UNIQUE_ACC(unit,
                                   MMU_REPL_GROUP_INITIAL_COPY_COUNT0m)[slice];
                SOC_IF_ERROR_RETURN
                    (soc_mem_clear(unit, ipmc_mem_inst, MEM_BLOCK_ALL, 0));
                ipmc_mem_inst =
                    SOC_MEM_UNIQUE_ACC(unit,
                                   MMU_REPL_GROUP_INITIAL_COPY_COUNT1m)[slice];
                SOC_IF_ERROR_RETURN
                    (soc_mem_clear(unit, ipmc_mem_inst, MEM_BLOCK_ALL, 0));
            }
#endif /* BCM_TOMAHAWK2_SUPPORT */
        }
        /* Only clear if not cold boot */
        if (!SOC_HW_RESET(unit) || SAL_BOOT_BCMSIM) {
            for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                /* Clear replication group table */
                ipmc_mem_inst = REPL_GROUP_INFO_TBL_PIPE(unit, pipe);
                SOC_IF_ERROR_RETURN
                    (soc_mem_clear(unit, ipmc_mem_inst, MEM_BLOCK_ALL, 0));

                /* Clear replication head table */
                if (soc_feature(unit, soc_feature_split_repl_head_table)) {
                    ipmc_mem_inst = REPL_HEAD_TBL_SPLIT_PIPE(unit, 0, pipe);
                    SOC_IF_ERROR_RETURN
                        (soc_mem_clear(unit, ipmc_mem_inst, MEM_BLOCK_ALL, 0));
                    ipmc_mem_inst = REPL_HEAD_TBL_SPLIT_PIPE(unit, 1, pipe);
                    SOC_IF_ERROR_RETURN
                        (soc_mem_clear(unit, ipmc_mem_inst, MEM_BLOCK_ALL, 0));

                    if (soc_feature(unit, soc_feature_split_4_repl_head_table)) {

                        ipmc_mem_inst = REPL_HEAD_TBL_SPLIT_PIPE(unit, 2, pipe);
                        SOC_IF_ERROR_RETURN
                            (soc_mem_clear(unit, ipmc_mem_inst, MEM_BLOCK_ALL, 0));
                        ipmc_mem_inst = REPL_HEAD_TBL_SPLIT_PIPE(unit, 3, pipe);
                        SOC_IF_ERROR_RETURN
                            (soc_mem_clear(unit, ipmc_mem_inst, MEM_BLOCK_ALL, 0));
                    }
                } else {
                    ipmc_mem_inst = REPL_HEAD_TBL_PIPE(unit, pipe);
                    SOC_IF_ERROR_RETURN
                        (soc_mem_clear(unit, ipmc_mem_inst, MEM_BLOCK_ALL, 0));
                }

                /* Clear replication list table */
                if (soc_feature(unit, soc_feature_split_2_repl_list_table)) {
                    ipmc_mem_inst = REPL_LIST_TBL_SPLIT_PIPE(unit, 0, pipe);
                    SOC_IF_ERROR_RETURN
                        (soc_mem_clear(unit, ipmc_mem_inst, MEM_BLOCK_ALL, 0));

                    ipmc_mem_inst = REPL_LIST_TBL_SPLIT_PIPE(unit, 1, pipe);
                    SOC_IF_ERROR_RETURN
                        (soc_mem_clear(unit, ipmc_mem_inst, MEM_BLOCK_ALL, 0));
                } else {

                    ipmc_mem_inst = REPL_LIST_TBL_PIPE(unit, pipe);
                    SOC_IF_ERROR_RETURN
                        (soc_mem_clear(unit, ipmc_mem_inst, MEM_BLOCK_ALL, 0));
                }
            }
        }

        /* Initialize PORT_INITIAL_COPY_COUNT_WIDTH registers */
        PBMP_ITER(PBMP_ALL(unit), port) {
            /* coverity[overrun-local : FALSE] */
            phy_port = SOC_INFO(unit).port_l2p_mapping[port];
            /* coverity[overrun-local : FALSE] */
            mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
            /* coverity[illegal_address : FALSE] */
            if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) ||
                IS_MANAGEMENT_PORT(unit, port) || IS_FAE_PORT(unit, port)) {
                /* For CPU, loopback, and management ports,
                 * the copy count width is fixed at 2/3 bits
                 * (encoded value of 0/1).
                 * Pysical ports 129 and 131 are management ports.
                 */
                count_width = 1;
                if (phy_port == 129 || phy_port == 131 ||
                    (phy_port == 128 && SOC_IS_TRIDENT3(unit)) || (phy_port == 81 && SOC_IS_MAVERICK2(unit))) {
                    if (soc_mem_field_length(unit,
                        MMU_REPL_GROUP_INITIAL_COPY_COUNTm, ICC_BIT_MPf) <= 2) {
                        count_width = 0;
                    }
                }
            } else {
                if (SOC_IS_TOMAHAWK2(unit)) {
                    /* Single-lane ports are allocated 2 bits, while 2 or 4
                     * lane ports are allocated 3 bits. */
                    count_width = (si->port_num_lanes[port] > 1) ? 1 : 0;
                } else {
#ifdef BCM_HURRICANE4_SUPPORT
                    if (SOC_IS_HURRICANE4(unit) && IS_MACSEC_PORT(unit, port)) {
                        port_speed = si->port_speed_max[port];
                    } else
#endif /* BCM_HURRICANE4_SUPPORT */
                    {
                        /* For ports with speed 40G and above the copy count width
                         * is 3 bits (encoded value of 1) for lower speeds its
                         * 2 bits (encoded value of 0).
                         */
                        BCM_IF_ERROR_RETURN
                            (bcm_esw_port_speed_get(unit, port, &port_speed));
                    }
                    count_width = (port_speed >= 40000) ? 1 : 0;
                }
            }

            /* If the port_initial_copy_count is not skipped, update the same */
            if (!soc_feature(unit, soc_feature_skip_port_initial_copy_count)) {
                regval = 0;
                soc_reg_field_set(unit, PORT_INITIAL_COPY_COUNT_WIDTHr, &regval,
                        BIT_WIDTHf, count_width);
                SOC_IF_ERROR_RETURN(WRITE_PORT_INITIAL_COPY_COUNT_WIDTHr(unit,
                            port, regval));
            }

            /* configure mmu port to repl aggregateId map */
            regval = 0;

            if (soc_feature(unit, soc_feature_repl_port_agg_map_is_mem)) {
                /* Font panel and subports are initialized below together. */

            } else {


#ifdef BCM_TOMAHAWK3_SUPPORT
                reg = (SOC_IS_TOMAHAWK3(unit)) ? MMU_RQE_REPL_PORT_AGG_MAPr :
                      MMU_DQS_REPL_PORT_AGG_MAPr;
#endif

                soc_reg_field_set(unit, reg, &regval,
                      L3MC_PORT_AGG_IDf,
                      BCM_MC_PER_TRUNK_REPL_MODE(unit) ?
                      TH_AGG_ID_HW_INVALID : (mmu_port % mmu_port_stride[unit]));
                SOC_IF_ERROR_RETURN(
                    soc_reg32_set(unit, reg, port, 0, regval));
            }
        }
    }

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        int agg_mem_count=0, i=0;
        mmu_dqs_repl_port_agg_map_entry_t  dqs_entry;
        mmu_rqe_repl_port_agg_map_entry_t  rqe_entry;

        agg_mem_count = soc_mem_index_count(unit,
                        MMU_DQS_REPL_PORT_AGG_MAP_PIPE0m);

        for (; i<agg_mem_count; i++) {

            sal_memset(&dqs_entry, 0, sizeof(mmu_dqs_repl_port_agg_map_entry_t));

            soc_mem_field32_set(unit,
                 MMU_DQS_REPL_PORT_AGG_MAP_PIPE(unit, 0), &dqs_entry,
                 L3MC_PORT_AGG_IDf, BCM_MC_PER_TRUNK_REPL_MODE(unit) ?
                 TH_AGG_ID_HW_INVALID : i);


            SOC_IF_ERROR_RETURN(
                soc_mem_write(unit, MMU_DQS_REPL_PORT_AGG_MAP_PIPE(unit, 0),
                MEM_BLOCK_ALL, i, &dqs_entry));
        }

        for (i=0; i<agg_mem_count; i++) {

            sal_memset(&rqe_entry, 0, sizeof(mmu_rqe_repl_port_agg_map_entry_t));

            soc_mem_field32_set(unit,
                 MMU_RQE_REPL_PORT_AGG_MAP_SC0m, &rqe_entry,
                 L3MC_PORT_AGG_IDf, BCM_MC_PER_TRUNK_REPL_MODE(unit) ?
                 TH_AGG_ID_HW_INVALID : i);

            SOC_IF_ERROR_RETURN(
                soc_mem_write(unit, MMU_RQE_REPL_PORT_AGG_MAP_SC0m,
                MEM_BLOCK_ALL, i, &rqe_entry));
        }


        /*
         * Channelization ports replication attach.
         */
        BCM_IF_ERROR_RETURN(
            bcmi_ipmc_chn_repl_port_attach(unit));

    }
#endif

    return rv;
}


/* Function:
 *	_bcm_th_port_pipe_get
 * Purpose:
 *      Get pipeline to which the given port belongs.
 * Parameters:
 *	unit - SOC unit number.
 *	port - Port number.
 *      pipe - (OUT) Pipeline number.
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_th_port_pipe_get(int unit, int port, int *pipe)
{
    soc_info_t *si = &SOC_INFO(unit);

    if (si->port_pipe[port] == -1) {
        return BCM_E_PORT;
    }

    *pipe = si->port_pipe[port];

    return BCM_E_NONE;
}

/* Function:
 *	_bcm_th_port_slice_get
 * Purpose:
 *      Get MMU slice to which the given port belongs.
 * Parameters:
 *	unit - SOC unit number.
 *	port - Port number.
 *      slice - (OUT) Slice number.
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_td3_port_slice_get(int unit, int port, int *slice)
{
    *slice = 0;

    return BCM_E_NONE;
}

/* Function:
 *	_bcm_th_port_slice_get
 * Purpose:
 *      Get MMU slice to which the given port belongs.
 * Parameters:
 *	unit - SOC unit number.
 *	port - Port number.
 *      slice - (OUT) Slice number.
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_th_port_slice_get(int unit, int port, int *slice)
{
    int pipe;

    if (SOC_IS_TRIDENT3X(unit)) {
        return _bcm_td3_port_slice_get(unit, port, slice);
    }
    /* Get pipeline number */
    BCM_IF_ERROR_RETURN(_bcm_th_port_pipe_get(unit, port, &pipe));

    /* Derive slice number */
    if (pipe <= 1) {
        *slice = 0;
    } else {
        *slice = 1;
    }

    return BCM_E_NONE;
}

/* Function:
 *	_bcm_td3_repl_icc_set
 * Purpose:
 *      Set replication initial copy count.
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	repl_group - The replication group number.
 *	port       - Port.
 *	intf_count - Replication list's interface count.
 *  count_width - count_width
 *  copy_count - copy_count
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_td3_repl_icc_set(int unit, int repl_group, bcm_port_t port,
                      int intf_count, int count_width, int copy_count)
{
    soc_info_t *si;
    bcm_port_t phy_port;
    uint32 initial_copy_count_entry[SOC_MAX_MEM_WORDS];
/*    mmu_repl_group_initial_copy_count_entry_t initial_copy_count_entry;*/
    uint32 count_mask, count_shift;
    SHR_BITDCL copy_count_buf[32];
    int slice = 0;    /* Only 1 slice in TD3 */
    soc_mem_t repl_group_icc_table;
#if defined(BCM_HELIX5_SUPPORT)
    int bit = 0;
#endif

    sal_memset(initial_copy_count_entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
    /* TD3TBD:
     * PORT_INITIAL_COPY_COUNT_WIDTHr NEEDS to be updated while
     * Chaging lanes or speed */

    /* Update REPL_GROUP_INITIAL_COPY_COUNT table */
    repl_group_icc_table =
        REPL_GROUP_INITIAL_COPY_COUNT_TBL_SLICE(unit, slice);


#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_repl_head_icc)) {
        return BCM_E_NONE;
    }
#endif

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, repl_group_icc_table, MEM_BLOCK_ANY,
                      repl_group, &initial_copy_count_entry));

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[port];
    if (SOC_IS_TRIDENT3(unit)) {
        if (slice == 0) {
            if (phy_port == 128 &&
                    SOC_PORT_BLOCK(unit, phy_port) == SOC_PORT_BLOCK(unit, 129)) {
                /* Second management port */
                soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                        &initial_copy_count_entry, ICC_BIT_2ND_MPf, copy_count);
            } else if (phy_port == 131) {
                soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                        &initial_copy_count_entry, ICC_BIT_LP_PYf, copy_count);
            } else if (phy_port == 130) {
                soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                        &initial_copy_count_entry, ICC_BIT_LP_PXf, copy_count);
            } else if (phy_port == 129) {
                soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                        &initial_copy_count_entry, ICC_BIT_MPf, copy_count);
            } else if (IS_CPU_PORT(unit, port)) {
                soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                        &initial_copy_count_entry, ICC_BIT_CPUf, copy_count);
            } else if (phy_port <= 128 && phy_port >= 65) {
                soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field_get(unit,
                        &initial_copy_count_entry, ICC_BIT_FP_PYf, copy_count_buf);
                count_mask = (1 << count_width) - 1;
                count_shift = (phy_port - 65) * 2 % 32;
                copy_count_buf[(phy_port - 65) * 2 / 32] &= ~(count_mask << count_shift);
                copy_count_buf[(phy_port - 65) * 2 / 32] |= (copy_count << count_shift);
                soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field_set(unit,
                        &initial_copy_count_entry, ICC_BIT_FP_PYf, copy_count_buf);
            } else if (phy_port <= 64 && phy_port >= 1) {
                soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field_get(unit,
                        &initial_copy_count_entry, ICC_BIT_FP_PXf, copy_count_buf);
                count_mask = (1 << count_width) - 1;
                count_shift = (phy_port - 1) * 2 % 32;
                copy_count_buf[(phy_port - 1) * 2 / 32] &= ~(count_mask << count_shift);
                copy_count_buf[(phy_port - 1) * 2 / 32] |= (copy_count << count_shift);
                soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field_set(unit,
                        &initial_copy_count_entry, ICC_BIT_FP_PXf, copy_count_buf);
            } else {
                return BCM_E_PORT;
            }
        }
    }

#if defined(BCM_MAVERICK2_SUPPORT)
    if (SOC_IS_MAVERICK2(unit)) {
        if (slice == 0) {
            if (IS_LB_PORT(unit, port)) {
                soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                        &initial_copy_count_entry, ICC_BIT_LP_PXf, copy_count);
            } else if (IS_MANAGEMENT_PORT(unit, port)) {
                soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                        &initial_copy_count_entry, ICC_BIT_MPf, copy_count);
            } else if (IS_CPU_PORT(unit, port)) {
                soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                        &initial_copy_count_entry, ICC_BIT_CPUf, copy_count);
            } else if (IS_E_PORT(unit, port) || IS_HG_PORT(unit, port) ||
                       IS_MACSEC_PORT(unit, port)) {
                soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field_get(unit,
                        &initial_copy_count_entry, ICC_BIT_FP_PXf, copy_count_buf);
                count_mask = (1 << count_width) - 1;
                count_shift = (phy_port - 1) * 2 % 32;
                copy_count_buf[(phy_port - 1) * 2 / 32] &= ~(count_mask << count_shift);
                copy_count_buf[(phy_port - 1) * 2 / 32] |= (copy_count << count_shift);
                soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field_set(unit,
                        &initial_copy_count_entry, ICC_BIT_FP_PXf, copy_count_buf);
            } else {
                return BCM_E_PORT;
            }
        }
    }
#endif

#if defined(BCM_HELIX5_SUPPORT)
    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
        if (slice == 0) {
            if (IS_LB_PORT(unit, port)) {
                soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                        &initial_copy_count_entry, ICC_BIT_LP_PXf, copy_count);
            } else if (IS_CPU_PORT(unit, port)) {
                soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                        &initial_copy_count_entry, ICC_BIT_CPUf, copy_count);
            } else if (IS_E_PORT(unit, port) || IS_HG_PORT(unit, port) ||
                    IS_MACSEC_PORT(unit, port)) {
                soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field_get(unit,
                        &initial_copy_count_entry, ICC_BIT_FP_PXf, copy_count_buf);
                count_shift = (phy_port - 1) * 3;
                for (bit = 0; bit < count_width; bit++) {
                    if (copy_count & (1 << bit)) {
                        SHR_BITSET(copy_count_buf, count_shift + bit);
                    } else {
                        SHR_BITCLR(copy_count_buf, count_shift + bit);
                    }
                }

                soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field_set(unit,
                        &initial_copy_count_entry, ICC_BIT_FP_PXf, copy_count_buf);
            } else {
                return BCM_E_PORT;
            }
        }
    }
#endif

    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, repl_group_icc_table, MEM_BLOCK_ALL,
                       repl_group, &initial_copy_count_entry));

    return BCM_E_NONE;
}

/* Function:
 *	_bcm_th_repl_initial_copy_count_set
 * Purpose:
 *      Set replication initial copy count.
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	repl_group - The replication group number.
 *	port       - Port.
 *	intf_count - Replication list's interface count.
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_th_repl_initial_copy_count_set(int unit, int repl_group, bcm_port_t port,
                int intf_count)
{
    soc_info_t *si;
    bcm_port_t phy_port;
    uint32 regval;
    int count_width, copy_count;
    mmu_repl_group_initial_copy_count_entry_t initial_copy_count_entry;
    uint32 count_mask, count_shift, count_column;
    uint32 copy_count_buf[4];
    int pipe, slice;
    soc_mem_t repl_group_icc_table;
#if defined(BCM_TOMAHAWK2_SUPPORT)
    soc_mem_t repl_group_icc_table0 = INVALIDm;
    soc_mem_t repl_group_icc_table1 = INVALIDm;
#endif /* BCM_TOMAHAWK2_SUPPORT */

    /* Get port's copy count width */
    if (!soc_feature(unit, soc_feature_skip_port_initial_copy_count)) {
        SOC_IF_ERROR_RETURN(READ_PORT_INITIAL_COPY_COUNT_WIDTHr(unit,
                    port, &regval));
        count_width = 2 + soc_reg_field_get(unit,
                PORT_INITIAL_COPY_COUNT_WIDTHr, regval, BIT_WIDTHf);
    } else {
        /* In HX5, the above reg is marked as reserved, since for all
         * front-panel ports the initial-copy-count-width has been setup as 3.
         */
        count_width = 3;
    }
    /* Compute port's initial copy count */
    copy_count = 0;
    switch (count_width) {
        case 2:
            if (intf_count <= 3) {
                copy_count = intf_count;
            }
            break;
        case 3:
            if (intf_count <= 5) {
                copy_count = intf_count;
            }
            break;
        default:
            return BCM_E_INTERNAL;
            break;
    }

    if (SOC_IS_TRIDENT3X(unit)) {
        return _bcm_td3_repl_icc_set(unit, repl_group, port, intf_count,
                                     count_width, copy_count);
    }

    /* Get pipeline number */
    BCM_IF_ERROR_RETURN(_bcm_th_port_pipe_get(unit, port, &pipe));
    /* Update REPL_GROUP_INITIAL_COPY_COUNT table */
    BCM_IF_ERROR_RETURN(_bcm_th_port_slice_get(unit, port, &slice));

    repl_group_icc_table =
        SOC_MEM_UNIQUE_ACC(unit, MMU_REPL_GROUP_INITIAL_COPY_COUNTm)[slice];
#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (SOC_IS_TOMAHAWK2(unit)) {
        repl_group_icc_table0 =
            SOC_MEM_UNIQUE_ACC(unit, MMU_REPL_GROUP_INITIAL_COPY_COUNT0m)[slice];
        repl_group_icc_table1 =
            SOC_MEM_UNIQUE_ACC(unit, MMU_REPL_GROUP_INITIAL_COPY_COUNT1m)[slice];
    }
#endif /* BCM_TOMAHAWK2_SUPPORT */
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, repl_group_icc_table, MEM_BLOCK_ANY,
                      repl_group, &initial_copy_count_entry));
    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[port];

#   define IS_X_PIPE(_pipe_)               ((_pipe_ & 1) == 0)
#   define IS_Y_PIPE(_pipe_)               ((_pipe_ & 1) == 1)
#   define PORT_NUM_PER_PIPE(_u_)          (SOC_IS_TOMAHAWK2(_u_) ? 64 : 32)
#   define PHY_PORT_ID_PER_PIPE(_u_, _pp_) ((_pp_ - 1) % PORT_NUM_PER_PIPE(_u_))
    
    if (IS_LB_PORT(unit, port) && IS_Y_PIPE(pipe)) {
        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                &initial_copy_count_entry, ICC_BIT_LP_PYf, copy_count);
    } else if (IS_LB_PORT(unit, port) && IS_X_PIPE(pipe)) {
        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                &initial_copy_count_entry, ICC_BIT_LP_PXf, copy_count);
    } else if (IS_MANAGEMENT_PORT(unit, port)) {
        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                &initial_copy_count_entry, ICC_BIT_MPf, copy_count);
    } else if (IS_CPU_PORT(unit, port)) {
        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                &initial_copy_count_entry, ICC_BIT_CPUf, copy_count);
    } else if (IS_Y_PIPE(pipe)) {
        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field_get(unit,
                &initial_copy_count_entry, ICC_BIT_FP_PYf, copy_count_buf);
        count_mask   = (1 << count_width) - 1;
        count_shift  = PHY_PORT_ID_PER_PIPE(unit, phy_port) * 2 % 32;
        count_column = PHY_PORT_ID_PER_PIPE(unit, phy_port) * 2 / 32;
        copy_count_buf[count_column] &= ~(count_mask << count_shift);
        copy_count_buf[count_column] |=  (copy_count << count_shift);
        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field_set(unit,
                &initial_copy_count_entry, ICC_BIT_FP_PYf, copy_count_buf);
    } else if (IS_X_PIPE(pipe)) {
        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field_get(unit,
                &initial_copy_count_entry, ICC_BIT_FP_PXf, copy_count_buf);
        count_mask   = (1 << count_width) - 1;
        count_shift  = PHY_PORT_ID_PER_PIPE(unit, phy_port) * 2 % 32;
        count_column = PHY_PORT_ID_PER_PIPE(unit, phy_port) * 2 / 32;
        copy_count_buf[count_column] &= ~(count_mask << count_shift);
        copy_count_buf[count_column] |=  (copy_count << count_shift);
        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field_set(unit,
                &initial_copy_count_entry, ICC_BIT_FP_PXf, copy_count_buf);
    } else {
        return BCM_E_PORT;
    }
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, repl_group_icc_table, MEM_BLOCK_ALL, 
                       repl_group, &initial_copy_count_entry));
#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (SOC_IS_TOMAHAWK2(unit)) {
        /* coverity[negative_returns : FALSE] */
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, repl_group_icc_table0, MEM_BLOCK_ALL, 
                           repl_group, &initial_copy_count_entry));
        /* coverity[negative_returns : FALSE] */
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, repl_group_icc_table1, MEM_BLOCK_ALL, 
                           repl_group, &initial_copy_count_entry));
    }
#endif /* BCM_TOMAHAWK2_SUPPORT */
    return BCM_E_NONE;
}


STATIC int
bcmi_repl_list_tbl_write(int unit, int *pipe, int index, uint32 *entry)
{
    soc_mem_t repl_list_tbl;
    int max_index_offset = 0;

    if (!pipe || !entry) {
        return BCM_E_PARAM;
    }

    if (soc_feature(unit, soc_feature_split_2_repl_list_table)) {
        int split =0;
        max_index_offset = 64;

        split = index / ( max_index_offset * 1024);

        if (split > 1) {
            return BCM_E_PARAM;
        }

        if (index >= (split * max_index_offset * 1024)) {
            index -= (split * max_index_offset * 1024);
        }

        repl_list_tbl = REPL_LIST_TBL_SPLIT_PIPE(unit, split, *pipe);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, repl_list_tbl, MEM_BLOCK_ALL,
                                          index, entry));
    } else {
        if(SOC_IS_TOMAHAWK3(unit)) {
            repl_list_tbl = MMU_REPL_LIST_TBLm;
            *pipe = 0;
        } else {
            repl_list_tbl = REPL_LIST_TBL_PIPE(unit, *pipe);
        }

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, repl_list_tbl, MEM_BLOCK_ALL,
                                          index, entry));
    }

    return BCM_E_NONE;
}


STATIC int
bcmi_repl_list_tbl_read(int unit, int *pipe, int index, uint32 *entry)
{
    soc_mem_t repl_list_tbl;
    int max_index_offset = 0;

    if (!entry || !pipe) {
        return BCM_E_PARAM;
    }

    if (soc_feature(unit, soc_feature_split_2_repl_list_table)) {
        int split =0;
        max_index_offset = 64;

        split = index / ( max_index_offset * 1024);

        if (split > 1) {
            return BCM_E_PARAM;
        }

        if (index >= (split * max_index_offset * 1024)) {
            index -= (split * max_index_offset * 1024);
        }
        repl_list_tbl = REPL_LIST_TBL_SPLIT_PIPE(unit, split, *pipe);
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, repl_list_tbl, MEM_BLOCK_ALL,
                                         index, entry));
    } else {

        if(SOC_IS_TOMAHAWK3(unit)) {
            repl_list_tbl = MMU_REPL_LIST_TBLm;
            *pipe = 0;
        } else {
            repl_list_tbl = REPL_LIST_TBL_PIPE(unit, *pipe);
        }

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, repl_list_tbl, MEM_BLOCK_ALL,
                                         index, entry));
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_td3_repl_head_tbl_write(int unit, int pipe, int index, uint32 *entry)
{
    soc_mem_t repl_hd_tbl;
    int max_index_offset = 0;

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        max_index_offset = 16;
    } else
#endif
    if (SOC_IS_HELIX5(unit)) {
        max_index_offset = 32;
    } else {
        max_index_offset = 64;
    }

    if (soc_feature(unit, soc_feature_split_repl_head_table)) {
        int split =0;
        split = index / ( max_index_offset * 1024);

        if (split > 3) {
            return BCM_E_PARAM;
        }

        if (index >= (split * max_index_offset * 1024)) {
            index -= (split * max_index_offset * 1024);
        }

        repl_hd_tbl = REPL_HEAD_TBL_SPLIT_PIPE(unit, split, pipe);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, repl_hd_tbl, MEM_BLOCK_ALL,
                                          index, entry));
    } else {
        repl_hd_tbl = REPL_HEAD_TBL_PIPE(unit, pipe);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, repl_hd_tbl, MEM_BLOCK_ALL,
                                          index, entry));
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td3_repl_head_tbl_read(int unit, int pipe, int index, uint32 *entry, uint32 *head_ptr)
{
    soc_mem_t repl_hd_tbl;
    int max_index_offset = 0;

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        max_index_offset = 16;
    } else
#endif
    if (SOC_IS_HELIX5(unit)) {
        max_index_offset = 32;
    } else {
        max_index_offset = 64;
    }

    if (soc_feature(unit, soc_feature_split_repl_head_table)) {
        int split =0;
        split = index / ( max_index_offset * 1024);

        if (split > 3) {
            return BCM_E_PARAM;
        }

        if (index >= (split * max_index_offset * 1024)) {
            index -= (split * max_index_offset * 1024);
        }

        repl_hd_tbl = REPL_HEAD_TBL_SPLIT_PIPE(unit, split, pipe);
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, repl_hd_tbl, MEM_BLOCK_ALL,
                                         index, entry));
        if (head_ptr) {
            *head_ptr = soc_mem_field32_get(unit, repl_hd_tbl, entry,
                                             HEAD_PTRf);
        }
    } else {

#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (SOC_IS_TOMAHAWK3(unit)) {
            repl_hd_tbl = MMU_REPL_HEAD_TBLm;
        } else
#endif
        {
            repl_hd_tbl = REPL_HEAD_TBL_PIPE(unit, pipe);
        }

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, repl_hd_tbl, MEM_BLOCK_ALL,
                                         index, entry));
        if (head_ptr) {
            *head_ptr = soc_mem_field32_get(unit, repl_hd_tbl, entry, HEAD_PTRf);
        }
    }

    return BCM_E_NONE;
}

#ifdef BCM_CHANNELIZED_SWITCHING_SUPPORT

/*
 * Function:
 * bcmi_fb6_repl_head_icc_update
 * Purpose:
 *  Update the icc table based on the head.
 * Parameters:
 *  unit       - StrataSwitch PCI device unit number.
 *  pipe       - Pipeline number.
 *  head_index - (IN) head index for multicast replication list head.
 * Returns:
 *  BCM_E_XXX
 */
bcm_error_t
bcmi_fb6_repl_head_icc_update(int unit, int repl_group, int pipe)
{

    uint32 initial_copy_count_entry[SOC_MAX_MEM_WORDS];
    uint32 entry[SOC_MAX_MEM_WORDS];
/*    mmu_repl_group_initial_copy_count_entry_t initial_copy_count_entry;*/
    soc_mem_t repl_group_table;
    uint32 count_shift;
    SHR_BITDCL copy_count_buf[32];
    soc_mem_t repl_group_icc_table;
    int rv =  BCM_E_NONE;
    int count_width = 3;
    uint32      hw_ls_bits[2];
    uint32      repl_list_entry[SOC_MAX_MEM_FIELD_WORDS];
    uint32 fldbuf[BCM_REPL_HEAD_MEMBER_WORD_MAX] = {0};
    soc_mem_t   mem;
    int         repl_entry_ptr, prev_repl_entry_ptr;
    uint32      start_ptr;
    int         copy_count = 0;
    uint32 icc_row_idx, icc_clm_idx;
    int bit = 0, i=0, member_count=0;
    soc_field_t member_bitmap_f, head_index_f;
    int head_index = 0;


    member_bitmap_f = PIPE_MEMBER_BMPf;
    head_index_f = PIPE_BASE_PTRf;

    /* Read the L3_IPMC table to get number of ports. */
    sal_memset(entry, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));
    repl_group_table = REPL_GROUP_INFO_TBL_PIPE(unit, pipe);

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, repl_group_table, MEM_BLOCK_ANY,
                      repl_group, &entry));
    sal_memset(fldbuf, 0, BCM_REPL_HEAD_MEMBER_WORD_MAX * sizeof(uint32));
    soc_mem_field_get(unit, repl_group_table, (uint32 *)&entry,
            member_bitmap_f, fldbuf);
    for (i = 0; i < BCM_REPL_HEAD_MEMBER_WORD_MAX; i++) {
        member_count += _shr_popcount(fldbuf[i]);
    }

    head_index = soc_mem_field32_get(unit, repl_group_table,
            &entry, head_index_f);

    mem = REPL_LIST_TBL(unit);

    repl_group_icc_table =
        REPL_GROUP_INITIAL_COPY_COUNT_TBL_SLICE(unit, 0);

    for (i=0; i<member_count; i++) {
        prev_repl_entry_ptr = -1;
        sal_memset(entry, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));
        BCM_IF_ERROR_RETURN(
            _bcm_td3_repl_head_tbl_read(unit, pipe, (head_index+i), entry, &start_ptr));
        repl_entry_ptr = start_ptr;

        copy_count = 0;
        /* Get the total index count for this head. */
        do {
            SOC_IF_ERROR_RETURN(bcmi_repl_list_tbl_read(unit, &pipe,
                repl_entry_ptr, repl_list_entry));

            soc_mem_field_get(unit, mem, repl_list_entry,
                LSB_VLAN_BMf, hw_ls_bits);

            copy_count += _shr_popcount(hw_ls_bits[0]);
            copy_count += _shr_popcount(hw_ls_bits[1]);

            prev_repl_entry_ptr = repl_entry_ptr;
            repl_entry_ptr = soc_mem_field32_get(unit, mem,
                repl_list_entry, NEXTPTRf);
        } while (repl_entry_ptr != prev_repl_entry_ptr);


        if (copy_count > 6) {
            /*
             * The ICC value for replication copies is in range 0 to 6.
             * A value of zero indicates unknown copy count and hence allowed only for
             * 7 or more IPMC copies per packet.
             */
            copy_count = 0;
        }

        /* Now we have the count for that head and not only for that group. */

        icc_row_idx = (head_index+i)/32;
        icc_clm_idx = (head_index+i) % 32;

        /* Update the read entry now. */
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, repl_group_icc_table, MEM_BLOCK_ANY,
             icc_row_idx, &initial_copy_count_entry));

        soc_mem_field_get(unit, repl_group_icc_table,
            (uint32 *)&initial_copy_count_entry, INITIAL_COPY_COUNTf,
            copy_count_buf);

        /* For each head index there are 3 bits. */
        icc_clm_idx = (icc_clm_idx) * 3;

        soc_mem_field_get(unit, repl_group_icc_table,
            (uint32 *)&initial_copy_count_entry, INITIAL_COPY_COUNTf,
            copy_count_buf);

        /* shift is equal to the total bits for this head. */
        count_shift = icc_clm_idx;

        for (bit = 0; bit < count_width; bit++) {
            if (copy_count & (1 << bit)) {
                SHR_BITSET(copy_count_buf, count_shift + bit);
            } else {
                SHR_BITCLR(copy_count_buf, count_shift + bit);
            }
        }

        soc_mem_field_set(unit, repl_group_icc_table,
            (uint32 *)&initial_copy_count_entry, INITIAL_COPY_COUNTf,
            copy_count_buf);

        rv = soc_mem_write(unit, repl_group_icc_table, MEM_BLOCK_ALL,
            icc_row_idx, &initial_copy_count_entry);

        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
    return rv;
}
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

/* Function:
 *	_bcm_th_repl_list_start_ptr_set
 * Purpose:
 *      Set replication list start pointer for given (repl_group, port).
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	repl_group - The replication group number.
 *	port       - Port.
 *	start_ptr  - Replication list's start pointer.
 *	intf_count - Replication list's interface count.
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_th_repl_list_start_ptr_set(int unit, int repl_group, bcm_port_t port,
                int start_ptr, int intf_count, bcm_gport_t gport)
{
    int add_member;
    soc_mem_t repl_group_table;
    soc_mem_t repl_head_table;
    soc_field_t member_bitmap_f, head_index_f; 
    soc_info_t *si;
    bcm_port_t phy_port, mmu_port, local_port;
    int member_bitmap_index;
    int pipe;
    mmu_repl_group_info_tbl_entry_t repl_group_entry;
    uint32 fldbuf[BCM_REPL_HEAD_MEMBER_WORD_MAX] = {0};
    int i;
    uint32 old_member_bitmap[BCM_REPL_HEAD_MEMBER_WORD_MAX] = {0};
    uint32 new_member_bitmap[BCM_REPL_HEAD_MEMBER_WORD_MAX] = {0};
    int old_member_count=0, new_member_count=0;
    int old_head_index, new_head_index;
    int member_id, member, old_member_id, new_member_id;
    uint32 repl_head_entry[SOC_MAX_MEM_FIELD_WORDS];
    uint32 old_repl_head_entry[SOC_MAX_MEM_FIELD_WORDS];
    int member_bitmap_len;

    if (start_ptr > 0) {
        add_member = TRUE;
    } else {
        add_member = FALSE;
    }

    member_bitmap_f = PIPE_MEMBER_BMPf;
    head_index_f = PIPE_BASE_PTRf;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (!((soc_feature(unit, soc_feature_channelized_switching)) &&
        BCM_GPORT_IS_SET(gport) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport))) {
        gport = port;
    }
#endif

    local_port = port;
    if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
        int aggid = TH_AGG_ID_INVALID;
        BCM_IF_ERROR_RETURN(bcm_th_port_to_aggid(unit, gport, &aggid));
        member_bitmap_index = aggid;
    } else {
        /* Get member bitmap index */
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
        if ((soc_feature(unit, soc_feature_channelized_switching)) &&
            BCM_GPORT_IS_SET(gport) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)) {
            int subport_num = 0;

            BCM_IF_ERROR_RETURN(
                _bcmi_coe_subport_physical_port_get(unit, port, &local_port));

            BCM_IF_ERROR_RETURN(
                _bcm_coe_subtag_subport_port_subport_num_get(unit,
                gport, NULL, &subport_num));

            mmu_port = subport_num;
        }  else
#endif
        {
            si = &SOC_INFO(unit);
            phy_port = si->port_l2p_mapping[port];
            mmu_port = si->port_p2m_mapping[phy_port];
        }

        member_bitmap_index = mmu_port % mmu_port_stride[unit];
    }

    member_bitmap_len = soc_mem_field_length(unit, MMU_REPL_GROUP_INFO_TBLm,
                                             member_bitmap_f);
    if (member_bitmap_index >= member_bitmap_len) {
        return BCM_E_PARAM;
    }

    /* Get old member bitmap and old head index */
    BCM_IF_ERROR_RETURN(_bcm_th_port_pipe_get(unit, local_port, &pipe));
    if (soc_feature(unit, soc_feature_split_repl_head_table)) {
        if (member_bitmap_index < 32) {
            repl_head_table = REPL_HEAD_TBL_SPLIT_PIPE(unit, 0, pipe);
        } else {
            repl_head_table = REPL_HEAD_TBL_SPLIT_PIPE(unit, 1, pipe);
        }
    } else {
        repl_head_table = REPL_HEAD_TBL_PIPE(unit, pipe);
    }
    repl_group_table = REPL_GROUP_INFO_TBL_PIPE(unit, pipe);

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, repl_group_table, MEM_BLOCK_ANY, 
                      repl_group, &repl_group_entry));
    sal_memset(fldbuf, 0, BCM_REPL_HEAD_MEMBER_WORD_MAX * sizeof(uint32));
    soc_mem_field_get(unit, repl_group_table, (uint32 *)&repl_group_entry,
            member_bitmap_f, fldbuf);
    for (i = 0; i < BCM_REPL_HEAD_MEMBER_WORD_MAX; i++) {
        old_member_bitmap[i] = fldbuf[i];
        new_member_bitmap[i] = old_member_bitmap[i];

        old_member_count += _shr_popcount(old_member_bitmap[i]);
    }

    old_head_index = soc_mem_field32_get(unit, repl_group_table,
            &repl_group_entry, head_index_f);

    if (add_member) {
        /* Update REPL_HEAD table */
        sal_memset(repl_head_entry, 0, sizeof(repl_head_entry));
        soc_mem_field32_set(unit, repl_head_table, repl_head_entry,
                            HEAD_PTRf, start_ptr);
        if (SHR_BITGET(old_member_bitmap, member_bitmap_index)) {
            /* Port is already a member of the group */
            member_id = 0;
            for (member=0; member<BCM_REPL_HEAD_MEMBER_MAX; member++) {
                if (!SHR_BITGET(old_member_bitmap, member)) {
                    continue;
                }

                if (member == member_bitmap_index) {
                    break;
                }
                member_id++;
            }
            SOC_IF_ERROR_RETURN
                (_bcm_td3_repl_head_tbl_write(unit, pipe,
                 (old_head_index + member_id), repl_head_entry));
            new_head_index = old_head_index;
        } else {
            SHR_BITSET(new_member_bitmap, member_bitmap_index);

            new_member_count = old_member_count + 1;
            BCM_IF_ERROR_RETURN(_bcm_th_repl_head_block_alloc(unit,
                        pipe, new_member_count, &new_head_index));
            old_member_id = 0;
            new_member_id = 0;
            for (member=0; member<BCM_REPL_HEAD_MEMBER_MAX; member++) {
                if (!SHR_BITGET(new_member_bitmap, member)) {
                    continue;
                }

                if (member == member_bitmap_index) {
                    SOC_IF_ERROR_RETURN
                        (_bcm_td3_repl_head_tbl_write(unit, pipe,
                                       (new_head_index + new_member_id),
                                       repl_head_entry));
                } else {
                    SOC_IF_ERROR_RETURN
                        (_bcm_td3_repl_head_tbl_read(unit, pipe,
                                      (old_head_index + old_member_id),
                                      old_repl_head_entry, NULL));
                    SOC_IF_ERROR_RETURN
                        (_bcm_td3_repl_head_tbl_write(unit, pipe,
                                       (new_head_index + new_member_id),
                                       old_repl_head_entry));
                    old_member_id++;
                }

                new_member_id++;
            }
        }

        /* Update REPL_GROUP entry */
        for (i = 0; i < BCM_REPL_HEAD_MEMBER_WORD_MAX; i++) {
            fldbuf[i] = new_member_bitmap[i];
        }
        soc_mem_field_set(unit, repl_group_table, (uint32 *)&repl_group_entry,
                member_bitmap_f, fldbuf);
        soc_mem_field32_set(unit, repl_group_table, &repl_group_entry,
                head_index_f, new_head_index);
        repl_group_table = REPL_GROUP_INFO_TBL_PIPE(unit, pipe);

        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, repl_group_table, MEM_BLOCK_ALL,
                           repl_group, &repl_group_entry));

        /* Update REPL_GROUP_INITIAL_COPY_COUNT entry */
        BCM_IF_ERROR_RETURN(_bcm_th_repl_initial_copy_count_set(unit,
                        repl_group, port, intf_count));

        /* Release old block of REPL_HEAD entries */
        if (old_member_count > 0 && old_head_index != new_head_index) {
            BCM_IF_ERROR_RETURN(_bcm_th_repl_head_block_free(unit,
                        pipe, old_head_index, old_member_count));
        }

    } else { /* Remove member from replication group */

        if (!(SHR_BITGET(old_member_bitmap, member_bitmap_index))) {

            /* Port is not a member. Nothing more to do. */
            return BCM_E_NONE;
        }

        /* Update REPL_HEAD table */
        new_member_count = old_member_count - 1;
        if (new_member_count > 0) {
            BCM_IF_ERROR_RETURN(_bcm_th_repl_head_block_alloc(unit,
                        pipe, new_member_count, &new_head_index));
            old_member_id = 0;
            new_member_id = 0;
            for (member=0; member<BCM_REPL_HEAD_MEMBER_MAX; member++) {
                if (!SHR_BITGET(old_member_bitmap, member)) {
                    continue;
                }
                if (member != member_bitmap_index) {
                    SOC_IF_ERROR_RETURN
                        (_bcm_td3_repl_head_tbl_read(unit, pipe,
                                      (old_head_index + old_member_id),
                                      old_repl_head_entry, NULL));
                    SOC_IF_ERROR_RETURN
                        (_bcm_td3_repl_head_tbl_write(unit, pipe,
                                       (new_head_index + new_member_id),
                                       old_repl_head_entry));
                    new_member_id++;
                }
                old_member_id++;
            }
        } else {
            new_head_index = 0;
        }

        /* Update REPL_GROUP entry */
        SHR_BITCLR(new_member_bitmap, member_bitmap_index);
        for (i = 0; i < BCM_REPL_HEAD_MEMBER_WORD_MAX; i++) {
            fldbuf[i] = new_member_bitmap[i];

        }
        soc_mem_field_set(unit, repl_group_table, (uint32 *)&repl_group_entry,
                member_bitmap_f, fldbuf);
        soc_mem_field32_set(unit, repl_group_table, &repl_group_entry,
                head_index_f, new_head_index);
        repl_group_table = REPL_GROUP_INFO_TBL_PIPE(unit, pipe);

        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, repl_group_table, MEM_BLOCK_ALL,
                           repl_group, &repl_group_entry));

        /* Update REPL_GROUP_INITIAL_COPY_COUNT entry. MMU spec
         * recommends setting the initial copy count to 1 for
         * ports that are being removed from a replication group.
         */
        BCM_IF_ERROR_RETURN(_bcm_th_repl_initial_copy_count_set(unit,
                    repl_group, port, 1));


        /* Release old block of REPL_HEAD entries */
        BCM_IF_ERROR_RETURN(_bcm_th_repl_head_block_free(unit,
                    pipe, old_head_index, old_member_count));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_th_repl_list_start_ptr_get
 * Purpose:
 *      Get replication list start pointer for given (repl_group, port).
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	repl_group - The replication group number.
 *	port       - Port.
 *	start_ptr  - (OUT) Replication list's start pointer to
 *	             REPL_LIST table.
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_th_repl_list_start_ptr_get(int unit, int repl_group, bcm_port_t port,
        int *start_ptr,  bcm_gport_t gport)
{
    soc_mem_t repl_group_table;
    soc_field_t member_bitmap_f, head_index_f; 
    soc_info_t *si;
    bcm_port_t phy_port, mmu_port;
    int member_bitmap_index;
    int pipe;
    mmu_repl_group_info_tbl_entry_t repl_group_entry;
    uint32 fldbuf[BCM_REPL_HEAD_MEMBER_WORD_MAX] = {0};
    int i;
    uint32 member_bitmap[BCM_REPL_HEAD_MEMBER_WORD_MAX] = {0};
    int member_id, member;
    int head_index;
    uint32 repl_head_entry[SOC_MAX_MEM_FIELD_WORDS];

    member_bitmap_f = PIPE_MEMBER_BMPf;
    head_index_f = PIPE_BASE_PTRf;


#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (!((soc_feature(unit, soc_feature_channelized_switching)) &&
        BCM_GPORT_IS_SET(gport) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport))) {
        gport = port;
    }
#endif


    if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
        int aggid = TH_AGG_ID_INVALID;
        BCM_IF_ERROR_RETURN(bcm_th_port_to_aggid(unit, gport, &aggid));
        member_bitmap_index = aggid;
    } else {

       /* Get member bitmap index */
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
        if ((soc_feature(unit, soc_feature_channelized_switching)) &&
            BCM_GPORT_IS_SET(gport) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)) {
            int subport_num = 0;

            BCM_IF_ERROR_RETURN(
                _bcmi_coe_subport_physical_port_get(unit, gport, &port));

            BCM_IF_ERROR_RETURN(
                _bcm_coe_subtag_subport_port_subport_num_get(unit,
                gport, NULL, &subport_num));

            mmu_port = subport_num;
        }  else
#endif
        {
            si = &SOC_INFO(unit);
            phy_port = si->port_l2p_mapping[port];
            mmu_port = si->port_p2m_mapping[phy_port];
        }
        member_bitmap_index = mmu_port % mmu_port_stride[unit];
    }

    /* Get member bitmap */
    BCM_IF_ERROR_RETURN(_bcm_th_port_pipe_get(unit, port, &pipe));

    repl_group_table = REPL_GROUP_INFO_TBL_PIPE(unit, pipe);

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, repl_group_table, MEM_BLOCK_ANY,
                      repl_group, &repl_group_entry));

    sal_memset(fldbuf, 0, BCM_REPL_HEAD_MEMBER_WORD_MAX * sizeof(uint32));
    soc_mem_field_get(unit, repl_group_table, (uint32 *)&repl_group_entry,
            member_bitmap_f, fldbuf);
    for (i = 0; i < BCM_REPL_HEAD_MEMBER_WORD_MAX; i++) {
        member_bitmap[i] = fldbuf[i];
    }

    /* Check if port is in member bitmap */
    if (!(SHR_BITGET(member_bitmap, member_bitmap_index))) {
        *start_ptr = 0;
        return BCM_E_NONE;
    }

    /* Get replication list start pointer */
    member_id = 0;
    for (member=0; member<BCM_REPL_HEAD_MEMBER_MAX; member++) {
        if (!(SHR_BITGET(member_bitmap, member))) {
            continue;
        }

        if (member == member_bitmap_index) {
            break;
        }
        member_id++;
    }
    head_index = member_id + soc_mem_field32_get(unit, repl_group_table,
            &repl_group_entry, head_index_f);
    SOC_IF_ERROR_RETURN
        (_bcm_td3_repl_head_tbl_read(unit, pipe,
                      head_index, repl_head_entry, (uint32 *)start_ptr));

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_th_repl_list_free
 * Purpose:
 *	Free the REPL_LIST entries in the HW list starting at start_ptr.
 * Parameters:
 *	unit      - StrataSwitch PCI device unit number.
 *	pipe      - Pipeline number.
 *	start_ptr - Replication list's start pointer to
 *	            REPL_LIST table.
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_th_repl_list_free(int unit, int pipe, int start_ptr)
{
    uint32 repl_list_entry[SOC_MAX_MEM_FIELD_WORDS];
    int	prev_repl_entry_ptr, repl_entry_ptr;
    soc_mem_t mem;

    mem = REPL_LIST_TBL(unit);
    prev_repl_entry_ptr = -1;
    repl_entry_ptr = start_ptr;

    while (repl_entry_ptr != prev_repl_entry_ptr) {
        SOC_IF_ERROR_RETURN(bcmi_repl_list_tbl_read(unit, &pipe,
            repl_entry_ptr, repl_list_entry));
        BCM_IF_ERROR_RETURN
            (_bcm_th_repl_list_entry_free(unit, pipe, repl_entry_ptr)); 
        prev_repl_entry_ptr = repl_entry_ptr;
        repl_entry_ptr = soc_mem_field32_get(unit, mem,
                repl_list_entry, NEXTPTRf);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_th_repl_list_write
 * Purpose:
 *	Write the replication list contained in intf_vec into REPL_LIST table.
 *      Return the start_ptr and total interface count.
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	pipe       - Pipeline number.
 *	start_ptr  - (OUT) Replication list's start pointer to
 *	             REPL_LIST table.
 *      intf_count - (OUT) Interface count.
 *	intf_vec   - (IN) Vector of interfaces.
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_th_repl_list_write(int unit, int pipe, int *start_ptr, int *intf_count,
                        SHR_BITDCL *intf_vec)
{
    int i;
    int remaining_count;
    int prev_repl_entry_ptr, repl_entry_ptr;
    uint32 msb_max, msb;
    uint32 ls_bits[2];
    int rv = BCM_E_NONE;
    int no_more_free_repl_entries;
    uint32 repl_list_entry[SOC_MAX_MEM_FIELD_WORDS];
    soc_mem_t mem;
/*    soc_mem_t repl_list_table;*/
    int max_rem_count = 5;

    *intf_count = 0;
    for (i = 0; i < _SHR_BITDCLSIZE(REPL_INTF_TOTAL(unit)); i++) {
        *intf_count += _shr_popcount(intf_vec[i]);
    }

    if (*intf_count == 0) {
        return BCM_E_NONE;
    }
/*    repl_list_table = REPL_LIST_TBL_PIPE(unit, pipe);*/

    mem = REPL_LIST_TBL(unit);
    remaining_count = *intf_count;
    prev_repl_entry_ptr = -1;
    msb_max = _SHR_BITDCLSIZE(REPL_INTF_TOTAL(unit)) / 2; /* 32 -> 64 */
    for (msb = 0; msb < msb_max; msb++) {
        ls_bits[0] = intf_vec[2 * msb + 0];
        ls_bits[1] = intf_vec[2 * msb + 1];
        if ((ls_bits[0] != 0) || (ls_bits[1] != 0)) {
            rv = _bcm_th_repl_list_entry_alloc(unit, pipe, &repl_entry_ptr);
            if (rv == BCM_E_RESOURCE) {
                no_more_free_repl_entries = TRUE;
            } else if (BCM_FAILURE(rv)) {
                return rv;
            } else {
                no_more_free_repl_entries = FALSE;
            }
            if (prev_repl_entry_ptr == -1) {
                if (no_more_free_repl_entries) {
                    return BCM_E_RESOURCE;
                }
                *start_ptr = repl_entry_ptr;
            } else {
                if (no_more_free_repl_entries) {
                    /* Terminate replication list */
                    soc_mem_field32_set(unit, mem, repl_list_entry,
                            NEXTPTRf, prev_repl_entry_ptr);
                } else {
                    soc_mem_field32_set(unit, mem, repl_list_entry,
                            NEXTPTRf, repl_entry_ptr);
                }
                SOC_IF_ERROR_RETURN(bcmi_repl_list_tbl_write(unit, &pipe,
                    prev_repl_entry_ptr, repl_list_entry));

/*                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, repl_list_table, MEM_BLOCK_ALL, 
                                   prev_repl_entry_ptr, &repl_list_entry));*/
                if (no_more_free_repl_entries) {
                    /* Free the list already written */
                    _bcm_th_repl_list_free(unit, pipe, *start_ptr);
                    return BCM_E_RESOURCE;
                }
            } 
            prev_repl_entry_ptr = repl_entry_ptr;

            sal_memset(repl_list_entry, 0, SOC_MAX_MEM_FIELD_WORDS *sizeof(uint32));
            soc_mem_field32_set(unit, mem, repl_list_entry, MSB_VLANf, msb);
            soc_mem_field_set(unit, mem, repl_list_entry, LSB_VLAN_BMf, ls_bits);

            remaining_count -= (_shr_popcount(ls_bits[0]) +
                    _shr_popcount(ls_bits[1]));
            if (remaining_count > max_rem_count) {
                /* Set the RMNG_REPS field to all zeroes */
                soc_mem_field32_set(unit, mem, repl_list_entry, RMNG_REPSf, 0);
            } else if (remaining_count > 0) {
                soc_mem_field32_set(unit, mem, repl_list_entry, RMNG_REPSf, remaining_count);
            } else { /* No more intefaces left to be written */
                break;
            }
        }
    }

    if (prev_repl_entry_ptr > 0) {
        /* Write final entry */
        soc_mem_field32_set(unit, mem, repl_list_entry, NEXTPTRf, prev_repl_entry_ptr);
        SOC_IF_ERROR_RETURN(bcmi_repl_list_tbl_write(unit, &pipe,
            prev_repl_entry_ptr, repl_list_entry));
/*
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, repl_list_table, MEM_BLOCK_ALL, 
                           prev_repl_entry_ptr, &repl_list_entry));
*/
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_th_repl_list_compare
 * Purpose:
 *	Compare replication list starting at given pointer to
 *	the interface list contained in intf_vec.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number.
 *	pipe - Pipeline number.
 *	start_ptr - Replication list's start pointer to REPL_LIST table.
 *	intf_vec - Vector of interfaces.
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_th_repl_list_compare(int unit, int pipe, int start_ptr,
                          SHR_BITDCL *intf_vec)
{
    uint32		msb, hw_msb, msb_max;
    uint32		ls_bits[2], hw_ls_bits[2];
    uint32      repl_list_entry[SOC_MAX_MEM_FIELD_WORDS];
    soc_mem_t   mem;
    int	        repl_entry_ptr, prev_repl_entry_ptr;

    prev_repl_entry_ptr = -1;
    repl_entry_ptr = start_ptr;
    msb_max = _SHR_BITDCLSIZE(REPL_INTF_TOTAL(unit)) / 2; /* 32 -> 64 */
    mem = REPL_LIST_TBL(unit);

    for (msb = 0; msb < msb_max; msb++) {
        ls_bits[0] = intf_vec[2 * msb + 0];
        ls_bits[1] = intf_vec[2 * msb + 1];
        if ((ls_bits[0] != 0) || (ls_bits[1] != 0)) {
            if (repl_entry_ptr == prev_repl_entry_ptr) { /* HW list end */
                return BCM_E_NOT_FOUND;
            }
            SOC_IF_ERROR_RETURN(bcmi_repl_list_tbl_read(unit, &pipe,
                repl_entry_ptr, repl_list_entry));

/*
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, repl_list_table, MEM_BLOCK_ANY, 
                              repl_entry_ptr, repl_list_entry));
*/
            hw_msb = soc_mem_field32_get(unit, mem,
                    repl_list_entry, MSB_VLANf);
            soc_mem_field_get(unit, mem, repl_list_entry,
                    LSB_VLAN_BMf, hw_ls_bits);
            if ((hw_msb != msb) || (ls_bits[0] != hw_ls_bits[0]) ||
                    (ls_bits[1] != hw_ls_bits[1])) {
                return BCM_E_NOT_FOUND;
            }
            prev_repl_entry_ptr = repl_entry_ptr;
            repl_entry_ptr = soc_mem_field32_get(unit, mem,
                    repl_list_entry, NEXTPTRf);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_th_repl_intf_vec_construct
 * Purpose:
 *	Construct replication interface vector.
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	port       - Port.
 *	if_count   - Number of interfaces in replication list.
 *      if_array   - (IN) List of interface numbers.
 *      is_l3      - (IN) Indicates if interfaces are IPMC interfaces.
 *      check_port - If if_array consists of L3 interfaces, this parameter 
 *                   controls whether to check the given port is a member
 *                   in each L3 interface's VLAN. This check should be  
 *                   disabled when not needed, in order to improve
 *                   performance.
 *      intf_vec   - (OUT) Vector of interfaces.
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_th_repl_intf_vec_construct(int unit, bcm_port_t port, int if_count,
        bcm_if_t *if_array, int is_l3, int check_port, SHR_BITDCL *intf_vec)
{
    int if_num;
    bcm_l3_intf_t l3_intf;
    pbmp_t pbmp, ubmp;
    uint32 nh_flags;
    bcm_l3_egress_t nh_info;
    int nh_index;
    egr_l3_next_hop_entry_t egr_nh;
    int mac_oui, mac_non_oui;
    bcm_mac_t rbridge_mac;
    soc_field_t ent_type_fld = ENTRY_TYPEf;
    uint32 ent_type_val = 7;

#if defined(BCM_HGPROXY_COE_SUPPORT)
    bcm_port_t port_in = port;
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    egr_l3_next_hop_2_entry_t egr_nh2;
    if(SOC_IS_TOMAHAWK3(unit)) {
        ent_type_val = 3;
    }
#endif


#if defined(BCM_HGPROXY_COE_SUPPORT)
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
            BCM_IF_ERROR_RETURN(
                _bcmi_coe_subport_physical_port_get(unit, port, &port));
    }
#endif

    if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, DATA_TYPEf)) {
        ent_type_fld = DATA_TYPEf;
    }

    for (if_num = 0; if_num < if_count; if_num++) {
        if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, if_array[if_num])) {
            /* L3 interface is used */

            if (if_array[if_num] > soc_mem_index_max(unit, EGR_L3_INTFm)) {
                return BCM_E_PARAM;
            }

            if (check_port) {
                bcm_l3_intf_t_init(&l3_intf);
                l3_intf.l3a_intf_id = if_array[if_num];
                BCM_IF_ERROR_RETURN(bcm_esw_l3_intf_get(unit, &l3_intf));
                if (!SOC_IS_TRIDENT3X(unit)) {
                    BCM_IF_ERROR_RETURN(bcm_esw_vlan_port_get(unit,
                                        l3_intf.l3a_vid, &pbmp, &ubmp));
                    if (!BCM_PBMP_MEMBER(pbmp, port)) {
                        return BCM_E_PARAM;
                    }
                }
            }

            /* Get a next hop index if the L3 inteface is not already
             * associated with one.
             */
            if (is_l3) { /* L3 interface is being added to IPMC group */
                if (REPL_L3_INTF_NEXT_HOP_IPMC(unit, if_array[if_num]) == -1) {
                    bcm_l3_egress_t_init(&nh_info);
                    nh_flags = _BCM_L3_SHR_MATCH_DISABLE |
                        _BCM_L3_SHR_WRITE_DISABLE;
                    BCM_IF_ERROR_RETURN
                        (bcm_xgs3_nh_add(unit, nh_flags, &nh_info, &nh_index));

                    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            ent_type_fld, ent_type_val);
                    if(soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                    L3MC__INTF_NUMf)) {
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3MC__INTF_NUMf, if_array[if_num]);
                    }
                    if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                L3MC__HG_MODIFY_ENABLEf)) {
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__HG_MODIFY_ENABLEf, 0x1);
                    }

                    if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                L3MC__L2_MC_DA_DISABLEf)) {
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__L2_MC_DA_DISABLEf, 1);
                    }
                    if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                L3MC__L2_MC_SA_DISABLEf)) {
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__L2_MC_SA_DISABLEf, 1);
                    }
                    if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                L3MC__L2_MC_VLAN_DISABLEf)) {
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__L2_MC_VLAN_DISABLEf, 1);
                    }

#if defined(BCM_HGPROXY_COE_SUPPORT)
                    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
                        soc_feature(unit, soc_feature_channelized_switching)) &&
                        BCM_GPORT_IS_SET(port_in) &&
                        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port_in)) {
                        bcm_module_t mod_out;
                        bcm_port_t port_out;
                        bcm_trunk_t trunk_id;
                        int local_id;

                        BCM_IF_ERROR_RETURN(
                            _bcm_esw_gport_resolve(unit, port_in,
                                                   &mod_out, &port_out,
                                                   &trunk_id, &local_id));
                        if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                                L3MC__USE_MAC_DA_PROFILEf)) {
                            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                                L3MC__USE_MAC_DA_PROFILEf,
                                                TRUE);
                        }
                        if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                                L3MC__HG_MC_DST_PORT_NUMf)) {
                            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                                L3MC__HG_MC_DST_PORT_NUMf,
                                                port_out);
                        }
                        if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                                L3MC__HG_MC_DST_MODIDf)) {
                            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                                L3MC__HG_MC_DST_MODIDf,
                                                mod_out);
                        }
                    }
#endif
                    SOC_IF_ERROR_RETURN(soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                                MEM_BLOCK_ALL, nh_index, &egr_nh));

#ifdef BCM_TOMAHAWK3_SUPPORT
                    if(SOC_IS_TOMAHAWK3(unit)) {
                        SOC_IF_ERROR_RETURN(
                                READ_EGR_L3_NEXT_HOP_2m(unit, MEM_BLOCK_ANY,
                                    nh_index, &egr_nh2) );

                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOP_2m, &egr_nh2,
                            INTF_NUMf, if_array[if_num]);

                        SOC_IF_ERROR_RETURN(soc_mem_write(unit, EGR_L3_NEXT_HOP_2m,
                                    MEM_BLOCK_ALL, nh_index, &egr_nh2));
                    }
#endif
                    REPL_L3_INTF_NEXT_HOP_IPMC(unit, if_array[if_num]) =
                        nh_index;
                }
                SHR_BITSET(intf_vec,
                    REPL_L3_INTF_NEXT_HOP_IPMC(unit, if_array[if_num]));

            } else { /* L3 interface is being added to Trill group */
                if (REPL_L3_INTF_NEXT_HOP_TRILL(unit, if_array[if_num]) == -1) {
                    bcm_l3_egress_t_init(&nh_info);
                    nh_flags = _BCM_L3_SHR_MATCH_DISABLE |
                        _BCM_L3_SHR_WRITE_DISABLE;
                    BCM_IF_ERROR_RETURN
                        (bcm_xgs3_nh_add(unit, nh_flags, &nh_info, &nh_index));

                    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
                    if (SOC_IS_TD2_TT2(unit)) {
                        /*
                         * TD2 needs to set the entry_type to 0x7 (L3MC) 
                         * in order to use trill_all_rbridges_macda as the DA 
                         * in TRILL header 
                         */
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                ent_type_fld, ent_type_val);
                    } else
                    {
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                ent_type_fld, 0);
                    }
#ifdef BCM_TOMAHAWK3_SUPPORT
                    if(SOC_IS_TOMAHAWK3(unit)) {
                        SOC_IF_ERROR_RETURN(
                                READ_EGR_L3_NEXT_HOP_2m(unit, MEM_BLOCK_ANY,
                                    nh_index, &egr_nh2) );
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOP_2m, &egr_nh2,
                            INTF_NUMf, if_array[if_num]);

                        SOC_IF_ERROR_RETURN(soc_mem_write(unit, EGR_L3_NEXT_HOP_2m,
                                    MEM_BLOCK_ALL, nh_index, &egr_nh2));
                    } else
#endif
                    {
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3__INTF_NUMf, if_array[if_num]);
                    }
                    /* Get and Set rbridge mac */
                    BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit,
                            bcmSwitchTrillBroadcastDestMacOui, &mac_oui)); 
                    BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit,
                            bcmSwitchTrillBroadcastDestMacNonOui, &mac_non_oui)); 
                    rbridge_mac[0] = (mac_oui >> 16) & 0xff;
                    rbridge_mac[1] = (mac_oui >> 8) & 0xff;
                    rbridge_mac[2] = mac_oui & 0xff;
                    rbridge_mac[3] = (mac_non_oui >> 16) & 0xff;
                    rbridge_mac[4] = (mac_non_oui >> 8) & 0xff;
                    rbridge_mac[5] = mac_non_oui & 0xff;
                    soc_mem_mac_addr_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3__MAC_ADDRESSf, rbridge_mac);

                    SOC_IF_ERROR_RETURN(soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                                MEM_BLOCK_ALL, nh_index, &egr_nh));

                    REPL_L3_INTF_NEXT_HOP_TRILL(unit, if_array[if_num]) =
                        nh_index;
                }
                SHR_BITSET(intf_vec,
                    REPL_L3_INTF_NEXT_HOP_TRILL(unit, if_array[if_num]));
            }
        } else {
            /* Next hop is used */
            SHR_BITSET(intf_vec,
                    if_array[if_num] - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_th_ipmc_cut_through_eligibility_set
 * Purpose:
 *	Check cut-through eligibility for a given replication group.
 *  These functions are not extended for subports.
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	repl_group - The replication group number.
 *	port       - Port to assign replication list.
 * Returns:
 *	BCM_E_XXX
 */
int
_bcm_th_ipmc_cut_through_eligibility_set(int unit, int repl_group)
{
    int rv = BCM_E_NONE;
    int i, pipe, member_bitmap_width;
    int member = 0;
    soc_mem_t repl_group_table;
    soc_field_t member_bitmap_f;
    mmu_repl_group_info_tbl_entry_t repl_group_entry;
    soc_pbmp_t old_member_bitmap[4];
    soc_pbmp_t new_member_bitmap[4];
    uint32 fldbuf[SOC_PBMP_WORD_MAX];
    soc_info_t *si;
    soc_pbmp_t l3_pbmp;
    bcm_port_t mmu_port, phy_port, logical_port;
    int cut_through_eligible = TRUE;
    ipmc_entry_t l3_ipmc_entry;
#if defined(BCM_TRIDENT3_SUPPORT)
    bcm_multicast_t  ipmc_group = 0;
#endif

    member_bitmap_f = PIPE_MEMBER_BMPf;
    member_bitmap_width = soc_mem_field_length(unit, MMU_REPL_GROUP_INFO_TBLm,
                                               member_bitmap_f);

    si = &SOC_INFO(unit);
    SOC_PBMP_CLEAR(l3_pbmp);

    for (pipe = 0; pipe < NUM_PIPES(unit); pipe++) {
        SOC_PBMP_CLEAR(old_member_bitmap[pipe]);
        SOC_PBMP_CLEAR(new_member_bitmap[pipe]);
        /* Read pipeline's replication group table */
        repl_group_table = REPL_GROUP_INFO_TBL_PIPE(unit, pipe);

        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, repl_group_table, MEM_BLOCK_ANY,
                          repl_group, &repl_group_entry));
             sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
             soc_mem_field_get(unit, repl_group_table, (uint32 *)&repl_group_entry,
                               member_bitmap_f, fldbuf);
             for (i = 0; (i < SOC_PBMP_WORD_MAX) && (i < BCM_REPL_HEAD_MEMBER_WORD_MAX); i++) {
                SOC_PBMP_WORD_SET(old_member_bitmap[pipe], i, fldbuf[i]);
             }
            if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
                int aggid = TH_AGG_ID_INVALID;
                PBMP_ITER(PBMP_PIPE(unit, pipe), logical_port) {
                    rv = bcm_th_port_to_aggid(unit, logical_port, &aggid);
                    if (rv == BCM_E_NONE) {
                        if (SOC_PBMP_MEMBER(old_member_bitmap[pipe], aggid)) {
                            SOC_PBMP_PORT_ADD(new_member_bitmap[pipe], logical_port);
                        }
                    }
                }
            } else {
                for (i = 0; i < member_bitmap_width; i++) {
                    if (SOC_PBMP_MEMBER(old_member_bitmap[pipe], i)) {
                        mmu_port = i + mmu_port_stride[unit] * pipe;
                        phy_port = si->port_m2p_mapping[mmu_port];
                        logical_port = si->port_p2l_mapping[phy_port];
                        SOC_PBMP_PORT_ADD(new_member_bitmap[pipe], logical_port);
                    }
                }
            }
            SOC_PBMP_OR(l3_pbmp, new_member_bitmap[pipe]);
    }
#if defined(BCM_TRIDENT3_SUPPORT)
    /* For VXLAN IPMC, cut-through is always disabled.
    *  The IP header for the tunnel needs the entire length of
    *  the IP payload. This implies that the MMU must store the
    *  entire packet, determining its length and then forward it
    *  to the EP.
    */
    (void)_bcm_tr_multicast_ipmc_group_type_get(unit, repl_group, &ipmc_group);
    if (SOC_IS_TRIDENT3X(unit) &&
        (_BCM_MULTICAST_TYPE_VXLAN == _BCM_MULTICAST_TYPE_GET(ipmc_group))){
        cut_through_eligible = FALSE;
    } else
#endif
    {
    SOC_PBMP_ITER(l3_pbmp, member) {

        /*
         * Packets with a single logical replication per port are
         * eligible for cut-through consideration. If the IPMC group has more
         * than 1 switch copy per egress port, DO_NOT_CUT_THROUGH bit in
         * L3_IPMC table must be set for the ipmc group.
         */
        /* Coverity issue ignored:
         * Maximum port number used in l3_pbmp
         * should never exceed SOC_MAX_NUM_PP_PORTS */
        /* coverity[overrun-local : FALSE] */         
        if (REPL_PORT_GROUP_INTF_COUNT(unit, member, repl_group) > 1) {
            cut_through_eligible = FALSE;
            break;
        }
    }
    }
    
    soc_mem_lock(unit, L3_IPMCm);
    rv = READ_L3_IPMCm(unit, MEM_BLOCK_ANY, repl_group, &l3_ipmc_entry);
    if (BCM_SUCCESS(rv)) {
        if (!cut_through_eligible) {
            soc_L3_IPMCm_field32_set(unit, &l3_ipmc_entry, DO_NOT_CUT_THROUGHf,
                                     !cut_through_eligible);
        } else {
            /* This is needed to set the mode back to cut-through when
             * intf_count <= 1 after bcm_th_ipmc_egress_intf_delete is called.
             */
            soc_L3_IPMCm_field32_set(unit, &l3_ipmc_entry, DO_NOT_CUT_THROUGHf,
                                     !cut_through_eligible);
        }
        rv = WRITE_L3_IPMCm(unit, MEM_BLOCK_ALL, repl_group, &l3_ipmc_entry);
    }
    soc_mem_unlock(unit, L3_IPMCm);
 
    return rv;

}

/*
 * Function:
 *	bcm_th_ipmc_egress_intf_set
 * Purpose:
 *	Assign set of egress interfaces to port's replication list for chosen
 *	replication group.
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	repl_group - The replication group number.
 *	port       - Port to assign replication list.
 *	if_count   - Number of interfaces in replication list.
 *      if_array   - (IN) List of interface numbers.
 *      is_l3      - (IN) Indicates if multicast group is of type IPMC.
 *      check_port - (IN) If if_array consists of L3 interfaces, this parameter 
 *                        controls whether to check the given port is a member
 *                        in each L3 interface's VLAN. This check should be  
 *                        disabled when not needed, in order to improve
 *                        performance.
 * Returns:
 *	BCM_E_XXX
 */
int
bcm_th_ipmc_egress_intf_set(int unit, int repl_group, bcm_port_t port,
                            int if_count, bcm_if_t *if_array, int is_l3,
                            int check_port)
{
    int rv = BCM_E_NONE;
    int pipe;
    int set_repl_list;
    int prev_start_ptr, list_start_ptr=0;
    int alloc_size;
    SHR_BITDCL *intf_vec = NULL;
    int repl_hash;
    _bcm_repl_list_info_t *rli_current, *rli_prev;
    int intf_count;
    int new_repl_list = FALSE;
    bcm_port_t port_in = port;
    bcm_port_t port_num = port;

    REPL_INIT(unit);
    REPL_GROUP_ID(unit, repl_group);
#if defined(BCM_HGPROXY_COE_SUPPORT)
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
        int i=0;

        for (; i<if_count; i++) {
            if(!(BCM_XGS3_L3_EGRESS_IDX_VALID(unit, if_array[i]) ||
                BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, if_array[i]))) {
                return BCM_E_CONFIG;
            }
        }

        BCM_IF_ERROR_RETURN(
            _bcmi_coe_subport_physical_port_get(unit, port, &port));

        BCM_IF_ERROR_RETURN(
            _bcm_coe_subtag_subport_port_subport_num_get(unit,
            port_in, NULL, (int *)&port_num));



    }
#endif


    if (!IS_CPU_PORT(unit, port)) {
        REPL_PORT_CHECK(unit, port);
    }
    BCM_IF_ERROR_RETURN(_bcm_th_port_pipe_get(unit, port, &pipe));

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        if (if_count > TH3_IPMC_MAX_INTF_COUNT) {
            return BCM_E_PARAM;
        }
    }
#endif
    if (if_count > REPL_INTF_TOTAL(unit)) {
        return BCM_E_PARAM;
    } else if (if_count > 0) {
        set_repl_list = TRUE;
    } else {
        set_repl_list = FALSE;
    }

    REPL_LOCK(unit);
    if (REPL_PORT_GROUP_INFO(unit, port) == NULL) {
        REPL_UNLOCK(unit);
        return BCM_E_PORT;
    }

    if ((if_count == 0) &&
        (REPL_PORT_GROUP_INTF_COUNT(unit, port_num, repl_group) == 0)) {
        REPL_UNLOCK(unit);
        return BCM_E_NONE;
    }

#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        rv = bcm_th3_repl_list_start_ptr_get(unit, repl_group, port,
            &prev_start_ptr);
        pipe = 0;
    } else
#endif
    {
    /* Get current replication list start pointer of (repl_group, port) */
    rv = _bcm_th_repl_list_start_ptr_get(unit, repl_group, port,
            &prev_start_ptr, port_in);
    }
    if (BCM_FAILURE(rv)) {
        goto intf_set_done;
    }

    if (set_repl_list) { 
        /* Set new replication list for given (repl_group, port) */

        /* Interface validation and vector construction */
        alloc_size = SHR_BITALLOCSIZE(REPL_INTF_TOTAL(unit));
        intf_vec = sal_alloc(alloc_size, "Repl interface vector");
        if (intf_vec == NULL) {
            rv = BCM_E_MEMORY;
            goto intf_set_done;
        }
        sal_memset(intf_vec, 0, alloc_size);
        rv = _bcm_th_repl_intf_vec_construct(unit, port_in, if_count, if_array,
                is_l3, check_port, intf_vec);
        if (BCM_FAILURE(rv)) {
            goto intf_set_done;
        }

        /* Search for matching replication list */
        repl_hash = _shr_crc32b(0, (uint8 *)intf_vec, REPL_INTF_TOTAL(unit));
        for (rli_current = REPL_LIST_INFO(unit, pipe); rli_current != NULL;
                rli_current = rli_current->next) {
            if (repl_hash == rli_current->hash) {
#ifdef BCM_TOMAHAWK3_SUPPORT
                if(SOC_IS_TOMAHAWK3(unit)) {
                    rv = _bcm_th3_repl_list_compare(unit, pipe,
                                        rli_current->index, intf_vec);

                } else
#endif
                {
                rv = _bcm_th_repl_list_compare(unit, pipe, rli_current->index,
                        intf_vec);
                }
                if (rv == BCM_E_NOT_FOUND) {
                    continue; /* Not a match */
                } else if (BCM_FAILURE(rv)) {
                    goto intf_set_done; 
                } else {
                    break; /* Match */
                }
            }
        }

        if (rli_current != NULL) {
            /* Found a match */
            if (prev_start_ptr == rli_current->index) {
                /* (repl_group, port) already points to this list, so done */
                rv = BCM_E_NONE;
                goto intf_set_done;
            } else {
                list_start_ptr = rli_current->index;
                intf_count = rli_current->list_size;
            }
        } else {
            /* Not a match, make a new chain */
#ifdef BCM_TOMAHAWK3_SUPPORT
            if(SOC_IS_TOMAHAWK3(unit)) {
                rv = _bcm_th3_repl_list_write(unit, pipe, &list_start_ptr,
                    &intf_count, intf_vec);
            } else
#endif
            {
            rv = _bcm_th_repl_list_write(unit, pipe, &list_start_ptr,
                    &intf_count, intf_vec);
            }
            if (BCM_FAILURE(rv)) {
                goto intf_set_done;
            }

            if (intf_count > 0) {
                /* Update data structures */
                alloc_size = sizeof(_bcm_repl_list_info_t);
                rli_current = sal_alloc(alloc_size, "IPMC repl list info");
                if (rli_current == NULL) {
                    /* Release list */
                    _bcm_th_repl_list_free(unit, pipe, list_start_ptr);
                    rv = BCM_E_MEMORY;
                    goto intf_set_done;
                }
                sal_memset(rli_current, 0, alloc_size);
                rli_current->index = list_start_ptr;
                rli_current->hash = repl_hash;
                rli_current->list_size = intf_count;
                rli_current->next = REPL_LIST_INFO(unit, pipe);
                REPL_LIST_INFO(unit, pipe) = rli_current;
                new_repl_list = TRUE;
            } else {
                rv = BCM_E_INTERNAL;
                goto intf_set_done;
            }
        }

        /* Update replication list start pointer */
#ifdef BCM_TOMAHAWK3_SUPPORT
        if  (SOC_IS_TOMAHAWK3(unit)) {
            rv = bcm_th3_repl_list_start_ptr_set(unit, repl_group, port,
                list_start_ptr, intf_count);

        } else

#endif
        {
        rv = _bcm_th_repl_list_start_ptr_set(unit, repl_group, port,
                list_start_ptr, intf_count, port_in);
        }
        if (BCM_FAILURE(rv)) {
            if (new_repl_list) {
                _bcm_th_repl_list_free(unit, pipe, list_start_ptr);
                REPL_LIST_INFO(unit, pipe) = rli_current->next;
                sal_free(rli_current);
            }
            goto intf_set_done;
        }
        (rli_current->refcount)++;
        REPL_PORT_GROUP_INTF_COUNT(unit, port_num, repl_group) = intf_count;

    } else { 
        /* Delete replication list for given (repl_group, port) */

        if (prev_start_ptr != 0) {
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                rv = bcm_th3_repl_list_start_ptr_set(unit, repl_group,
                                                                 port, 0, 0);

            } else
#endif
            {
                rv = _bcm_th_repl_list_start_ptr_set(unit, repl_group,
                                                                port, 0, 0, port_in);
            }
            if (BCM_FAILURE(rv)) {
                goto intf_set_done;
            }
        }
        REPL_PORT_GROUP_INTF_COUNT(unit, port_num, repl_group) = 0;
    }

    /* Delete old replication list */
    if (prev_start_ptr != 0) {
        rli_prev = NULL;
        for (rli_current = REPL_LIST_INFO(unit, pipe); rli_current != NULL;
                rli_current = rli_current->next) {
            if (prev_start_ptr == rli_current->index) {
                (rli_current->refcount)--;
                if (rli_current->refcount == 0) {
                    /* Free these linked list entries */
                    if (rli_prev == NULL) {
                        REPL_LIST_INFO(unit, pipe) = rli_current->next;
                    } else {
                        rli_prev->next = rli_current->next;
                    }
#ifdef BCM_TOMAHAWK3_SUPPORT
                    /* the repl list is first put into a recycle candidate pool */
                    if(SOC_IS_TOMAHAWK3(unit)) {

                        rli_current->next = _repl_list_recycle_array[unit]->next;
                        _repl_list_recycle_array[unit]->next = rli_current;
                        (_repl_list_recycle_array[unit]->list_size)++;

                    } else
#endif
                    {
                        _bcm_th_repl_list_free(unit, pipe, prev_start_ptr);
                    sal_free(rli_current);
                }
                }
                break;
            }
            rli_prev = rli_current;
        }
    }
#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        goto intf_set_done;
    }
#endif
    /*
     * Packets with a single logical replication per port are 
     * eligible for cut-through consideration. If the IPMC group has more
     * than 1 switch copy per egress port, DO_NOT_CUT_THROUGH bit in 
     * L3_IPMC table must be set for the ipmc group.
     */     
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (!soc_feature(unit, soc_feature_channelized_switching))
#endif
    {

        rv = _bcm_th_ipmc_cut_through_eligibility_set(unit, repl_group);
    }

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (BCM_SUCCESS(rv)) {
        if ((soc_feature(unit, soc_feature_subport_forwarding_support)) &&
            BCM_GPORT_IS_SET(port_in) &&
                _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port_in)) {

            rv = bcmi_subport_multicast_lc_pbm_set(unit, repl_group, port_in, 1);
            if (BCM_FAILURE(rv)) {
                goto intf_set_done;
            }
        }
        if (soc_feature(unit, soc_feature_repl_head_icc)) {
            rv = bcmi_fb6_repl_head_icc_update(unit, repl_group, pipe);
        }

    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */


intf_set_done:

    REPL_UNLOCK(unit);

    if (intf_vec != NULL) {
        sal_free(intf_vec);
    }

    return rv;
}

/*
 * Function:
 *	bcm_th_ipmc_egress_intf_get
 * Purpose:
 *	Retrieve set of egress interfaces to port's replication list for chosen
 *	replication group.
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	repl_group - The replication group number.
 *	port       - Port from which to get replication list.
 *	if_max     - Maximum number of interfaces in replication list.
 *      if_array   - (OUT) List of interface numbers.
 *	if_count   - (OUT) Number of interfaces in replication list.
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *      If the input parameter if_max = 0, return in the output parameter
 *      if_count the total number of interfaces in the specified multicast 
 *      group's replication list.
 */
int
bcm_th_ipmc_egress_intf_get(int unit, int repl_group, bcm_port_t port,
                            int if_max, bcm_if_t *if_array, int *if_count)
{
    int rv = BCM_E_NONE;
    int pipe;
    int prev_repl_entry_ptr, repl_entry_ptr;
    int intf_count;
    int l3_intf, intf_base;
    int ls_pos;
    uint32 ls_bits[2];
    uint32 repl_list_entry[SOC_MAX_MEM_FIELD_WORDS];
    int next_hop_index;
    int entry_type;
    egr_l3_next_hop_entry_t egr_nh;
#ifdef BCM_TOMAHAWK3_SUPPORT
    egr_l3_next_hop_2_entry_t egr_nh2;
#endif
    soc_mem_t mem;
    soc_field_t ent_type_fld = ENTRY_TYPEf;
    bcm_port_t port_in = port;
    uint32 port_num = port;

    mem = REPL_LIST_TBL(unit);
    if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, DATA_TYPEf)) {
        ent_type_fld = DATA_TYPEf;
    }

#if defined(BCM_HGPROXY_COE_SUPPORT)
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
            BCM_IF_ERROR_RETURN(
                _bcmi_coe_subport_physical_port_get(unit, port, &port));

        BCM_IF_ERROR_RETURN(
            _bcm_coe_subtag_subport_port_subport_num_get(unit,
            port_in, NULL, (int *)&port_num));

    }
#endif

    REPL_INIT(unit);
    REPL_GROUP_ID(unit, repl_group);
    if (!IS_CPU_PORT(unit, port)) {
        REPL_PORT_CHECK(unit, port);
    }
    BCM_IF_ERROR_RETURN(_bcm_th_port_pipe_get(unit, port, &pipe));

    if (if_max < 0) {
        return BCM_E_PARAM;
    } else if (if_max > 0 && NULL == if_array) {
        return BCM_E_PARAM;
    }

    if (NULL == if_count) {
        return BCM_E_PARAM;
    }

    REPL_LOCK(unit);
    if (REPL_PORT_GROUP_INFO(unit, port) == NULL) {
        REPL_UNLOCK(unit);
        return BCM_E_PORT;
    }

    if (REPL_PORT_GROUP_INTF_COUNT(unit, port_num, repl_group) == 0) {
        *if_count = 0;
        REPL_UNLOCK(unit);
        return BCM_E_NONE;
    }

#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        rv = bcm_th3_repl_list_start_ptr_get(unit, repl_group, port,
                                                        &repl_entry_ptr);
    } else
#endif
    {
    rv = _bcm_th_repl_list_start_ptr_get(unit, repl_group, port,
            &repl_entry_ptr, port_in);
    }
    if (BCM_FAILURE(rv)) {
        goto intf_get_done;
    }

    prev_repl_entry_ptr = -1;
    intf_count = 0;
    while (repl_entry_ptr != prev_repl_entry_ptr) {
        rv = bcmi_repl_list_tbl_read(unit, &pipe,
                repl_entry_ptr, repl_list_entry);
/*
        rv = soc_mem_read(unit, repl_list_table, MEM_BLOCK_ANY, 
                          repl_entry_ptr, &repl_list_entry);
*/
        if (BCM_FAILURE(rv)) {
            goto intf_get_done;
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        if(SOC_IS_TOMAHAWK3(unit)) {
            uint32 nh_mode;

            nh_mode = soc_mem_field32_get(unit, mem, repl_list_entry,
                            MODEf);
            if(nh_mode) {
                /* sparse mode */
                int i;
                for(i = 0; i < 4; i++) {
                    rv = _bcm_th3_sparse_mode_egress_intf_get(unit,
                            repl_entry_ptr, i, if_array, &intf_count);
                    if(BCM_FAILURE(rv)) {
                        goto intf_get_done;
                    }
                    if (intf_count == if_max) {
                        *if_count = intf_count;
                        goto intf_get_done;
                    }

                }
                prev_repl_entry_ptr = repl_entry_ptr;
                repl_entry_ptr = soc_mem_field32_get(unit, mem,
                        repl_list_entry, NEXTPTRf);
                if (intf_count >= REPL_PORT_GROUP_INTF_COUNT(unit,
                                                    port, repl_group)) {
                    break;
                }

                continue;
            }
        }
#endif
        /* Each MSB represents 64 entries in LSB bitmap */
        intf_base = soc_mem_field32_get(unit, mem, repl_list_entry,
                MSB_VLANf) * 64;
        soc_mem_field_get(unit, mem, repl_list_entry,
                LSB_VLAN_BMf, ls_bits);

        for (ls_pos = 0; ls_pos < 64; ls_pos++) {
            if (ls_bits[ls_pos / 32] & (1 << (ls_pos % 32))) {
                if (if_max == 0) {
                    intf_count++;
                } else {
                    next_hop_index = intf_base + ls_pos;
                    if_array[intf_count] = next_hop_index +
                        BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);

                    /* Check if next hop index corresponds to
                     * an IPMC or Trill interface.
                     */
                    rv = READ_EGR_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY,
                            next_hop_index, &egr_nh);
                    if (BCM_FAILURE(rv)) {
                        goto intf_get_done;
                    }
#ifdef BCM_TOMAHAWK3_SUPPORT
                    if (SOC_IS_TOMAHAWK3(unit)) {
                        rv = READ_EGR_L3_NEXT_HOP_2m(unit, MEM_BLOCK_ANY,
                                next_hop_index, &egr_nh2);
                        if (BCM_FAILURE(rv)) {
                            goto intf_get_done;
                        }
                    }
#endif
                    entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                            &egr_nh, ent_type_fld);
                    if (entry_type == 0) {
#ifdef BCM_TOMAHAWK3_SUPPORT
                        if (SOC_IS_TOMAHAWK3(unit)) {
                            l3_intf = soc_EGR_L3_NEXT_HOP_2m_field32_get(unit,
                                &egr_nh2, INTF_NUMf);

                        } else
#endif
                        {
                        l3_intf = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                                &egr_nh, L3__INTF_NUMf);
                        }
                        if (REPL_L3_INTF_NEXT_HOP_TRILL(unit, l3_intf) ==
                                next_hop_index) {
                            if_array[intf_count] = l3_intf;
                        } 
                    } else if (entry_type == 7) {
#ifdef BCM_TOMAHAWK3_SUPPORT
                        if (SOC_IS_TOMAHAWK3(unit)) {
                            l3_intf = soc_EGR_L3_NEXT_HOP_2m_field32_get(unit,
                                    &egr_nh2, INTF_NUMf);

                        } else
#endif
                        {
                        l3_intf = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                                &egr_nh, L3MC__INTF_NUMf);
                        }
                        if (REPL_L3_INTF_NEXT_HOP_IPMC(unit, l3_intf) ==
                                next_hop_index) {
                            if_array[intf_count] = l3_intf;
                        }
                    }
#ifdef BCM_TOMAHAWK3_SUPPORT
                    else if ((entry_type == 3) &&
                             (SOC_IS_TOMAHAWK3(unit))) {
                        l3_intf = soc_EGR_L3_NEXT_HOP_2m_field32_get(unit,
                                    &egr_nh2, INTF_NUMf);
                        if (REPL_L3_INTF_NEXT_HOP_IPMC(unit, l3_intf) ==
                                next_hop_index) {
                            if_array[intf_count] = l3_intf;
                        } 

                    }
#endif

                    intf_count++;
                    if (intf_count == if_max) {
                        *if_count = intf_count;
                        goto intf_get_done;
                    }
                }
            }
        }
        prev_repl_entry_ptr = repl_entry_ptr;
        repl_entry_ptr = soc_mem_field32_get(unit, mem,
                repl_list_entry, NEXTPTRf);
        if (intf_count >= REPL_PORT_GROUP_INTF_COUNT(unit, port_num, repl_group)) {
            break;
        }
    }

    *if_count = intf_count;

intf_get_done:
    REPL_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_th_ipmc_egress_intf_add
 * Purpose:
 *      Add L3 interface to selected ports' replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - IPMC group number.
 *      port     - port to add.
 *      l3_intf  - L3 interface to replicate.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_ipmc_egress_intf_add(int unit, int ipmc_id, bcm_port_t port,
                            bcm_l3_intf_t *l3_intf)
{
    bcm_if_t *if_array = NULL;
    int  intf_num, intf_max, alloc_size, rv = BCM_E_NONE;
    pbmp_t pbmp, ubmp;
#ifdef BCM_TOMAHAWK3_SUPPORT
    bcm_pbmp_t l2_pbmp, l3_pbmp;
    bcm_port_t port_idx;
    int th3_if_count = 0;
#endif
    bcm_port_t port_in = port;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if ((soc_feature(unit, soc_feature_channelized_switching)) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {

        BCM_IF_ERROR_RETURN(
            _bcmi_coe_subport_physical_port_get(unit, port, &port));

    }
#endif


    REPL_INIT(unit);
    REPL_GROUP_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    /* Check if port belongs to this VLAN */
    if (!SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_vlan_port_get(unit, l3_intf->l3a_vid, &pbmp, &ubmp));
        if (!SOC_PBMP_MEMBER(pbmp, port)) {
            return BCM_E_PARAM;
        }
    }

    intf_max = REPL_INTF_TOTAL(unit);
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        intf_max = TH3_IPMC_MAX_INTF_COUNT;
        /* Retrieve the number of replications for this MC group across all
         * ports */
        rv = _bcm_esw_multicast_ipmc_read(unit, ipmc_id, &l2_pbmp, &l3_pbmp);
        /* Add any L2 copies for the MC group */
        BCM_PBMP_COUNT(l2_pbmp, th3_if_count);
        /* Iterate among all L3 ports to get the sum of interface counts */
        alloc_size = intf_max * sizeof(bcm_if_t);
        if_array = sal_alloc(alloc_size, "IPMC repl interface array");
        if (if_array == NULL) {
            return BCM_E_MEMORY;
        }

        REPL_LOCK(unit);
        BCM_PBMP_ITER(l3_pbmp, port_idx) {
            if (port_idx == 158) {
                /* Skip checke for spare port used in B0 WAR*/
                continue;
            }
            rv = bcm_th_ipmc_egress_intf_get(unit, ipmc_id, port_idx, intf_max,
                                     if_array, &intf_num);
            if (!BCM_SUCCESS(rv)) {
                REPL_UNLOCK(unit);
                sal_free(if_array);
                return rv;
            }
            th3_if_count += intf_num;
        }
        REPL_UNLOCK(unit);
        sal_free(if_array);
        if (th3_if_count >= intf_max) {
            return BCM_E_EXISTS;
        }
    }
#endif

    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    REPL_LOCK(unit);
    rv = bcm_th_ipmc_egress_intf_get(unit, ipmc_id, port_in, intf_max,
                                     if_array, &intf_num);
    if (BCM_SUCCESS(rv)) {
        if (intf_num < intf_max) {
            if_array[intf_num++] = l3_intf->l3a_intf_id;
            rv = bcm_th_ipmc_egress_intf_set(unit, ipmc_id, port_in,
                                             intf_num, if_array, TRUE, FALSE);
        } else {
            rv = BCM_E_EXISTS;
        }
    }


#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (BCM_SUCCESS(rv)) {
        if ((soc_feature(unit, soc_feature_subport_forwarding_support)) &&
            BCM_GPORT_IS_SET(port_in) &&
                _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port_in)) {

            rv = bcmi_subport_multicast_lc_pbm_set(unit, ipmc_id, port_in, 1);
        }
    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */


    REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      bcm_th_ipmc_egress_intf_delete
 * Purpose:
 *      Remove L3 interface from selected ports' replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - IPMC group number.
 *      port     - port to remove.
 *      l3_intf  - L3 interface to delete from replication.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_ipmc_egress_intf_delete(int unit, int ipmc_id, bcm_port_t port,
                               bcm_l3_intf_t *l3_intf)
{
    bcm_if_t *if_array = NULL;
    int alloc_size, intf_max, if_count, if_cur, match, rv = BCM_E_NONE;

    bcm_port_t port_in = port;
    bcm_port_t port_num = port;


#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if ((soc_feature(unit, soc_feature_channelized_switching)) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {

        BCM_IF_ERROR_RETURN(
            _bcmi_coe_subport_physical_port_get(unit, port, &port));


        BCM_IF_ERROR_RETURN(
            _bcm_coe_subtag_subport_port_subport_num_get(unit,
            port, NULL, (int *)&port_num));

    }
#endif

    REPL_INIT(unit);
    REPL_GROUP_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if (!REPL_PORT_GROUP_INTF_COUNT(unit, port_num, ipmc_id)) {
        return BCM_E_NOT_FOUND;
    }

    intf_max = REPL_INTF_TOTAL(unit);
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        intf_max = TH3_IPMC_MAX_INTF_COUNT;
    }
#endif
    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    REPL_LOCK(unit);
    rv = bcm_th_ipmc_egress_intf_get(unit, ipmc_id, port_in, intf_max,
                                     if_array, &if_count);
    if (BCM_SUCCESS(rv)) {
        match = FALSE;
        for (if_cur = 0; if_cur < if_count; if_cur++) {
            if (match) {
                if_array[if_cur - 1] = if_array[if_cur];
            } else {
                if (if_array[if_cur] == l3_intf->l3a_intf_id) {
                    match = TRUE;
                }
            }
        }
        if (match) {
            if_count--;
            rv = bcm_th_ipmc_egress_intf_set(unit, ipmc_id, port_in,
                                             if_count, if_array, TRUE, FALSE);
        } else {
            rv = BCM_E_NOT_FOUND;
        }
    }


#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (BCM_SUCCESS(rv)) {
        if ((soc_feature(unit, soc_feature_subport_forwarding_support)) &&
            BCM_GPORT_IS_SET(port_in) &&
                _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port_in)) {

            rv = bcmi_subport_multicast_lc_pbm_set(unit, ipmc_id, port_in, 0);
        }
    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

    REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      _bcm_th_ipmc_repl_get
 * Purpose:
 *      Return set of VLANs selected for port's replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - IPMC group number.
 *      port     - port to list.
 *      vlan_vec - (OUT) vector of replicated VLANs common to selected ports.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_ipmc_repl_get(int unit, int ipmc_id, bcm_port_t port,
                        bcm_vlan_vector_t vlan_vec)
{
    int rv = BCM_E_NONE;
    int pipe;
    uint32 ls_bits[2];
    int prev_repl_entry_ptr, repl_entry_ptr;
    int intf_base;
    int ls_pos;
    uint32 repl_list_entry[SOC_MAX_MEM_FIELD_WORDS];
    int next_hop_index;
    int entry_type;
    bcm_l3_intf_t l3_intf;
    bcm_l3_egress_t egress_object;
    bcm_if_t egr_if;
    int vlan_count;
    soc_mem_t mem;
    egr_l3_next_hop_entry_t egr_nh;
    soc_field_t ent_type_fld = ENTRY_TYPEf;
    if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, DATA_TYPEf)) {
        ent_type_fld = DATA_TYPEf;
    }

    REPL_INIT(unit);
    REPL_GROUP_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);
    BCM_IF_ERROR_RETURN(_bcm_th_port_pipe_get(unit, port, &pipe));

    BCM_VLAN_VEC_ZERO(vlan_vec);
    mem = REPL_LIST_TBL(unit);

    REPL_LOCK(unit);
    if (REPL_PORT_GROUP_INTF_COUNT(unit, port, ipmc_id) == 0) {
        REPL_UNLOCK(unit);
        return BCM_E_NONE;
    }

    rv = _bcm_th_repl_list_start_ptr_get(unit, ipmc_id, port,
            &repl_entry_ptr, port);
    if (BCM_FAILURE(rv)) {
        goto vlan_get_done;
    }

    prev_repl_entry_ptr = -1;
    vlan_count = 0;
    while (repl_entry_ptr != prev_repl_entry_ptr) {
        rv = bcmi_repl_list_tbl_read(unit, &pipe,
            repl_entry_ptr, repl_list_entry);
/*
        rv = soc_mem_read(unit, repl_list_table, MEM_BLOCK_ANY, 
                          repl_entry_ptr, &repl_list_entry);
*/
        if (BCM_FAILURE(rv)) {
            goto vlan_get_done;
        }
        /* Each MSB represents 64 entries in LSB bitmap */
        intf_base = soc_mem_field32_get(unit, mem, repl_list_entry,
                MSB_VLANf) * 64;
        soc_mem_field_get(unit, mem, repl_list_entry,
                LSB_VLAN_BMf, ls_bits);
        for (ls_pos = 0; ls_pos < 64; ls_pos++) {
            if (ls_bits[ls_pos / 32] & (1 << (ls_pos % 32))) {
                next_hop_index = intf_base + ls_pos;
                rv = READ_EGR_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY,
                        next_hop_index, &egr_nh);
                if (BCM_FAILURE(rv)) {
                    goto vlan_get_done;
                }
                entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                        &egr_nh, ent_type_fld);
                bcm_l3_intf_t_init(&l3_intf);
                if (entry_type == 7) {
                    l3_intf.l3a_intf_id = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                            &egr_nh, L3MC__INTF_NUMf);
                } else {
                    egr_if = next_hop_index + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
                    rv = bcm_esw_l3_egress_get(unit, egr_if, &egress_object);
                    if (BCM_FAILURE(rv)) {
                        goto vlan_get_done;
                    }
                    l3_intf.l3a_intf_id = egress_object.intf;
                }
                rv = bcm_esw_l3_intf_get(unit, &l3_intf);
                if (BCM_FAILURE(rv)) {
                    goto vlan_get_done;
                }
                BCM_VLAN_VEC_SET(vlan_vec, l3_intf.l3a_vid);
                vlan_count++;
            }
        }
        prev_repl_entry_ptr = repl_entry_ptr;
        repl_entry_ptr = soc_mem_field32_get(unit, mem,
                repl_list_entry, NEXTPTRf);
        if (vlan_count >= REPL_PORT_GROUP_INTF_COUNT(unit, port, ipmc_id)) {
            break;
        }
    }

vlan_get_done:
    REPL_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_th_ipmc_repl_add
 * Purpose:
 *      Add VLAN to selected ports' replication list for chosen
 *      IPMC group.
 *Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - IPMC group number.
 *      port     - port to add.
 *      vlan     - VLAN to replicate.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_ipmc_repl_add(int unit, int ipmc_id, bcm_port_t port,
                     bcm_vlan_t vlan)
{
    int alloc_size, intf_max, if_count, rv = BCM_E_NONE;
    bcm_if_t *if_array = NULL;
    pbmp_t pbmp, ubmp;
    bcm_l3_intf_t l3_intf;

    REPL_INIT(unit);
    REPL_GROUP_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    /* Check if port belongs to this VLAN */
    BCM_IF_ERROR_RETURN
        (bcm_esw_vlan_port_get(unit, vlan, &pbmp, &ubmp));
    if (!SOC_PBMP_MEMBER(pbmp, port)) {
        return BCM_E_PARAM;
    }

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_vid = vlan;
    if (bcm_esw_l3_intf_find_vlan(unit, &l3_intf) < 0) {
        return BCM_E_PARAM;
    }

    REPL_LOCK(unit);

    intf_max = 1 + REPL_PORT_GROUP_INTF_COUNT(unit, port, ipmc_id);
    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        REPL_UNLOCK(unit);
        return BCM_E_MEMORY;
    }

    rv = bcm_th_ipmc_egress_intf_get(unit, ipmc_id, port, intf_max,
                                     if_array, &if_count);
    if (BCM_SUCCESS(rv)) {
        if (if_count < intf_max) {
            if_array[if_count++] = l3_intf.l3a_intf_id;
            rv = bcm_th_ipmc_egress_intf_set(unit, ipmc_id, port,
                                             if_count, if_array, TRUE, FALSE);
        } else {
            rv = BCM_E_EXISTS;
        }
    }

    REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      _bcm_th_ipmc_repl_delete
 * Purpose:
 *      Remove VLAN from selected ports' replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - IPMC group number.
 *      port     - port to remove.
 *      vlan     - VLAN to delete from replication.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_th_ipmc_repl_delete(int unit, int ipmc_id, bcm_port_t port,
                        bcm_vlan_t vlan)
{
    int alloc_size, intf_max, if_count, if_cur, match, rv = BCM_E_NONE;
    bcm_if_t *if_array = NULL;
    bcm_l3_intf_t l3_intf;

    REPL_INIT(unit);
    REPL_GROUP_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if (!REPL_PORT_GROUP_INTF_COUNT(unit, port, ipmc_id)) {
        return BCM_E_NOT_FOUND;
    }

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_vid = vlan;
    if (bcm_esw_l3_intf_find_vlan(unit, &l3_intf) < 0) {
        return BCM_E_PARAM;
    }

    REPL_LOCK(unit);
    intf_max = REPL_PORT_GROUP_INTF_COUNT(unit, port, ipmc_id);
    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        REPL_UNLOCK(unit);
        return BCM_E_MEMORY;
    }

    rv = bcm_th_ipmc_egress_intf_get(unit, ipmc_id, port, intf_max,
                                     if_array, &if_count);
    if (BCM_SUCCESS(rv)) {
        match = FALSE;
        for (if_cur = 0; if_cur < if_count; if_cur++) {
            if (match) {
                if_array[if_cur - 1] = if_array[if_cur];
            } else {
                if (if_array[if_cur] == l3_intf.l3a_intf_id) {
                    match = TRUE;
                }
            }
        }
        if (match) {
            if_count--;
            rv = bcm_th_ipmc_egress_intf_set(unit, ipmc_id, port,
                                             if_count, if_array, TRUE, FALSE);
        } else {
            rv = BCM_E_NOT_FOUND;
        }
    }
    REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      bcm_th_ipmc_repl_delete_all
 * Purpose:
 *      Remove all VLANs from selected ports' replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - The MC index number.
 *      port     - port from which to remove VLANs.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_ipmc_repl_delete_all(int unit, int ipmc_id, bcm_port_t port)
{
    REPL_INIT(unit);
    REPL_GROUP_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if (!REPL_PORT_GROUP_INTF_COUNT(unit, port, ipmc_id)) {
        /* Nothing to do */
        return BCM_E_NONE;
    }

    return bcm_th_ipmc_egress_intf_set(unit, ipmc_id, port, 0, NULL,
                                        TRUE, FALSE);
}

/*
 * Function:
 *      bcm_th_ipmc_repl_set
 * Purpose:
 *      Assign set of VLANs provided to port's replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - The index number.
 *      port     - port to list.
 *      vlan_vec - (IN) vector of replicated VLANs common to selected ports.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_ipmc_repl_set(int unit, int ipmc_id, bcm_port_t port,
                     bcm_vlan_vector_t vlan_vec)
{
    int rv = BCM_E_NONE;
    bcm_if_t *if_array = NULL;
    bcm_l3_intf_t l3_intf;
    pbmp_t pbmp, ubmp;
    int  intf_num, intf_max, alloc_size, vid;

    REPL_INIT(unit);
    REPL_GROUP_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    intf_max = BCM_VLAN_MAX - BCM_VLAN_MIN + 1;
    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    sal_memset(if_array, 0, alloc_size);
    intf_num = 0;
    for (vid = BCM_VLAN_MIN; vid < BCM_VLAN_MAX; vid++) {
        if (BCM_VLAN_VEC_GET(vlan_vec, vid)) {
            rv = bcm_esw_vlan_port_get(unit, vid, &pbmp, &ubmp);
            if (BCM_FAILURE(rv)) {
                sal_free(if_array);
                return rv;
            }
            if (!BCM_PBMP_MEMBER(pbmp, port)) {
                sal_free(if_array);
                return BCM_E_PARAM;
            }
            bcm_l3_intf_t_init(&l3_intf);
            l3_intf.l3a_vid = vid;
            if ((rv = bcm_esw_l3_intf_find_vlan(unit, &l3_intf)) < 0) {
                sal_free(if_array);
                return rv;
            }
            if_array[intf_num++] = l3_intf.l3a_intf_id;
        }
    }

    rv = bcm_th_ipmc_egress_intf_set(unit, ipmc_id, port,
                                     intf_num, if_array, TRUE, FALSE);

    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      _bcm_th_ipmc_egress_intf_add
 * Purpose:
 *      Add encap ID to selected ports' replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - IPMC group number.
 *      port     - port to add.
 *      encap_id - Encap ID.
 *      is_l3    - Indicates if multicast group type is IPMC.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_ipmc_egress_intf_add(int unit, int ipmc_id, bcm_port_t port,
                            int encap_id, int is_l3)
{
    bcm_if_t *if_array = NULL;
    int  intf_num, intf_max, alloc_size, rv = BCM_E_NONE;
    pbmp_t pbmp, ubmp;
    bcm_l3_intf_t l3_intf;
    bcm_port_t port_in = port;

#ifdef BCM_TOMAHAWK3_SUPPORT
    bcm_pbmp_t l2_pbmp, l3_pbmp;
    bcm_port_t port_idx;
    int th3_if_count = 0;
#endif

    REPL_INIT(unit);
    REPL_GROUP_ID(unit, ipmc_id);

#if defined(BCM_HGPROXY_COE_SUPPORT)
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
            BCM_IF_ERROR_RETURN(
                _bcmi_coe_subport_physical_port_get(unit, port, &port));
    }
#endif

    if (!IS_CPU_PORT(unit, port)) {
        REPL_PORT_CHECK(unit, port);
    }

    if ((BCM_MC_PER_TRUNK_REPL_MODE(unit)) &&
         encap_id == BCM_ENCAP_TRUNK_MEMBER) {
         /* For TH_MC_PER_TRUNK_REPL mode, indentical replication list is set
          * for all the members of a trunk.
          */
         return BCM_E_NONE;
    }

    intf_max = REPL_INTF_TOTAL(unit);
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        intf_max = TH3_IPMC_MAX_INTF_COUNT;
        /* Retrieve the number of replications for this MC group across all
         * ports */
        rv = _bcm_esw_multicast_ipmc_read(unit, ipmc_id, &l2_pbmp, &l3_pbmp);
        /* Add any L2 copies for the MC group */
        BCM_PBMP_COUNT(l2_pbmp, th3_if_count);
        /* Iterate among all L3 ports to get the sum of interface counts */
        alloc_size = intf_max * sizeof(bcm_if_t);
        if_array = sal_alloc(alloc_size, "IPMC repl interface array");
        if (if_array == NULL) {
            return BCM_E_MEMORY;
        }

        REPL_LOCK(unit);
        BCM_PBMP_ITER(l3_pbmp, port_idx) {
            if (port_idx == 158) {
                /* Skip checke for spare port used in B0 WAR*/
                continue;
            }
            rv = bcm_th_ipmc_egress_intf_get(unit, ipmc_id, port_idx, intf_max,
                                     if_array, &intf_num);
            if (!BCM_SUCCESS(rv)) {
                goto intf_add_done;
            }
            th3_if_count += intf_num;
        }
        REPL_UNLOCK(unit);
        sal_free(if_array);
        if (th3_if_count >= intf_max) {
            return BCM_E_EXISTS;
        }
    }
#endif
    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    REPL_LOCK(unit);
    rv = bcm_th_ipmc_egress_intf_get(unit, ipmc_id, port_in, intf_max,
                                     if_array, &intf_num);
    if (BCM_SUCCESS(rv)) {
        if (intf_num < intf_max) {
            if_array[intf_num++] = encap_id;

            /* For IPMC and Trill, check port is a member of the L3 interface's 
             * VLAN. Performing this check here is more efficient than doing 
             * it in bcm_th_ipmc_egress_intf_set.
             */ 
            if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, encap_id)) {
                /* L3 interface is used */

                if (encap_id > soc_mem_index_max(unit, EGR_L3_INTFm)) {
                    rv = BCM_E_PARAM;
                    goto intf_add_done;
                }

                bcm_l3_intf_t_init(&l3_intf);
                l3_intf.l3a_intf_id = encap_id;
                rv = bcm_esw_l3_intf_get(unit, &l3_intf);
                if (BCM_FAILURE(rv)) {
                    goto intf_add_done;
                }

                if (!SOC_IS_TRIDENT3X(unit)) {
                    rv = bcm_esw_vlan_port_get(unit, l3_intf.l3a_vid,
                            &pbmp, &ubmp);
                    if (BCM_FAILURE(rv)) {
                        goto intf_add_done;
                    }
                    if (!BCM_PBMP_MEMBER(pbmp, port)) {
                        rv = BCM_E_PARAM;
                        goto intf_add_done;
                    }
                }
            }

            rv = bcm_th_ipmc_egress_intf_set(unit, ipmc_id, port_in,
                                             intf_num, if_array, is_l3, FALSE);
        } else {
            rv = BCM_E_EXISTS;
        }
    }


#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (BCM_SUCCESS(rv)) {
        if ((soc_feature(unit, soc_feature_subport_forwarding_support)) &&
            BCM_GPORT_IS_SET(port_in) &&
                _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port_in)) {

            rv = bcmi_subport_multicast_lc_pbm_set(unit, ipmc_id, port_in, 1);
        }
    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */


intf_add_done:

    REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      _bcm_th_ipmc_egress_intf_delete
 * Purpose:
 *      Remove encap ID from selected ports' replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - IPMC group number.
 *      port     - port to remove.
 *      if_max   - Maximal interface.
 *      encap_id - Encap ID to delete from replication.
 *      is_l3    - Indicates if multicast group type is IPMC.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_ipmc_egress_intf_delete(int unit, int ipmc_id, bcm_port_t port,
                               int if_max, int encap_id, int is_l3)
{
    bcm_if_t *if_array = NULL;
    int alloc_size, if_count, if_cur, match, rv = BCM_E_NONE;
    bcm_port_t port_in = port;
    bcm_port_t port_num = port;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if ((soc_feature(unit, soc_feature_channelized_switching)) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {

        BCM_IF_ERROR_RETURN(
            _bcmi_coe_subport_physical_port_get(unit, port, &port));


        BCM_IF_ERROR_RETURN(
            _bcm_coe_subtag_subport_port_subport_num_get(unit,
            port_in, NULL, (int *)&port_num));

    }
#endif


    REPL_INIT(unit);
    REPL_GROUP_ID(unit, ipmc_id);
    if (!IS_CPU_PORT(unit, port)) {
        REPL_PORT_CHECK(unit, port);
    }

    if ((BCM_MC_PER_TRUNK_REPL_MODE(unit)) &&
         encap_id == BCM_ENCAP_TRUNK_MEMBER) {
         /* For MC_PER_TRUNK_REPL mode, indentical replication list is set
          * for all the members of a trunk.
          */
         return BCM_E_NONE;
    }

    if (!REPL_PORT_GROUP_INTF_COUNT(unit, port_num, ipmc_id)) {
        return BCM_E_NOT_FOUND;
    }

    if ((if_max <= 0) || ((uint32)if_max > REPL_INTF_TOTAL(unit))) {
        return BCM_E_PARAM;
    }
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        if((uint32)if_max > TH3_IPMC_MAX_INTF_COUNT) {
            return BCM_E_PARAM;
        }
    }
#endif

    alloc_size = if_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    REPL_LOCK(unit);
    rv = bcm_th_ipmc_egress_intf_get(unit, ipmc_id, port_in, if_max,
                                     if_array, &if_count);
    if (BCM_SUCCESS(rv)) {
        match = FALSE;
        for (if_cur = 0; if_cur < if_count; if_cur++) {
            if (match) {
                if_array[if_cur - 1] = if_array[if_cur];
            } else {
                if (if_array[if_cur] == encap_id) {
                    match = TRUE;
                }
            }
        }
        if (match) {
            if_count--;
            rv = bcm_th_ipmc_egress_intf_set(unit, ipmc_id, port_in,
                                             if_count, if_array, is_l3, FALSE);
        } else {
            rv = BCM_E_NOT_FOUND;
        }
    }

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (BCM_SUCCESS(rv)) {
        if ((soc_feature(unit, soc_feature_subport_forwarding_support)) &&
            BCM_GPORT_IS_SET(port_in) &&
                _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port_in)) {

            rv = bcmi_subport_multicast_lc_pbm_set(unit, ipmc_id, port_in, 0);
        }
    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

    REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      bcm_th_ipmc_trill_mac_update
 * Purpose:
 *      Update the Trill MAC address in next hop entries that are
 *      in Trill multicast replication list. 
 * Parameters:
 *      unit      - (IN) SOC unit #
 *      mac_field - (IN) MAC address
 *      flag      - (IN) 0 = Set the lower 24 bits of MAC address,
 *                       1 = Set the higher 24 bits of MAC address.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_ipmc_trill_mac_update(int unit, uint32 mac_field, uint8 flag)
{
    int intf, nh_index;
    egr_l3_next_hop_entry_t egr_nh;
    uint32 entry_type;
    bcm_mac_t mac;
    soc_field_t ent_type_fld = ENTRY_TYPEf;
    if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, DATA_TYPEf)) {
        ent_type_fld = DATA_TYPEf;
    }

    for (intf = 0; intf < soc_mem_index_count(unit, EGR_L3_INTFm); intf++) {
        nh_index = REPL_L3_INTF_NEXT_HOP_TRILL(unit, intf);
        if (nh_index >= 0) {
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm,
                        MEM_BLOCK_ANY, nh_index, &egr_nh));
            entry_type = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                    &egr_nh, ent_type_fld);
            if (entry_type == 0) { /* normal entry type */
                soc_mem_mac_addr_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3__MAC_ADDRESSf, mac);
                if (!flag) {
                    /* Set the low-order bytes */
                    mac[3] = (uint8) (mac_field >> 16 & 0xff);
                    mac[4] = (uint8) (mac_field >> 8 & 0xff);
                    mac[5] = (uint8) (mac_field & 0xff);
                } else {
                    /* Set the High-order bytes */
                    mac[0] = (uint8) (mac_field >> 16 & 0xff);
                    mac[1] = (uint8) (mac_field >> 8 & 0xff);
                    mac[2] = (uint8) (mac_field & 0xff);
                }
                soc_mem_mac_addr_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3__MAC_ADDRESSf, mac);
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_ipmc_l3_intf_next_hop_free
 * Purpose:
 *      Free the next hop index associated with the given L3 interface.
 * Parameters:
 *      unit - (IN) SOC unit #
 *      intf - (IN) L3 interface ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_ipmc_l3_intf_next_hop_free(int unit, int intf)
{
    int nh_index;

    if (_th_repl_info[unit] == NULL) {
        /* IPMC replication is not initialized. There is no next hop
         * index to free.
         */
        return BCM_E_NONE;
    }

    nh_index = REPL_L3_INTF_NEXT_HOP_IPMC(unit, intf);
    if (nh_index >= 0) {
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_L3_NEXT_HOP_2m(unit, MEM_BLOCK_ALL, nh_index,
                                          soc_mem_entry_null(unit,
                                          EGR_L3_NEXT_HOP_2m)));
        }
#endif
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_L3_NEXT_HOPm(unit, MEM_BLOCK_ALL, nh_index,
                                    soc_mem_entry_null(unit, EGR_L3_NEXT_HOPm)));
        BCM_IF_ERROR_RETURN
            (bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, nh_index));
        REPL_L3_INTF_NEXT_HOP_IPMC(unit, intf) = -1;
    }

    nh_index = REPL_L3_INTF_NEXT_HOP_TRILL(unit, intf);
    if (nh_index >= 0) {
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_L3_NEXT_HOP_2m(unit, MEM_BLOCK_ALL, nh_index,
                                          soc_mem_entry_null(unit,
                                          EGR_L3_NEXT_HOP_2m)));
        }

#endif
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_L3_NEXT_HOPm(unit, MEM_BLOCK_ALL, nh_index,
                                    soc_mem_entry_null(unit, EGR_L3_NEXT_HOPm)));
        BCM_IF_ERROR_RETURN
            (bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, nh_index));
        REPL_L3_INTF_NEXT_HOP_TRILL(unit, intf) = -1;
    }

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int
bcm_th_set_port_intf_cnt(int unit, int pipe,int aggid,
                         int repl_group, int intf_cnt)
{
    int port_iter, aggid_iter, pipe_iter=0;
    uint32 regval;
    bcm_pbmp_t            pbmp_all;
    int num_ports = SOC_MAX_NUM_PORTS;
    bcm_port_t pport, lport;

    BCM_PBMP_CLEAR(pbmp_all);

    BCM_PBMP_ASSIGN(pbmp_all, PBMP_ALL(unit));

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    /*
     * In channelized switching, the packets can be sent to remote
     * port which is not represented in your local port bitmap.
     * Threfore the iternation is done over all local and remote
     * ports to set multicast memberships.
     */
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        num_ports = FB6_TOTAL_NUM_SUBPORT;
    }
#endif

    for (port_iter=0; port_iter < num_ports; port_iter++) {

        pport = port_iter;
        lport = port_iter;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
        if (soc_feature(unit, soc_feature_channelized_switching)) {
            bcm_gport_t port_gport;
            soc_info_t *si = &SOC_INFO(unit);

            if (port_iter < FB6_NUM_PORT) {
                pport = si->port_m2p_mapping[port_iter];
                lport = si->port_l2p_mapping[pport];
                if (!(BCM_PBMP_MEMBER(pbmp_all, lport))) {
                    continue;
                }
            } else {
                _bcm_coe_subtag_subport_num_subport_gport_get(unit, port_iter, &port_gport);
                if (!_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port_gport)) {
                    /* This is not a valid subport. continue;*/
                    continue;
                }
            }
        } else
#endif
        {
            if (!(BCM_PBMP_MEMBER(pbmp_all, port_iter))) {
                continue;
            }
        }

        if (SOC_IS_TOMAHAWK3(unit)) {
            BCM_IF_ERROR_RETURN(
                READ_MMU_RQE_REPL_PORT_AGG_MAPr(unit, port_iter, &regval));
            aggid_iter = soc_reg_field_get(unit, MMU_RQE_REPL_PORT_AGG_MAPr, regval,
                                       L3MC_PORT_AGG_IDf);
        } else {
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
            if (soc_feature(unit, soc_feature_repl_port_agg_map_is_mem)) {
                mmu_dqs_repl_port_agg_map_entry_t  dqs_entry;
                sal_memset(&dqs_entry, 0, sizeof(mmu_dqs_repl_port_agg_map_entry_t));


                SOC_IF_ERROR_RETURN(
                    soc_mem_read(unit, MMU_DQS_REPL_PORT_AGG_MAP_PIPE(unit, 0),
                       MEM_BLOCK_ALL, port_iter, &dqs_entry));

               aggid_iter = soc_mem_field32_get(unit,
                     MMU_DQS_REPL_PORT_AGG_MAP_PIPE(unit, 0), &dqs_entry,
                     L3MC_PORT_AGG_IDf);

                /* Set the phy port to 1 to get pipe for single pipe. */
                pport = 1;

            } else 
#endif
            {

                BCM_IF_ERROR_RETURN(
                    READ_MMU_DQS_REPL_PORT_AGG_MAPr(unit, port_iter, &regval));
                aggid_iter = soc_reg_field_get(unit, MMU_DQS_REPL_PORT_AGG_MAPr, regval,
                                       L3MC_PORT_AGG_IDf);
            }
            /* coverity[overrun-local:FALSE] */
            pipe_iter = SOC_INFO(unit).port_pipe[pport];
        }

        if ((aggid_iter == aggid) && (pipe_iter == pipe)) {
            /* coverity[overrun-local:FALSE] */
            REPL_PORT_GROUP_INTF_COUNT(unit, lport, repl_group) = intf_cnt;
        }
    }

    return BCM_E_NONE;
}

STATIC int
bcm_th_add_port_intf_cnt(int unit, int pipe, int aggid,
                         int repl_group, int intf_cnt)
{
    int port_iter, aggid_iter, pipe_iter=0;
    uint32 regval;
    bcm_pbmp_t pbmp_all;
    int num_ports = SOC_MAX_NUM_PORTS;
    bcm_port_t pport, lport;

    BCM_PBMP_CLEAR(pbmp_all);
    BCM_PBMP_ASSIGN(pbmp_all, PBMP_ALL(unit));

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    /*
     * In channelized switching, the packets can be sent to remote
     * port which is not represented in your local port bitmap.
     * Threfore the iternation is done over all local and remote
     * ports to set multicast memberships.
     */
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        num_ports = FB6_TOTAL_NUM_SUBPORT;
    }
#endif

    for (port_iter=0; port_iter < num_ports; port_iter++) {

        pport = port_iter;
        lport = port_iter;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
        if (soc_feature(unit, soc_feature_channelized_switching)) {
            bcm_gport_t port_gport;
            soc_info_t *si = &SOC_INFO(unit);

            if (port_iter < FB6_NUM_PORT) {
                pport = si->port_m2p_mapping[port_iter];
                lport = si->port_l2p_mapping[pport];
                if (!(BCM_PBMP_MEMBER(pbmp_all, lport))) {
                    continue;
                }
            } else {
                _bcm_coe_subtag_subport_num_subport_gport_get(unit, port_iter, &port_gport);
                if (!_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port_gport)) {
                    /* This is not a valid subport. continue;*/
                    continue;
                }
            }
        } else
#endif
        {
            if (!(BCM_PBMP_MEMBER(pbmp_all, port_iter))) {
                continue;
            }
        }

        if (SOC_IS_TOMAHAWK3(unit)) {
            BCM_IF_ERROR_RETURN(
                READ_MMU_RQE_REPL_PORT_AGG_MAPr(unit, port_iter, &regval));
            aggid_iter = soc_reg_field_get(unit, MMU_RQE_REPL_PORT_AGG_MAPr, regval,
                                       L3MC_PORT_AGG_IDf);
        } else {
            if (soc_feature(unit, soc_feature_repl_port_agg_map_is_mem)) {
                mmu_dqs_repl_port_agg_map_entry_t  dqs_entry;
                sal_memset(&dqs_entry, 0, sizeof(mmu_dqs_repl_port_agg_map_entry_t));


                SOC_IF_ERROR_RETURN(
                    soc_mem_read(unit, MMU_DQS_REPL_PORT_AGG_MAP_PIPE(unit, 0),
                       MEM_BLOCK_ALL, port_iter, &dqs_entry));

               aggid_iter = soc_mem_field32_get(unit,
                     MMU_DQS_REPL_PORT_AGG_MAP_PIPE(unit, 0), &dqs_entry,
                     L3MC_PORT_AGG_IDf);

                /* Set the phy port to 1 to get pipe for single pipe. */
                pport = 1;
            } else {

                BCM_IF_ERROR_RETURN(
                    READ_MMU_DQS_REPL_PORT_AGG_MAPr(unit, port_iter, &regval));
                aggid_iter = soc_reg_field_get(unit, MMU_DQS_REPL_PORT_AGG_MAPr, regval,
                                       L3MC_PORT_AGG_IDf);
            }
            /* coverity[overrun-local:FALSE] */
            pipe_iter = SOC_INFO(unit).port_pipe[pport];
        }

        if ((aggid_iter == aggid) && (pipe_iter == pipe)) {
            /* coverity[overrun-local:FALSE] */
            REPL_PORT_GROUP_INTF_COUNT(unit, lport, repl_group) += intf_cnt;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ipmc_repl_reload
 * Purpose:
 *      Re-Initialize replication software to state consistent with
 *      hardware.
 * Parameters:
 *      unit - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_ipmc_repl_reload(int unit)
{
    int rv = BCM_E_NONE;
    soc_mem_t repl_group_table;
    soc_field_t base_idx_f;
    soc_field_t member_bitmap_f;
    int alloc_size;
    int intf_vec_alloc_size;
    uint8 *repl_group_buf = NULL;
    SHR_BITDCL *intf_vec = NULL;
    int pipe;
    int index_min, index_max;
    int i, j;
    uint32 *repl_group_entry;
    int head_index;
    uint32 fldbuf[BCM_REPL_HEAD_MEMBER_WORD_MAX] = {0};
    uint32 member_bitmap[BCM_REPL_HEAD_MEMBER_WORD_MAX] = {0};
/*
    uint32 fldbuf[SOC_PBMP_WORD_MAX];
    soc_pbmp_t member_bitmap;
*/
    int member_count=0;
    int member_id, member;
    soc_info_t *si;
    bcm_port_t mmu_port, phy_port, port = -1;
    uint32 repl_head_entry[SOC_MAX_MEM_FIELD_WORDS];
    int start_ptr;
    _bcm_repl_list_info_t *rli_current;
    int prev_repl_entry_ptr, repl_entry_ptr;
    uint32 repl_list_entry[SOC_MAX_MEM_FIELD_WORDS];
    int msb;
    uint32 ls_bits[2];
    uint8 *egr_nh_buf = NULL;
    egr_l3_next_hop_entry_t *egr_nh_entry;
#ifdef BCM_TOMAHAWK3_SUPPORT
    uint8 *egr_nh2_buf = NULL;
    egr_l3_next_hop_2_entry_t *egr_nh2_entry = NULL;
#endif
    int entry_type;
    int l3_intf;
    uint8 flags;
    int aggid = -1, intf_cnt = 0;
    soc_mem_t mem;
    soc_field_t ent_type_fld = ENTRY_TYPEf;

    mem = REPL_LIST_TBL(unit);
    if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, DATA_TYPEf)) {
        ent_type_fld = DATA_TYPEf;
    }

    /* Initialize internal data structures */
    BCM_IF_ERROR_RETURN(bcm_th_ipmc_repl_init(unit));

    /* Recover internal state by traversing replication lists
     * in each pipeline.
     */
#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        base_idx_f = BASE_PTRf;
        member_bitmap_f = MEMBER_BMPf;
    } else
#endif
    {
    base_idx_f = PIPE_BASE_PTRf;
    member_bitmap_f = PIPE_MEMBER_BMPf;
    }

    alloc_size = 4 * SOC_MEM_WORDS(unit, MMU_REPL_GROUP_INFO_TBLm) *
        _th_repl_info[unit]->num_repl_groups;
    repl_group_buf = soc_cm_salloc(unit, alloc_size, "repl group buf");
    if (NULL == repl_group_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    intf_vec_alloc_size = SHR_BITALLOCSIZE(REPL_INTF_TOTAL(unit));
    intf_vec = sal_alloc(intf_vec_alloc_size, "Repl interface vector");
    if (intf_vec == NULL) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(intf_vec, 0, intf_vec_alloc_size);

    for (pipe = 0; pipe < NUM_PIPES(unit); pipe++) {
        /* Read pipeline's replication group table */
        if(SOC_IS_TOMAHAWK3(unit)) {
            repl_group_table = MMU_REPL_GROUP_INFO_TBLm;
        } else {
        repl_group_table = REPL_GROUP_INFO_TBL_PIPE(unit, pipe);
        }

        index_min = 0;
        index_max = _th_repl_info[unit]->num_repl_groups - 1;
        rv = soc_mem_read_range(unit, repl_group_table, MEM_BLOCK_ANY,
                                index_min, index_max, repl_group_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        for (i = index_min; i <= index_max; i++) {
            repl_group_entry = soc_mem_table_idx_to_pointer(unit,
                    repl_group_table, uint32 *, repl_group_buf, i);

            /* Get the head index */
            head_index = soc_mem_field32_get(unit, repl_group_table,
                    repl_group_entry, base_idx_f);
            if (0 == head_index) {
                continue; /* with next replication group */
            }

            /* Get the member bitmap */
            sal_memset(fldbuf, 0, BCM_REPL_HEAD_MEMBER_WORD_MAX * sizeof(uint32));
            soc_mem_field_get(unit, repl_group_table, repl_group_entry,
                    member_bitmap_f, fldbuf);
            member_count = 0;
            for (j = 0; j < BCM_REPL_HEAD_MEMBER_WORD_MAX; j++) {
                member_bitmap[j] = fldbuf[j];
                member_count += _shr_popcount(member_bitmap[j]);
            }
/*
            sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
            for (j = 0; j < SOC_PBMP_WORD_MAX; j++) {
                SOC_PBMP_WORD_SET(member_bitmap, j, fldbuf[j]);
            }
            SOC_PBMP_COUNT(member_bitmap, member_count);
*/
            if (0 == member_count) {
                continue; /* with next replication group */
            }

            member_id = 0;
            for (member=0; member<BCM_REPL_HEAD_MEMBER_MAX; member++) {
                if (!SHR_BITGET(member_bitmap, member)) {
                    continue;
                }

/*            SOC_PBMP_ITER(member_bitmap, member) {*/
                if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
                    aggid = member;
                } else {
                    if (SOC_IS_TOMAHAWK3(unit)) {
                        port = member;
                    } else {
                        /* Convert member to mmu port, then to physical port, and
                         * finally to logical port */
                        mmu_port = pipe * mmu_port_stride[unit] + member;
                        si = &SOC_INFO(unit);
                        phy_port = si->port_m2p_mapping[mmu_port];
                        port = si->port_p2l_mapping[phy_port];
                    }
                }

                /* Get replication list start pointer */
                rv = _bcm_td3_repl_head_tbl_read(unit, pipe,
                                  (head_index + member_id), repl_head_entry,
                                  (uint32 *)&start_ptr);
                if (SOC_FAILURE(rv)) {
                    goto cleanup;
                }
                member_id++;
                if (0 == start_ptr) {
                    continue; /* with next member */
                }

                if (_bcm_th_repl_list_entry_used_get(unit, pipe, start_ptr)) {
                    /* Already traversed this replication list */
                    for (rli_current = REPL_LIST_INFO(unit, pipe);
                            rli_current != NULL;
                            rli_current = rli_current->next) {
                        if (rli_current->index == start_ptr) {
                            (rli_current->refcount)++;
                            if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
                                intf_cnt = rli_current->list_size;
                                bcm_th_set_port_intf_cnt(unit, pipe, aggid,
                                                         i, intf_cnt);
                            } else {
                                REPL_PORT_GROUP_INTF_COUNT(unit, port, i) =
                                    rli_current->list_size;
                            }
                            break;
                        }
                    }
                    if (rli_current == NULL) {
                        /* Unexpected */
                        rv = BCM_E_INTERNAL;
                        goto cleanup;
                    } else {
                        continue; /* with next member */
                    }
                }

                /* Traverse the replication list table */
                sal_memset(intf_vec, 0, intf_vec_alloc_size);
                prev_repl_entry_ptr = -1;
                repl_entry_ptr = start_ptr;
                if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
                    intf_cnt = 0;
                }
                while (repl_entry_ptr != prev_repl_entry_ptr) {
                    rv = bcmi_repl_list_tbl_read(unit, &pipe,
                             repl_entry_ptr, repl_list_entry);

/*                    rv = soc_mem_read(unit, repl_list_table, MEM_BLOCK_ANY, 
                                      repl_entry_ptr, &repl_list_entry);
*/
                    if (BCM_FAILURE(rv)) {
                        goto cleanup;
                    }
                    msb = soc_mem_field32_get(unit, mem,
                            repl_list_entry, MSB_VLANf);
                    soc_mem_field_get(unit, mem, repl_list_entry,
                            LSB_VLAN_BMf, ls_bits);
                    intf_vec[2 * msb + 0] = ls_bits[0];
                    intf_vec[2 * msb + 1] = ls_bits[1];
                    if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
                        intf_cnt += (_shr_popcount(ls_bits[0]) +
                                     _shr_popcount(ls_bits[1]));
                    } else {
                        REPL_PORT_GROUP_INTF_COUNT(unit, port, i) +=
                            _shr_popcount(ls_bits[0]) + _shr_popcount(ls_bits[1]);
                    }
                    rv = _bcm_th_repl_list_entry_used_set(unit, pipe,
                            repl_entry_ptr);
                    if (BCM_FAILURE(rv)) {
                        goto cleanup;
                    }
                    prev_repl_entry_ptr = repl_entry_ptr;
                    repl_entry_ptr = soc_mem_field32_get(unit, mem,
                            repl_list_entry, NEXTPTRf);
                }
                if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
                    bcm_th_add_port_intf_cnt(unit, pipe, aggid, i, intf_cnt);
                }

                /* Insert a new replication list into linked list */
                rli_current = sal_alloc(sizeof(_bcm_repl_list_info_t),
                        "repl list info");
                if (rli_current == NULL) {
                    rv = BCM_E_MEMORY;
                    goto cleanup;
                }
                sal_memset(rli_current, 0, sizeof(_bcm_repl_list_info_t));
                rli_current->index = start_ptr;
                rli_current->hash = _shr_crc32b(0, (uint8 *)intf_vec,
                        REPL_INTF_TOTAL(unit));
                if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
                    rli_current->list_size = intf_cnt;
                } else {
                    rli_current->list_size = REPL_PORT_GROUP_INTF_COUNT(unit, port, i);
                }
                (rli_current->refcount)++;
                rli_current->next = REPL_LIST_INFO(unit, pipe);
                REPL_LIST_INFO(unit, pipe) = rli_current;
            }

            /* Update REPL_HEAD table usage */
            rv = _bcm_th_repl_head_block_used_set(unit, pipe, head_index,
                    member_count);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    }
    soc_cm_sfree(unit, repl_group_buf);
    repl_group_buf = NULL;
    sal_free(intf_vec);
    intf_vec = NULL;

    /* Recover array of next hop indices for L3 interfaces */
    egr_nh_buf = soc_cm_salloc(unit,
            SOC_MEM_TABLE_BYTES(unit, EGR_L3_NEXT_HOPm), "egr nh buf");
    if (NULL == egr_nh_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    index_min = soc_mem_index_min(unit, EGR_L3_NEXT_HOPm);
    index_max = soc_mem_index_max(unit, EGR_L3_NEXT_HOPm);
    rv = soc_mem_read_range(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
            index_min, index_max, egr_nh_buf);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        /* Recover array of next hop indices for L3 interfaces */
        egr_nh2_buf = soc_cm_salloc(unit,
                                    SOC_MEM_TABLE_BYTES(unit, EGR_L3_NEXT_HOP_2m),
                                    "egr nh_2 buf");
        if (NULL == egr_nh2_buf) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        index_min = soc_mem_index_min(unit, EGR_L3_NEXT_HOP_2m);
        index_max = soc_mem_index_max(unit, EGR_L3_NEXT_HOP_2m);
        rv = soc_mem_read_range(unit, EGR_L3_NEXT_HOP_2m, MEM_BLOCK_ANY,
                                index_min, index_max, egr_nh2_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }
    }
#endif

    for (i = index_min; i <= index_max; i++) {
        egr_nh_entry = soc_mem_table_idx_to_pointer
            (unit, EGR_L3_NEXT_HOPm, egr_l3_next_hop_entry_t *,
             egr_nh_buf, i);
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            egr_nh2_entry = soc_mem_table_idx_to_pointer
                (unit, EGR_L3_NEXT_HOP_2m, egr_l3_next_hop_2_entry_t *,
                 egr_nh2_buf, i);
        }
#endif
        entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                ent_type_fld);
        if (entry_type == 0) {
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                l3_intf = soc_EGR_L3_NEXT_HOP_2m_field32_get(unit, egr_nh2_entry,
                    INTF_NUMf);
            } else
#endif
            {
                l3_intf = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                    L3__INTF_NUMf);
            }
            REPL_L3_INTF_NEXT_HOP_TRILL(unit, l3_intf) = i;
        } else if (entry_type == 7) {
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                l3_intf = soc_EGR_L3_NEXT_HOP_2m_field32_get(unit, egr_nh2_entry,
                    INTF_NUMf);
            } else
#endif
            {
                l3_intf = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                    L3MC__INTF_NUMf);
            }
            REPL_L3_INTF_NEXT_HOP_IPMC(unit, l3_intf) = i;
        } else {
            continue;
        }
    }
    soc_cm_sfree(unit, egr_nh_buf);
    egr_nh_buf = NULL;
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        soc_cm_sfree(unit, egr_nh2_buf);
        egr_nh2_buf = NULL;
    }
#endif

    /* Recover REPL list mode from HW cache */
    rv = _bcm_esw_ipmc_repl_wb_flags_get(unit,
                                         _BCM_IPMC_WB_REPL_LIST, &flags);
    if (flags) {
        SOC_IPMCREPLSHR_SET(unit, 1);
    }

cleanup:
    if (repl_group_buf) {
        soc_cm_sfree(unit, repl_group_buf);
    }
    if (intf_vec) {
        sal_free(intf_vec);
    }

    if (egr_nh_buf) {
        soc_cm_sfree(unit, egr_nh_buf);
    }

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        if (egr_nh2_buf) {
            soc_cm_sfree(unit, egr_nh2_buf);
        }
    }
#endif

    if (BCM_FAILURE(rv)) {
        bcm_th_ipmc_repl_detach(unit);
    }

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     _bcm_th_ipmc_repl_sw_dump
 * Purpose:
 *     Displays IPMC replication information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_th_ipmc_repl_sw_dump(int unit)
{
    _th_repl_info_t *repl_info;
    int pipe;
    bcm_port_t port;
    _th_repl_port_info_t *port_info;
    int i, j;
    _bcm_repl_list_info_t *rli_start, *rli_current;
    SHR_BITDCL *bitmap;
    _th_repl_head_free_block_t *free_block;
    int num_ports = 0;

    LOG_CLI((BSL_META_U(unit,
                        "  IPMC REPL Info -\n")));
    repl_info = _th_repl_info[unit];
    if (repl_info != NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "    Number of Pipelines : %d\n"),
                 repl_info->num_pipes));
        LOG_CLI((BSL_META_U(unit,
                            "    Replication Group Size : %d\n"),
                 repl_info->num_repl_groups));
        LOG_CLI((BSL_META_U(unit,
                            "    Replication Intf Size  : %d\n"),
                 repl_info->num_intf));

        LOG_CLI((BSL_META_U(unit,
                            "    Port Info    -\n")));
        LOG_CLI((BSL_META_U(unit,
                            "    port (index:intf-count) :\n")));
        for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
            port_info = repl_info->port_info[port];
            LOG_CLI((BSL_META_U(unit,
                                "    %3d -"), port));
            if (port_info == NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    " null\n")));
                continue;
            }
            for (i = 0, j = 0; i < repl_info->num_repl_groups; i++) {
                /* If zero, skip print */
                if (port_info->intf_count[i] == 0) {
                    continue;
                }
                if ((j > 0) && !(j % 4)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n         ")));
                }
                LOG_CLI((BSL_META_U(unit,
                                    " %5d:%-5d"), i, port_info->intf_count[i]));
                j++;
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
        }

        for (pipe = 0; pipe < NUM_PIPES(unit); pipe++) {
            rli_start = REPL_LIST_INFO(unit, pipe);
            LOG_CLI((BSL_META_U(unit,
                                "    Pipeline %d List Info -\n"), pipe));
            for (rli_current = rli_start; rli_current != NULL;
                    rli_current = rli_current->next) {
                LOG_CLI((BSL_META_U(unit,
                                    "    Hash:  0x%08x\n"), rli_current->hash));
                LOG_CLI((BSL_META_U(unit,
                                    "    Index: %5d\n"), rli_current->index));
                LOG_CLI((BSL_META_U(unit,
                                    "    Size:  %5d\n"), rli_current->list_size));
                LOG_CLI((BSL_META_U(unit,
                                    "    Refs:  %5d\n"), rli_current->refcount));
            }
        }

        LOG_CLI((BSL_META_U(unit,
                            "    L3 Interface Next Hop IPMC Info -\n")));
        for (i = 0; i < soc_mem_index_count(unit, EGR_L3_INTFm); i++) {
           if (repl_info->l3_intf_next_hop_ipmc[i] == -1) {
              continue;
           }
          LOG_CLI((BSL_META_U(unit,
                              "      L3 Interface %4d: Next Hop Index %5d\n"), i,
                   repl_info->l3_intf_next_hop_ipmc[i]));
        }

        LOG_CLI((BSL_META_U(unit,
                            "    L3 Interface Next Hop Trill Info -\n")));
        for (i = 0; i < soc_mem_index_count(unit, EGR_L3_INTFm); i++) {
           if (repl_info->l3_intf_next_hop_trill[i] == -1) {
              continue;
           }
          LOG_CLI((BSL_META_U(unit,
                              "      L3 Interface %4d: Next Hop Index %5d\n"), i,
                   repl_info->l3_intf_next_hop_trill[i]));
        }
    }

    if (_th_repl_list_entry_info[unit] != NULL) {
        for (pipe = 0; pipe < NUM_PIPES(unit); pipe++) {
            LOG_CLI((BSL_META_U(unit,
                                "    Pipeline %d Replication List Table Size : %d\n"),
                     pipe, _th_repl_list_entry_info[unit][pipe].num_entries));
            LOG_CLI((BSL_META_U(unit,
                                "    Pipeline %d Replication List Table Usage Bitmap (index:value-hex) :"),
                     pipe));
            if (NULL !=
                    _th_repl_list_entry_info[unit][pipe].bitmap_entries_used) {
                bitmap = _th_repl_list_entry_info[unit][pipe].bitmap_entries_used;
                for (i = 0, j = 0;
                        i < _SHR_BITDCLSIZE(_th_repl_list_entry_info[unit][pipe].num_entries);
                        i++) {
                    /* If zero, skip print */
                    if (bitmap[i] == 0) {
                        continue;
                    }
                    if (!(j % 4)) {
                        LOG_CLI((BSL_META_U(unit,
                                            "\n    ")));
                    }
                    LOG_CLI((BSL_META_U(unit,
                                        "  %5d:%8.8x"), i, bitmap[i]));
                    j++;
                }
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
        }
    }

    if (_th_repl_head_info[unit] != NULL) {
        for (pipe = 0; pipe < NUM_PIPES(unit); pipe++) {
            LOG_CLI((BSL_META_U(unit,
                                "    Pipeline %d Replication Head Table Size : %d\n"),
                     pipe, 1 << soc_mem_field_length(unit,
                     MMU_REPL_GROUP_INFO_TBLm, PIPE_BASE_PTRf)));
            if (soc_property_get(unit, spn_RESERVE_MULTICAST_RESOURCES, 0)) {
                num_ports = soc_mem_field_length(unit, MMU_REPL_GROUP_INFO_TBLm,
                        MEMBER_BMPf);
                LOG_CLI(
                    (BSL_META_U(unit,
                                " Unit %d Replication Head reserved entries: %d\n"),
                     unit, num_ports)); 
            } 
            LOG_CLI((BSL_META_U(unit,
                                "    Pipeline %d Replication Head Free List Array:\n"),
                     pipe));
            if (_th_repl_head_info[unit][pipe].free_list_array != NULL) {
                for (i = 0; i < _th_repl_head_info[unit][pipe].array_size; i++) {
                    LOG_CLI((BSL_META_U(unit,
                                        "      Free List %2d:"), i));
                    free_block = REPL_HEAD_FREE_LIST(unit, pipe, i);
                    j = 0;
                    while (NULL != free_block) {
                        if (j > 0 && !(j % 4)) {
                            LOG_CLI((BSL_META_U(unit,
                                                "\n                   ")));
                        }
                        LOG_CLI((BSL_META_U(unit,
                                            "  %7d:%-7d"), free_block->index,
                                 free_block->size));
                        free_block = free_block->next;
                        j++;
                    }
                    LOG_CLI((BSL_META_U(unit,
                                        "\n")));
                }
            }
        }
    }

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/*
  * Function:
  *      _th_ipmc_glp_get
  * Purpose:
  *      Fill information in bcm_ipmc_addr_t struct.
  */
    
bcm_error_t
_th_ipmc_glp_get(int unit, bcm_ipmc_addr_t *ipmc, _bcm_l3_cfg_t *l3cfg)
{

    int no_src_check = FALSE;
    int is_trunk = 0;
    _bcm_l3_cfg_t l3cfg_local;
    int rv = BCM_E_NONE;
    
    l3cfg_local = *l3cfg;  
    if (l3cfg_local.l3c_tunnel) {
        is_trunk = 1;
        if ((((l3cfg_local.l3c_port_tgid & TH_IPMC_NO_SRC_CHECK_PORT) == 
            TH_IPMC_NO_SRC_CHECK_PORT)) &&
            (l3cfg_local.l3c_modid == SOC_MODID_MAX(unit))) {
            no_src_check = TRUE;
            is_trunk = 0;
        }
    }

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        if ((((l3cfg_local.l3c_port_tgid & TH_IPMC_NO_SRC_CHECK_PORT) ==
            TH_IPMC_NO_SRC_CHECK_PORT)) &&
            (l3cfg_local.l3c_modid == SOC_MODID_MAX(unit))) {
            no_src_check = TRUE;
            is_trunk = 0;
        }
    }
#endif /* BCM_TRIDENT3_SUPPORT */

    if (no_src_check) {
        ipmc->ts = 0;
        ipmc->mod_id = -1;
        ipmc->port_tgid = -1;
        ipmc->flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
    } else if (is_trunk) {
        ipmc->ts = 1;
        ipmc->mod_id = 0;
        ipmc->port_tgid = l3cfg_local.l3c_port_tgid;
    } else {
        bcm_module_t    mod_in, mod_out;
        bcm_port_t      port_in, port_out;

        mod_in = l3cfg_local.l3c_modid;
        port_in = l3cfg_local.l3c_port_tgid;
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                     mod_in, port_in,
                                     &mod_out, &port_out));
        /* Check parameters, since above is an application callback */
        if (!SOC_MODID_ADDRESSABLE(unit, mod_out)) {
            return BCM_E_BADID;
        }
        if (!SOC_PORT_ADDRESSABLE(unit, port_out)) {
            return BCM_E_PORT;
        }        
        ipmc->ts = 0;
        ipmc->mod_id = mod_out;
        ipmc->port_tgid = port_out;
    }    

    return rv;
}

/*
 * Function:
 *      _th_ipmc_info_get
 * Purpose:
 *      Fill information in bcm_ipmc_addr_t struct.
 */

bcm_error_t
_th_ipmc_info_get(int unit, int ipmc_index, bcm_ipmc_addr_t *ipmc, 
                  ipmc_entry_t *entry, _bcm_esw_ipmc_l3entry_t *use_ipmc_l3entry)
{

    ipmc->v = soc_L3_IPMCm_field32_get(unit, entry, VALIDf);

    if (ipmc->v) {
        _bcm_l3_cfg_t l3cfg;
        _bcm_esw_ipmc_l3entry_t *ipmc_l3entry;

        if (use_ipmc_l3entry != NULL) {
            /* use the passed in l3 info */
            ipmc_l3entry = use_ipmc_l3entry;
        } else {
            /* Note: this simply picks up the first l3 info */
            ipmc_l3entry = IPMC_GROUP_INFO(unit, ipmc_index)->l3entry_list;
            if (NULL == ipmc_l3entry) {
                /* No entries in Multicast host table */
                return BCM_E_EMPTY;
            }
        }
        sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));
        l3cfg.l3c_flags = BCM_L3_IPMC;
        l3cfg.l3c_vrf = ipmc_l3entry->l3info.vrf;
        l3cfg.l3c_vid = ipmc_l3entry->l3info.vid;
        l3cfg.l3c_ing_intf = ipmc_l3entry->l3info.ing_intf;

        if (ipmc_l3entry->ip6) {
            ipmc->flags |= BCM_IPMC_IP6;
        } else {
            ipmc->flags &= ~BCM_IPMC_IP6;
        }
        if (ipmc->flags & BCM_IPMC_HIT_CLEAR) {
            l3cfg.l3c_flags |= BCM_L3_HIT_CLEAR;
        }

        if (ipmc_l3entry->l3info.flags & BCM_L3_L2ONLY) {
            l3cfg.l3c_flags |= BCM_L3_L2ONLY;
            ipmc->flags |= BCM_IPMC_L2;
        }
        
        /* need to get current l3 info from h/w like hit bits */
        if (ipmc->flags & BCM_IPMC_IP6) {
            sal_memcpy(ipmc->s_ip6_addr, &ipmc_l3entry->l3info.sip6, BCM_IP6_ADDRLEN);
            sal_memcpy(ipmc->mc_ip6_addr, &ipmc_l3entry->l3info.ip6, BCM_IP6_ADDRLEN);
            sal_memcpy(l3cfg.l3c_sip6, &ipmc_l3entry->l3info.sip6, BCM_IP6_ADDRLEN);
            sal_memcpy(l3cfg.l3c_ip6, &ipmc_l3entry->l3info.ip6, BCM_IP6_ADDRLEN);
            l3cfg.l3c_flags |= BCM_L3_IP6;
            BCM_IF_ERROR_RETURN(mbcm_driver[unit]->mbcm_l3_ip6_get(unit, &l3cfg));
        } else {
            ipmc->s_ip_addr = ipmc_l3entry->l3info.src_ip_addr;
            ipmc->mc_ip_addr = ipmc_l3entry->l3info.ipmc_group;
            l3cfg.l3c_src_ip_addr = ipmc_l3entry->l3info.src_ip_addr;
            l3cfg.l3c_ipmc_group = ipmc_l3entry->l3info.ipmc_group;
            BCM_IF_ERROR_RETURN(mbcm_driver[unit]->mbcm_l3_ip4_get(unit, &l3cfg));
        }
        
        if (l3cfg.l3c_flags & BCM_L3_HIT) {
            ipmc->flags |= BCM_IPMC_HIT;
        }

        if (l3cfg.l3c_flags & BCM_L3_DST_DISCARD) {
            ipmc->flags |= BCM_IPMC_DISCARD;
        }

        if (l3cfg.l3c_flags & BCM_IPMC_POST_LOOKUP_RPF_CHECK) {
            ipmc->flags |= BCM_IPMC_POST_LOOKUP_RPF_CHECK;
            ipmc->l3a_intf = l3cfg.l3c_intf;
            if (l3cfg.l3c_flags & BCM_IPMC_RPF_FAIL_DROP) {
                ipmc->flags |= BCM_IPMC_RPF_FAIL_DROP;
            }
            if (l3cfg.l3c_flags & BCM_IPMC_RPF_FAIL_TOCPU) {
                ipmc->flags |= BCM_IPMC_RPF_FAIL_TOCPU;
            }
        }

        if (ipmc_l3entry->l3info.flags & BCM_L3_RPE) {
            ipmc->cos = ipmc_l3entry->l3info.prio;
            ipmc->flags |= BCM_IPMC_SETPRI;
        } else {
            ipmc->cos = -1;
            ipmc->flags &= ~BCM_IPMC_SETPRI;
        }
        ipmc->group = ipmc_index;
        ipmc->group_l2 = ipmc_l3entry->l3info.ipmc_ptr_l2;
        ipmc->lookup_class = ipmc_l3entry->l3info.lookup_class;
        ipmc->vrf = ipmc_l3entry->l3info.vrf;
        ipmc->vid = ipmc_l3entry->l3info.vid;
        ipmc->ing_intf = ipmc_l3entry->l3info.ing_intf;
        ipmc->rp_id = ipmc_l3entry->l3info.rp_id;
        BCM_IF_ERROR_RETURN
            (_th_ipmc_glp_get(unit, ipmc, &l3cfg));
 
    }
    
    return BCM_E_NONE;
}

/*
  * Function:
  *      _th_ipmc_glp_set
  * Purpose:
  *      Fill information in bcm_ipmc_addr_t struct.
  */
    
bcm_error_t
_th_ipmc_glp_set(int unit, bcm_ipmc_addr_t *ipmc, _bcm_l3_cfg_t *l3cfg)
{

    int  mod, port_tgid, is_trunk, no_src_check = FALSE;
    int rv = BCM_E_NONE;

    if ((ipmc->flags & BCM_IPMC_SOURCE_PORT_NOCHECK) ||
        (ipmc->port_tgid < 0)) {                        /* no source port */
        no_src_check = TRUE;
        is_trunk = 0;
        mod = SOC_MODID_MAX(unit);
        port_tgid = TH_IPMC_NO_SRC_CHECK_PORT;
    } else if (ipmc->ts) {                              /* trunk source port */
        is_trunk = 1;
        mod = 0;
        port_tgid = ipmc->port_tgid;
    } else {                                            /* source port */
        bcm_module_t    mod_in, mod_out;
        bcm_port_t      port_in, port_out;

        mod_in = ipmc->mod_id;
        port_in = ipmc->port_tgid;
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                    mod_in, port_in,
                                    &mod_out, &port_out));
        /* Check parameters, since above is an application callback */
        if (!SOC_MODID_ADDRESSABLE(unit, mod_out)) {
            return BCM_E_BADID;
        }
        if (!SOC_PORT_ADDRESSABLE(unit, port_out)) {
            return BCM_E_PORT;
        }
        is_trunk = 0;
        mod = mod_out;
        port_tgid = port_out;
    }

    if (is_trunk) {
        l3cfg->l3c_tunnel = 1;
        l3cfg->l3c_modid = 0;
        l3cfg->l3c_port_tgid = port_tgid;
    } else {
        l3cfg->l3c_modid = mod;
        if (no_src_check) {
            l3cfg->l3c_tunnel = 1;
            if (SOC_IS_TRIDENT3X(unit)) {
                l3cfg->l3c_tunnel = 0;
            }
        } else {
            l3cfg->l3c_tunnel = 0;
        }
            l3cfg->l3c_port_tgid = port_tgid;            
    }

    return rv;
}


/*
 * Function:
 *      _bcm_th_ipmc_l3_intf_next_hop_get
 * Purpose:
 *      Get the next hop index that was silently allocated to the L3 interface.
 * Parameters:
 *      unit - (IN) SOC unit #
 *      intf - (IN) L3 interface ID
 *      nh_index - (OUT) Next hop index
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_ipmc_l3_intf_next_hop_get(int unit, int intf, int *nh_index)
{
    *nh_index = REPL_L3_INTF_NEXT_HOP_IPMC(unit, intf);

    if (*nh_index == REPL_NH_INDEX_UNALLOCATED ||
        *nh_index == REPL_NH_INDEX_L3_EGRESS_ALLOCATED) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ipmc_l3_intf_next_hop_l3_egress_clear
 * Purpose:
 *      Inform the IPMC module that a next hop index has been deleted
 *      for the given L3 interface by the bcm_l3_egress_destroy API.
 * Parameters:
 *      unit - SOC device unit number
 *      nh_idx - next hop index
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_XXX               ERROR
 */
int _bcm_th_ipmc_l3_intf_next_hop_l3_egress_clear(int unit, int nh_idx)
{
    soc_mem_t mem;                        /* Table location memory. */
    egr_l3_next_hop_entry_t  egr_entry;   /* Egress next hop entry. */
    int ent_type, intf_num;

    mem = EGR_L3_NEXT_HOPm;
    sal_memset(&egr_entry, 0, sizeof(egr_l3_next_hop_entry_t));

    SOC_IF_ERROR_RETURN(BCM_XGS3_MEM_READ(unit, EGR_L3_NEXT_HOPm,
                                          nh_idx, &egr_entry));

    if (soc_feature(unit, soc_feature_mem_access_data_type)) {
        ent_type = soc_mem_field32_get(unit, mem, &egr_entry, DATA_TYPEf);
    } else {
        ent_type = soc_mem_field32_get(unit, mem, &egr_entry, ENTRY_TYPEf);
    }

    /* chk if the entry is L3MC */
    if (ent_type == 7) {
        intf_num = soc_mem_field32_get
            (unit, mem, &egr_entry, L3MC__INTF_NUMf);
        if (REPL_L3_INTF_NEXT_HOP_IPMC(unit, intf_num)  ==
                         REPL_NH_INDEX_L3_EGRESS_ALLOCATED) {
            REPL_L3_INTF_NEXT_HOP_IPMC(unit, intf_num) =
                REPL_NH_INDEX_UNALLOCATED;
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th_ipmc_l3_intf_next_hop_l3_egress_set
 * Purpose:
 *      Inform the IPMC module that a next hop index has been allocated
 *      for the given L3 interface by the bcm_l3_egress_create API.
 * Parameters:
 *      unit - (IN) SOC unit #
 *      intf - (IN) L3 interface ID
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_ipmc_l3_intf_next_hop_l3_egress_set(int unit, int intf)
{
    int nh_index;

    nh_index = REPL_L3_INTF_NEXT_HOP_IPMC(unit, intf);

    if (nh_index == REPL_NH_INDEX_UNALLOCATED) {
        REPL_L3_INTF_NEXT_HOP_IPMC(unit, intf) =
            REPL_NH_INDEX_L3_EGRESS_ALLOCATED;
    } else if (nh_index == REPL_NH_INDEX_L3_EGRESS_ALLOCATED) {
        /* Do nothing. Users are allowed to create multiple L3
         * egress objects based on the same L3 interface.
         */
    } else {
        /* IPMC module had already silently allocated a next hop index
         * for this L3 interface.
         */
        return BCM_E_CONFIG;
    }
        
    return BCM_E_NONE;
}

STATIC int
_bcm_th_ipmc_egress_intf_set_for_trunk_first_member(
    int unit, int repl_group, bcm_port_t port,
    int if_count, bcm_if_t *if_array, int is_l3,
    int check_port, bcm_trunk_t tgid)
{
    int old_intf_count, new_intf_count;
    int aggid;
    bcm_port_t local_port = port;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if ((soc_feature(unit, soc_feature_channelized_switching)) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {

        /* Now get the subport number associated with this sub-port. */
        BCM_IF_ERROR_RETURN(
            _bcm_coe_subtag_subport_port_subport_num_get(unit,
            port, NULL, (int *)&local_port));
    }
#endif


    old_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, local_port, repl_group);
    if ((old_intf_count == 0) && (if_count > 0)) {
        BCM_IF_ERROR_RETURN(bcm_th_port_aggid_add(unit, port, tgid, &aggid));
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_ipmc_egress_intf_set(
            unit, repl_group, port, if_count, if_array, is_l3, check_port));

    new_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, local_port, repl_group);
    if ((old_intf_count > 0) && (new_intf_count == 0)) {
        BCM_IF_ERROR_RETURN(bcm_th_port_aggid_del(unit, port));
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_th_ipmc_egress_intf_set_for_same_pipe_member(
    int unit, int repl_group, bcm_port_t port,
    bcm_port_t first_port, bcm_trunk_t tgid)
{
    int aggid;
    int old_intf_count, new_intf_count;
    uint32 first_local_port = first_port;
    uint32 old_local_port = port;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if ((soc_feature(unit, soc_feature_channelized_switching)) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {

        /* Now get the subport number associated with this sub-port. */
        BCM_IF_ERROR_RETURN(
            _bcm_coe_subtag_subport_port_subport_num_get(unit,
            port, NULL, (int *)&old_local_port));
    }

    if ((soc_feature(unit, soc_feature_channelized_switching)) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, first_port)) {

        /* Now get the subport number associated with this sub-port. */
        BCM_IF_ERROR_RETURN(
            _bcm_coe_subtag_subport_port_subport_num_get(unit,
            first_port, NULL, (int *)&first_local_port));
    }
#endif

    old_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, old_local_port, repl_group);
    new_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, first_local_port, repl_group);
    if (old_intf_count == new_intf_count) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_th_repl_initial_copy_count_set(unit, repl_group,
                                             port, new_intf_count));

    if ((old_intf_count == 0) && (new_intf_count > 0)) {
        BCM_IF_ERROR_RETURN(bcm_th_port_aggid_add(unit, port, tgid, &aggid));
    } else if ((old_intf_count > 0) && (new_intf_count == 0)) {
        BCM_IF_ERROR_RETURN(bcm_th_port_aggid_del(unit, port));
    }

    REPL_PORT_GROUP_INTF_COUNT(unit, old_local_port, repl_group) = new_intf_count;

    return BCM_E_NONE;
}

STATIC int
_bcm_th_ipmc_egress_intf_add_in_per_trunk_mode(int unit, int repl_group,
    bcm_port_t port, int encap_id, int is_l3)
{
    int aggid;
    int port_num = port;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if ((soc_feature(unit, soc_feature_channelized_switching)) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {

        /* Now get the subport number associated with this sub-port. */
        BCM_IF_ERROR_RETURN(
            _bcm_coe_subtag_subport_port_subport_num_get(unit,
            port, NULL, (int *)&port_num));
    }
#endif

    if (REPL_PORT_GROUP_INTF_COUNT(unit, port_num, repl_group) == 0) {
        BCM_IF_ERROR_RETURN(
            bcm_th_port_aggid_add(unit, port, BCM_TRUNK_INVALID, &aggid));
    }

    BCM_IF_ERROR_RETURN(
        _bcm_th_ipmc_egress_intf_add(
            unit, repl_group, port, encap_id, is_l3));

    return BCM_E_NONE;
}

STATIC int
_bcm_th_ipmc_egress_intf_del_in_per_trunk_mode(int unit, int repl_group,
    bcm_port_t port, int if_max, int encap_id, int is_l3)
{

    int old_intf_count, new_intf_count;
    bcm_port_t local_port = port;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if ((soc_feature(unit, soc_feature_channelized_switching)) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {

        /* Now get the subport number associated with this sub-port. */
        BCM_IF_ERROR_RETURN(
            _bcm_coe_subtag_subport_port_subport_num_get(unit,
            port, NULL, (int *)&local_port));
    }
#endif


    old_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, local_port, repl_group);
    BCM_IF_ERROR_RETURN(
        _bcm_th_ipmc_egress_intf_delete(
            unit, repl_group, port, if_max, encap_id, is_l3));
    new_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, local_port, repl_group);
    if (new_intf_count == 0 && old_intf_count > 0) {
        BCM_IF_ERROR_RETURN(bcm_th_port_aggid_del(unit, port));
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_th_ipmc_egress_intf_add_for_trunk(int unit, int repl_group,
    bcm_trunk_t tgid, int encap_id, int is_l3)
{
#if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
    bcm_port_t      trunk_local_member_ports[SOC_MAX_NUM_PP_PORTS] = {0};
    int             max_num_ports = SOC_MAX_NUM_PP_PORTS;
#else
    bcm_port_t      trunk_local_member_ports[SOC_MAX_NUM_PORTS] = {0};
    int             max_num_ports = SOC_MAX_NUM_PORTS;
#endif
    bcm_port_t port;

    int idx;
    int member_count, port_iter;
    bcm_port_t trunk_pipe_first_port_array[BCM_PIPES_MAX];
    int aggid, pipe, pipe_port;

    BCM_IF_ERROR_RETURN(
            _bcm_esw_trunk_local_members_get(unit, tgid, max_num_ports,
                                         trunk_local_member_ports,
                                         &member_count));

    for (idx = 0; idx < BCM_PIPES_MAX; idx++) {
        trunk_pipe_first_port_array[idx] = -1;
    }
    for (idx = 0; idx < member_count; idx++) {
        port_iter = trunk_local_member_ports[idx];
        port = port_iter;
        pipe_port = port_iter;
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
        /* Get the physical port associated with the subport. */
        if ((soc_feature(unit, soc_feature_channelized_switching)) &&
            BCM_GPORT_IS_SET(port) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port_iter)) {

            BCM_IF_ERROR_RETURN(
                _bcmi_coe_subport_physical_port_get(unit, port_iter, &pipe_port));

            /* Now get the subport number associated with this sub-port. */
            BCM_IF_ERROR_RETURN(
                _bcm_coe_subtag_subport_port_subport_num_get(unit,
                port_iter, NULL, (int *)&port));
        }
#endif
        pipe = SOC_INFO(unit).port_pipe[pipe_port];

        if (trunk_pipe_first_port_array[pipe] == -1) {
            if (REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group) == 0) {
                BCM_IF_ERROR_RETURN(
                    bcm_th_port_aggid_add(unit, port_iter, tgid, &aggid));
            }
            BCM_IF_ERROR_RETURN(
                _bcm_th_ipmc_egress_intf_add(
                    unit, repl_group, port_iter, encap_id, is_l3));
            trunk_pipe_first_port_array[pipe] = port_iter;
        } else {
            BCM_IF_ERROR_RETURN(
                _bcm_th_ipmc_egress_intf_set_for_same_pipe_member(
                    unit, repl_group, port_iter,
                    trunk_pipe_first_port_array[pipe], tgid));

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
            /* Get the physical port associated with the subport. */
            if ((soc_feature(unit, soc_feature_subport_forwarding_support)) &&
                BCM_GPORT_IS_SET(port) &&
                _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port_iter)) {

                BCM_IF_ERROR_RETURN(
                    bcmi_subport_multicast_lc_pbm_set(unit, repl_group,
                    port_iter, 1));

            }
#endif
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_th_ipmc_egress_intf_del_for_trunk(int unit, int repl_group,
    bcm_trunk_t tgid, int if_max, int encap_id, int is_l3)
{
#if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
    bcm_port_t      trunk_local_member_ports[SOC_MAX_NUM_PP_PORTS] = {0};
    int             max_num_ports = SOC_MAX_NUM_PP_PORTS;
#else
    bcm_port_t      trunk_local_member_ports[SOC_MAX_NUM_PORTS] = {0};
    int             max_num_ports = SOC_MAX_NUM_PORTS;
#endif
    bcm_port_t port;

    int idx;
    int member_count, port_iter;
    bcm_port_t trunk_pipe_first_port_array[BCM_PIPES_MAX];
    int pipe, pipe_port;
    int old_intf_count, new_intf_count;

    BCM_IF_ERROR_RETURN(
            _bcm_esw_trunk_local_members_get(unit, tgid, max_num_ports,
                                         trunk_local_member_ports,
                                         &member_count));

    for (idx = 0; idx < BCM_PIPES_MAX; idx++) {
        trunk_pipe_first_port_array[idx] = -1;
    }

    for (idx = 0; idx < member_count; idx++) {
        port_iter = trunk_local_member_ports[idx];
        port = port_iter;
        pipe_port = port;
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)

        /* Get the physical port associated with the subport. */
        if ((soc_feature(unit, soc_feature_channelized_switching)) &&
            BCM_GPORT_IS_SET(port) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port_iter)) {

            BCM_IF_ERROR_RETURN(
                _bcmi_coe_subport_physical_port_get(unit, port_iter, &pipe_port));

            /* Now get the subport number associated with this sub-port. */
            BCM_IF_ERROR_RETURN(
                _bcm_coe_subtag_subport_port_subport_num_get(unit,
                port_iter, NULL, (int *)&port));
        }
#endif
        pipe = SOC_INFO(unit).port_pipe[pipe_port];
        if (trunk_pipe_first_port_array[pipe] == -1) {
            old_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group);
            BCM_IF_ERROR_RETURN(
                _bcm_th_ipmc_egress_intf_delete(
                     unit, repl_group, trunk_local_member_ports[idx],
                     if_max, encap_id, is_l3));
            new_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group);
            if (new_intf_count == 0 && old_intf_count > 0) {
                BCM_IF_ERROR_RETURN(
                    bcm_th_port_aggid_del(unit, port_iter));
            }
            trunk_pipe_first_port_array[pipe] = port_iter;
        } else {
            BCM_IF_ERROR_RETURN(
                _bcm_th_ipmc_egress_intf_set_for_same_pipe_member(
                    unit, repl_group, port_iter,
                    trunk_pipe_first_port_array[pipe], tgid));

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
            /* Get the physical port associated with the subport. */
            if ((soc_feature(unit, soc_feature_subport_forwarding_support)) &&
                BCM_GPORT_IS_SET(port) &&
                _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port_iter)) {

                BCM_IF_ERROR_RETURN(
                    bcmi_subport_multicast_lc_pbm_set(unit, repl_group,
                    port_iter, 0));

            }
#endif
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_th_ipmc_egress_intf_add_trunk_member(int unit, int repl_group,
                                           bcm_port_t port)
{
#if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
    bcm_port_t      trunk_local_member_ports[SOC_MAX_NUM_PP_PORTS] = {0};
    int             max_num_ports = SOC_MAX_NUM_PP_PORTS;
#else
    bcm_port_t      trunk_local_member_ports[SOC_MAX_NUM_PORTS] = {0};
    int             max_num_ports = SOC_MAX_NUM_PORTS;
#endif

    int local_modid;
    int trunk_local_ports;
    int intf_count, if_count, old_intf_count;
    int same_pipe_port = -1, diff_pipe_port = -1;
    int i, rv;
    bcm_port_t port_iter;
    bcm_if_t *if_array = NULL;
    bcm_trunk_t tgid;
    int aggid;

    /* Member port belongs to an existing trunk, it needs to share
     * identical replication list as other members of this trunk in
     * same pipe.
     */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get (unit, &local_modid));
    BCM_IF_ERROR_RETURN(
        bcm_esw_trunk_find(unit, local_modid, port, &tgid));
    BCM_IF_ERROR_RETURN(
        _bcm_esw_trunk_local_members_get(unit, tgid, max_num_ports,
                                         trunk_local_member_ports,
                                         &trunk_local_ports));
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        /* Only single pipe available here. */
        for (i=0; i<trunk_local_ports; i++) {
            if (trunk_local_member_ports[i] != port) {
                same_pipe_port = trunk_local_member_ports[i];
            }
        }
    } else
#endif
    {
    for (i = 0; i < trunk_local_ports; i++) {
        port_iter = trunk_local_member_ports[i];
        if (port != port_iter) {
            intf_count =
                REPL_PORT_GROUP_INTF_COUNT(unit, port_iter, repl_group);
            if (intf_count > 0) {
                if (SOC_INFO(unit).port_pipe[port] ==
                    SOC_INFO(unit).port_pipe[port_iter]) {
                    same_pipe_port = port_iter;
                    break;
                } else {
                    if (diff_pipe_port == -1) {
                        diff_pipe_port = port_iter;
                    }
                }
            }
        }
    }
    }

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        if(same_pipe_port == -1 && diff_pipe_port == -1) {
            return BCM_E_PARAM;
        }
        if (same_pipe_port != -1) {
            BCM_IF_ERROR_RETURN(
                _bcm_th3_ipmc_egress_intf_set_for_same_trunk_member(
                        unit, repl_group, port, same_pipe_port, tgid));
        } else {
             BCM_IF_ERROR_RETURN(
                _bcm_th3_ipmc_egress_intf_set_for_same_trunk_member(
                    unit, repl_group, port, diff_pipe_port, tgid));

        }
        return BCM_E_NONE;
    }
#endif

    /* or return BCM_E_PARAM jinghuan */
    if (same_pipe_port == -1 && diff_pipe_port == -1) {
        return BCM_E_NONE;
    }

    if (same_pipe_port != -1) {
        BCM_IF_ERROR_RETURN(
            _bcm_th_ipmc_egress_intf_set_for_same_pipe_member(
                unit, repl_group, port, same_pipe_port, tgid));
    } else {
        intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, diff_pipe_port,
                                                repl_group);
        if_array = sal_alloc(sizeof(bcm_if_t) * intf_count,
                             "if_array pointers");
        if (if_array == NULL) {
            return BCM_E_MEMORY;
        }

        rv = bcm_th_ipmc_egress_intf_get(unit, repl_group, diff_pipe_port,
                                          intf_count, if_array, &if_count);
        if (BCM_FAILURE(rv)) {
            sal_free(if_array);
            return rv;
        }

        old_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group);
        if ((old_intf_count == 0) && (intf_count > 0)) {
            rv = bcm_th_port_aggid_add(unit, port, tgid, &aggid);
            if (BCM_FAILURE(rv)) {
                sal_free(if_array);
                return rv;
            }
        }
        rv = _bcm_esw_ipmc_egress_intf_set(unit, repl_group, port,
                                           if_count, if_array, TRUE, FALSE);
        if (BCM_FAILURE(rv)) {
            sal_free(if_array);
            return rv;
        }
        sal_free(if_array);
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_th_ipmc_egress_intf_del_trunk_member(int unit, int repl_group,
                                           bcm_port_t port)
{
    int is_last_member;
    int old_intf_count;
    int local_port = port;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)

    if ((soc_feature(unit, soc_feature_channelized_switching)) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {

        /* Now get the subport number associated with this sub-port. */
        BCM_IF_ERROR_RETURN(
            _bcm_coe_subtag_subport_port_subport_num_get(unit,
            port, NULL, (int *)&local_port));

    }
#endif

    BCM_IF_ERROR_RETURN(
        bcm_th_port_last_member_check(unit, port, &is_last_member));

    old_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, local_port, repl_group);
    if (is_last_member) {
        BCM_IF_ERROR_RETURN(
            bcm_th_ipmc_egress_intf_set(
                unit, repl_group, port, 0, NULL, TRUE, FALSE));
    } else {
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group) = 0;
        } else
#endif
        {
        BCM_IF_ERROR_RETURN(
            _bcm_th_repl_initial_copy_count_set(
                unit, repl_group, port, 0));
        REPL_PORT_GROUP_INTF_COUNT(unit, local_port, repl_group) = 0;
    }
    }

    if (old_intf_count > 0) {
        BCM_IF_ERROR_RETURN(bcm_th_port_aggid_del(unit, port));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_ipmc_egress_intf_set_for_trunk_first_member
 * Purpose:
 *      Assign set of egress interfaces to the first port of a trunk.
 *      or to the only one non-trunk port.
 * Parameters:
 *  unit       - (IN) StrataSwitch PCI device unit number.
 *  repl_group - (IN) The replication group number.
 *  port       - (IN) Port to assign replication list.
 *  if_count   - (IN) Number of interfaces in replication list.
 *  if_array   - (IN) List of interface numbers.
 *  if_updated - (IN) interfaces info to be updated including
 *                    interfaces which is new to the group and port
 *                    and interfaces to be deleted.
 *  is_l3      - (IN) Indicates if multicast group is of type IPMC.
 *  check_port - (IN) If if_array consists of L3 interfaces, this parameter
 *                    controls whether to check the given port is a member
 *                    in each L3 interface's VLAN. This check should be
 *                    disabled when not needed, in order to improve
 *                    performance.
 *  tgid       - (IN) trunk id or BCM_TRUNK_INVALID for non-trunk port.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_ipmc_egress_intf_set_for_trunk_first_member(
    int unit, int repl_group, bcm_port_t port,
    int if_count, bcm_if_t *if_array, int is_l3,
    int check_port, bcm_trunk_t tgid)
{
    int rv = BCM_E_NONE;

    IPMC_LOCK(unit);
    rv = _bcm_th_ipmc_egress_intf_set_for_trunk_first_member(
             unit, repl_group, port, if_count, if_array,
             is_l3, check_port, tgid);
    IPMC_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_th_ipmc_egress_intf_set_for_same_pipe_member
 * Purpose:
 *      Align egress intf of same pipe port with the first port of trunk.
 * Parameters:
 *  unit       - (IN) StrataSwitch PCI device unit number.
 *  repl_group - (IN) The replication group number.
 *  port       - (IN) Port to assign replication list.
 *  first_port - (IN) The first member port of the trunk.
 *  tgid       - (IN) trunk id.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_ipmc_egress_intf_set_for_same_pipe_member(
    int unit, int repl_group, bcm_port_t port,
    bcm_port_t first_port, bcm_trunk_t tgid)
{
    int rv = BCM_E_NONE;

    IPMC_LOCK(unit);
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        rv = _bcm_th3_ipmc_egress_intf_set_for_same_trunk_member(
             unit, repl_group, port, first_port, tgid);
    } else
#endif
    {
    rv = _bcm_th_ipmc_egress_intf_set_for_same_pipe_member(
             unit, repl_group, port, first_port, tgid);
    }
    IPMC_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_th_ipmc_egress_intf_add_in_per_trunk_mode
 * Purpose:
 *      Add encap ID to selected ports' replication list for chosen
 *      IPMC group in mc per trunk repl mode.
 *      It is used for non-trunk port.
 * Parameters:
 *	unit       - (IN) StrataSwitch PCI device unit number.
 *	repl_group - (IN) The replication group number.
 *	port       - (IN) Port to assign replication list.
 *  encap_id   - (IN) Encap ID.
 *  is_l3      - (IN) Indicates if multicast group type is IPMC.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_ipmc_egress_intf_add_in_per_trunk_mode(int unit, int repl_group,
    bcm_port_t port, int encap_id, int is_l3)
{
    int rv = BCM_E_NONE;

    IPMC_LOCK(unit);
    rv = _bcm_th_ipmc_egress_intf_add_in_per_trunk_mode(
             unit, repl_group, port, encap_id, is_l3);
    IPMC_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_th_ipmc_egress_intf_del_in_per_trunk_mode
 * Purpose:
 *      Remove encap ID from selected ports' replication list for chosen
 *      IPMC group in mc per trunk repl mode.
 *      It is used for non-trunk port.
 * Parameters:
 *	unit       - (IN) StrataSwitch PCI device unit number.
 *	repl_group - (IN) The replication group number.
 *	port       - (IN) Port to assign replication list.
 *  if_max     - (IN) Maximal interface.
 *  encap_id   - (IN) Encap ID.
 *  is_l3      - (IN) Indicates if multicast group type is IPMC.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_ipmc_egress_intf_del_in_per_trunk_mode(int unit, int repl_group,
    bcm_port_t port, int if_max, int encap_id, int is_l3)
{
    int rv = BCM_E_NONE;

    IPMC_LOCK(unit);
    rv = _bcm_th_ipmc_egress_intf_del_in_per_trunk_mode(
             unit, repl_group, port, if_max, encap_id, is_l3);
    IPMC_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_th_ipmc_egress_intf_add_for_trunk
 * Purpose:
 *      Add encap ID to a trunk.
 * Parameters:
 *	unit       - (IN) StrataSwitch PCI device unit number.
 *	repl_group - (IN) The replication group number.
 *	tgid       - (IN) Trunk ID.
 *  encap_id   - (IN) Encap ID.
 *  is_l3      - (IN) Indicates if multicast group type is IPMC.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_ipmc_egress_intf_add_for_trunk(int unit, int repl_group,
    bcm_trunk_t tgid, int encap_id, int is_l3)
{
    int rv = BCM_E_NONE;

    IPMC_LOCK(unit);
#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        rv = _bcm_th3_ipmc_egress_intf_add_for_trunk(
             unit, repl_group, tgid, encap_id, is_l3);

    } else
#endif
    {
    rv = _bcm_th_ipmc_egress_intf_add_for_trunk(
             unit, repl_group, tgid, encap_id, is_l3);
    }
    IPMC_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_th_ipmc_egress_intf_del_for_trunk
 * Purpose:
 *      Remove encap ID from a trunk.
 * Parameters:
 *	unit       - (IN) StrataSwitch PCI device unit number.
 *	repl_group - (IN) The replication group number.
 *	tgid       - (IN) Trunk ID.
 *  if_max     - (IN) Maximal interface.
 *  encap_id   - (IN) Encap ID.
 *  is_l3      - (IN) Indicates if multicast group type is IPMC.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_ipmc_egress_intf_del_for_trunk(int unit, int repl_group,
    bcm_trunk_t tgid, int if_max, int encap_id, int is_l3)
{
    int rv = BCM_E_NONE;

    IPMC_LOCK(unit);

#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        rv = _bcm_th3_ipmc_egress_intf_del_for_trunk(
             unit, repl_group, tgid, if_max, encap_id, is_l3);
    } else
#endif
    {
    rv = _bcm_th_ipmc_egress_intf_del_for_trunk(
             unit, repl_group, tgid, if_max, encap_id, is_l3);
    }
    IPMC_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_th_ipmc_egress_intf_add_trunk_member
 * Purpose:
 *      Add a port member to a trunk for chosen IPMC group.
 * Parameters:
 *	unit       - (IN) StrataSwitch PCI device unit number.
 *	repl_group - (IN) The replication group number.
 *	port       - (IN) Trunk member port.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_ipmc_egress_intf_add_trunk_member(int unit, int repl_group,
                                           bcm_port_t port)
{
    int rv = BCM_E_NONE;

    IPMC_LOCK(unit);
    rv = _bcm_th_ipmc_egress_intf_add_trunk_member(
             unit, repl_group, port);
    IPMC_UNLOCK(unit);

    return rv;
}


/*
 * Function:
 *      bcm_th_ipmc_egress_intf_del_trunk_member
 * Purpose:
 *      Remove a port member from a trunk for chosen IPMC group.
 * Parameters:
 *	unit       - (IN) StrataSwitch PCI device unit number.
 *	repl_group - (IN) The replication group number.
 *	port       - (IN) Trunk member port.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_ipmc_egress_intf_del_trunk_member(int unit, int repl_group,
                                           bcm_port_t port)
{
    int rv = BCM_E_NONE;

    IPMC_LOCK(unit);
    rv = _bcm_th_ipmc_egress_intf_del_trunk_member(
             unit, repl_group, port);
    IPMC_UNLOCK(unit);

    return rv;
}

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
/*
 * Function:
 *      bcmi_ipmc_chn_repl_port_detach
 * Purpose:
 *      Detach channelization replication ports.
 * Parameters:
 *  unit       - (IN) StrataSwitch PCI device unit number.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_ipmc_chn_repl_port_detach(int unit)
{
    int port = FB6_NUM_PORT;
    int num_ports = FB6_TOTAL_NUM_SUBPORT;

    if (_th_repl_info[unit]->port_info == NULL) {
        return BCM_E_INIT;
    }

    for (; port < num_ports; port++) {
        if (_th_repl_info[unit]->port_info[port] != NULL) {
            if (_th_repl_info[unit]->
                    port_info[port]->intf_count != NULL) {
                sal_free(_th_repl_info[unit]->
                        port_info[port]->intf_count);
                _th_repl_info[unit]->port_info[port]->intf_count = NULL;
            }
            sal_free(_th_repl_info[unit]->port_info[port]);
            _th_repl_info[unit]->port_info[port] = NULL;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_ipmc_chn_repl_port_attach
 * Purpose:
 *      Attach channelization replication ports.
 * Parameters:
 *  unit       - (IN) StrataSwitch PCI device unit number.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_ipmc_chn_repl_port_attach(int unit)
{
    int port = FB6_NUM_PORT;
    int alloc_size = 0;

    if (_th_repl_info[unit]->port_info == NULL) {
        return BCM_E_INIT;
    }

    for (; port<FB6_TOTAL_NUM_SUBPORT; port++) {

        if (_th_repl_info[unit]->port_info[port] != NULL) {
            continue;
        }

        _th_repl_info[unit]->port_info[port] =
                    sal_alloc(sizeof(_th_repl_port_info_t), "repl port info");
        if (NULL == _th_repl_info[unit]->port_info[port]) {
            (void) bcmi_ipmc_chn_repl_port_detach(unit);
            return BCM_E_MEMORY;
        }

        sal_memset(_th_repl_info[unit]->port_info[port], 0,
                                                sizeof(_th_repl_port_info_t));

        alloc_size = sizeof(int) * _th_repl_info[unit]->num_repl_groups;
        _th_repl_info[unit]->port_info[port]->intf_count =
            sal_alloc(alloc_size, "repl port intf count");
        if (NULL == _th_repl_info[unit]->port_info[port]->intf_count) {
            (void) bcmi_ipmc_chn_repl_port_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(_th_repl_info[unit]->port_info[port]->intf_count, 0, alloc_size);
    }

#if 0
    total_mc_groups = soc_mem_index_count(unit, L3_IPMCm);

    alloc_size = sizeof(bcmi_mc_group_chnl_info_t) * total_mc_groups;
    bcmi_mc_group_chnl_info[unit] =
        sal_alloc(alloc_size, "repl port info");
        if (NULL == bcmi_mc_group_chnl_info[unit]) {
            (void) bcmi_ipmc_chn_repl_port_detach(unit);
            return BCM_E_MEMORY;
        }

    sal_memset(bcmi_mc_group_chnl_info[unit], 0, alloc_size);

    total_trunk_groups = soc_mem_index_count(unit, TRUNK_GROUPm);
    alloc_size = sizeof(bcm_gport_t) * total_trunk_groups;

    for (i=0; i<total_mc_groups; i++) {

        ((bcmi_mc_group_chnl_info[unit])[i]).trunk_fwd_subport =
            sal_alloc(alloc_size, "trunk forwarding subport");
        if (((bcmi_mc_group_chnl_info[unit])[i]).trunk_fwd_subport == NULL) {
            (void) bcmi_ipmc_chn_repl_port_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(((bcmi_mc_group_chnl_info[unit])[i]).trunk_fwd_subport, 0,
            alloc_size);

    }
#endif
    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      bcm_th_ipmc_repl_port_attach
 * Purpose:
 *      Initialize replication per-port configuration in the runtime.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - local port number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_ipmc_repl_port_attach(int unit, bcm_port_t port)
{
    int rv = BCM_E_NONE;
    int alloc_size;
    int phy_port, mmu_port;
    int port_speed, count_width;
    uint32 regval;
    soc_info_t *si;

    if (_th_repl_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    /* Detach the IPMC REPL in case it is already attached. */
    SOC_IF_ERROR_RETURN
        (bcm_th_ipmc_repl_port_detach(unit, port));

    _th_repl_info[unit]->port_info[port] =
                    sal_alloc(sizeof(_th_repl_port_info_t), "repl port info");
    if (NULL == _th_repl_info[unit]->port_info[port]) {
        return BCM_E_MEMORY;
    }
    sal_memset(_th_repl_info[unit]->port_info[port], 0,
                                                sizeof(_th_repl_port_info_t));

    alloc_size = sizeof(int) * _th_repl_info[unit]->num_repl_groups;
    _th_repl_info[unit]->port_info[port]->intf_count =
                                sal_alloc(alloc_size, "repl port intf count");
    if (NULL == _th_repl_info[unit]->port_info[port]->intf_count) {
        sal_free(_th_repl_info[unit]->port_info[port]);
        return BCM_E_MEMORY;
    }
    sal_memset(_th_repl_info[unit]->port_info[port]->intf_count, 0, alloc_size);

    si = &SOC_INFO(unit);

    /* Initialize PORT_INITIAL_COPY_COUNT_WIDTH registers */
    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];

    if (SOC_IS_TOMAHAWK2(unit)) {
        /* Single-lane ports are allocated 2 bits, while 2 or 4
         * lane ports are allocated 3 bits. */
        count_width = (si->port_num_lanes[port] > 1) ? 1 : 0;
    } else {
        /* For ports with speed 40G and above the copy count width
         * is 3 bits (encoded value of 1) for lower speeds its
         * 2 bits (encoded value of 0).
         */
        rv = bcm_esw_port_speed_get(unit, port, &port_speed);
        if (BCM_FAILURE(rv)) {
            (void)bcm_th_ipmc_repl_port_detach(unit, port);
            return rv;
        }

        count_width = (port_speed >= 40000) ? 1 : 0;
    }

    if (!soc_feature(unit, soc_feature_skip_port_initial_copy_count)) {
        regval = 0;
        soc_reg_field_set(unit, PORT_INITIAL_COPY_COUNT_WIDTHr, &regval,
                BIT_WIDTHf, count_width);
        rv = WRITE_PORT_INITIAL_COPY_COUNT_WIDTHr(unit, port, regval);
        if (BCM_FAILURE(rv)) {
            (void)bcm_th_ipmc_repl_port_detach(unit, port);
            return rv;
        }
    }

    if (soc_feature(unit, soc_feature_channelized_switching)) {
        soc_mem_t repl_group_icc_table;
        uint32 initial_copy_count_entry[SOC_MAX_MEM_WORDS];
        SHR_BITDCL copy_count_buf[12];
        int table_count, i;

        repl_group_icc_table =
            REPL_GROUP_INITIAL_COPY_COUNT_TBL_SLICE(unit, 0);

        sal_memset(initial_copy_count_entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
        sal_memset(copy_count_buf, 0, sizeof(SHR_BITDCL) * 12);

        table_count = soc_mem_index_count(unit, repl_group_icc_table);
        soc_mem_field_set(unit, repl_group_icc_table,
            (uint32 *)&initial_copy_count_entry, INITIAL_COPY_COUNTf,
            copy_count_buf);

        for (i=0; i< table_count; i++) {
           rv = soc_mem_write(unit, repl_group_icc_table, MEM_BLOCK_ALL,
               i, &initial_copy_count_entry);
            if (BCM_FAILURE(rv)) {
                (void)bcm_th_ipmc_repl_port_detach(unit, port);
                return rv;
            }
        }
    }


    /* configure mmu port to repl aggregateId map */
    regval = 0;
    if(SOC_IS_TOMAHAWK3(unit)) {
        soc_reg_field_set(unit, MMU_RQE_REPL_PORT_AGG_MAPr, &regval,
                      L3MC_PORT_AGG_IDf, mmu_port % mmu_port_stride[unit]);
        rv = soc_reg32_set(unit, MMU_RQE_REPL_PORT_AGG_MAPr, port,0,regval);
    } else {
        if (soc_feature(unit, soc_feature_repl_port_agg_map_is_mem)) {
            mmu_dqs_repl_port_agg_map_entry_t  dqs_entry;
            sal_memset(&dqs_entry, 0, sizeof(mmu_dqs_repl_port_agg_map_entry_t));

            soc_mem_field32_set(unit,
                MMU_DQS_REPL_PORT_AGG_MAP_PIPE(unit, 0), &dqs_entry,
                L3MC_PORT_AGG_IDf, (mmu_port % mmu_port_stride[unit]));


            SOC_IF_ERROR_RETURN(
                soc_mem_write(unit, MMU_DQS_REPL_PORT_AGG_MAP_PIPE(unit, 0),
                   MEM_BLOCK_ALL, mmu_port, &dqs_entry));

        } else {

            soc_reg_field_set(unit, MMU_DQS_REPL_PORT_AGG_MAPr, &regval,
                      L3MC_PORT_AGG_IDf, mmu_port % mmu_port_stride[unit]);
            rv = soc_reg32_set(unit, MMU_DQS_REPL_PORT_AGG_MAPr, port,0,regval);
        }
    }

    if (BCM_FAILURE(rv)) {
        (void)bcm_th_ipmc_repl_port_detach(unit, port);
    }

    return rv;
}

/*
 * Function:
 *      bcm_th_ipmc_repl_port_detach
 * Purpose:
 *      Clear replication per-port configuration in the runtime.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - local port number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_ipmc_repl_port_detach(int unit, bcm_port_t port)
{
    REPL_LOCK(unit);
    if (_th_repl_info[unit] == NULL) {
        REPL_UNLOCK(unit);
        return BCM_E_INIT;
    }

    if (_th_repl_info[unit]->port_info[port] != NULL) {
        if (_th_repl_info[unit]->port_info[port]->intf_count != NULL) {
            sal_free(_th_repl_info[unit]->port_info[port]->intf_count);
            _th_repl_info[unit]->port_info[port]->intf_count = NULL;
        }
        sal_free(_th_repl_info[unit]->port_info[port]);
        _th_repl_info[unit]->port_info[port] = NULL;
    }
    REPL_UNLOCK(unit);

    if (!soc_feature(unit, soc_feature_skip_port_initial_copy_count)) {
        /* Reset the BIT_WIDTHf to 0 */
        SOC_IF_ERROR_RETURN
            (WRITE_PORT_INITIAL_COPY_COUNT_WIDTHr(unit, port, 0));
    }

    /* Reset L3MC_PORT_AGG_IDf to 0 */
    if(SOC_IS_TOMAHAWK3(unit)) {
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, MMU_RQE_REPL_PORT_AGG_MAPr, port, 0, 0));

    } else {
        if (soc_feature(unit, soc_feature_repl_port_agg_map_is_mem)) {
            mmu_dqs_repl_port_agg_map_entry_t  dqs_entry;
            sal_memset(&dqs_entry, 0, sizeof(mmu_dqs_repl_port_agg_map_entry_t));


            SOC_IF_ERROR_RETURN(
                soc_mem_write(unit, MMU_DQS_REPL_PORT_AGG_MAP_PIPE(unit, 0),
                   MEM_BLOCK_ALL, port, &dqs_entry));

        } else {

            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, MMU_DQS_REPL_PORT_AGG_MAPr, port, 0, 0));
        }
    }


    return BCM_E_NONE;
}

#ifdef BCM_TOMAHAWK3_SUPPORT
bcm_th3_repl_head_alloc_f bcm_th3_repl_head_alloc
                                = &_bcm_th_repl_head_block_alloc;
bcm_th3_repl_head_free_f bcm_th3_repl_head_free
                                = &_bcm_th_repl_head_block_free;
#endif

#endif /* BCM_TOMAHAWK_SUPPORT && INCLUDE_L3 */
