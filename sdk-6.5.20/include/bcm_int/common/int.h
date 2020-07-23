/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    int.h
 */

#ifndef _BCM_INIT_INT_H_
#define _BCM_INIT_INT_H_

#if defined(INCLUDE_INT)
#include <soc/shared/int.h>
#include <soc/shared/int_msg.h>
#include <soc/profile_mem.h>
#include <shared/idxres_fl.h>
#include <bcm/int.h>

/*
 * Macro:
 *     _BCM_INT_ALLOC
 * Purpose:
 *      Generic memory allocation routine.
 * Parameters:
 *    _ptr_     - Pointer to allocated memory.
 *    _ptype_   - Pointer type.
 *    _size_    - Size of heap memory to be allocated.
 *    _descr_   - Information about this memory allocation.
 */
#define _BCM_INT_ALLOC(_ptr_,_ptype_,_size_,_descr_)                        \
    do {                                                                    \
        if (NULL == (_ptr_)) {                                              \
            (_ptr_) = (_ptype_ *) sal_alloc((_size_), (_descr_));           \
        }                                                                   \
        if((_ptr_) != NULL) {                                               \
            sal_memset((_ptr_), 0, (_size_));                               \
        }  else {                                                           \
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,                               \
                    (BSL_META("INT Error: Allocation failure %s\n"),        \
                     (_descr_)));                                           \
        }                                                                   \
    } while (0)

/* Internal data structure to store
 * global INT information.
 */
typedef struct _bcm_int_turnaround_info_s {
    /* flags, See BCM_INT_INT_INFO_FLAGS_XXX */
    uint32 flags;

    /* DMA Buffer used for messaging between SDK and UKERNEL. */
    uint8 *dma_buffer;

    /* Length of the DMA buffer */
    int dma_buffer_len;

    /* uController number running INT appl */
    int uc_num;

    /* CPU cos queue map INT Turnaround index */
    int cosq_turnaround_index;

    /* CPU cos queue map INT Turnaround Hop Limit index */
    int cosq_hoplimit_index;
} _bcm_int_turnaround_info_t;

/* Global data structures extern declarations */
extern _bcm_int_turnaround_info_t *int_global_info[BCM_MAX_NUM_UNITS];

/* Get the INT_TURNAROUND_INFO struct for the unit */
#define INT_TURNAROUND_INFO_GET(_u) int_global_info[(_u)]

/* Tx and Rx channel index used by INT Turnaround EAPP */
#define BCM_INT_EAPP_TX_CHANNEL 0
#define BCM_INT_EAPP_RX_CHANNEL 1

#endif /* INCLUDE_INT */
#endif  /* !_BCM_INIT_INT_H_ */
