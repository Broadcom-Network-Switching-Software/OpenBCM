/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    sum.h
 */

#ifndef _BCM_INIT_SUM_H_
#define _BCM_INIT_SUM_H_

#if defined(INCLUDE_SUM)
#include <soc/shared/sum.h>
#include <soc/shared/sum_msg.h>
#include <soc/profile_mem.h>
#include <shared/idxres_fl.h>
#include <bcm/sum.h>

/*
 * Macro:
 *     _BCM_SUM_ALLOC
 * Purpose:
 *      Generic memory allocation routine.
 * Parameters:
 *    _ptr_     - Pointer to allocated memory.
 *    _ptype_   - Pointer type.
 *    _size_    - Size of heap memory to be allocated.
 *    _descr_   - Information about this memory allocation.
 */
#define _BCM_SUM_ALLOC(_ptr_,_ptype_,_size_,_descr_)                        \
    do {                                                                    \
        if (NULL == (_ptr_)) {                                              \
            (_ptr_) = (_ptype_ *) sal_alloc((_size_), (_descr_));           \
        }                                                                   \
        if((_ptr_) != NULL) {                                               \
            sal_memset((_ptr_), 0, (_size_));                               \
        }  else {                                                           \
            LOG_ERROR(BSL_LS_BCM_SUM,                                       \
                    (BSL_META("SUM Error: Allocation failure %s\n"),        \
                     (_descr_)));                                           \
        }                                                                   \
    } while (0)

/* Internal data structure to store
 * global SUM information.
 */
typedef struct _bcm_int_sum_info_s {
    /* flags, See BCM_INT_SUM_INFO_FLAGS_XXX */
    uint32 flags;

    /* DMA Buffer used for messaging between SDK and UKERNEL. */
    uint8 *dma_buffer;

    /* Length of the DMA buffer */
    uint32 dma_buffer_len;

    /* uController number running SUM appl */
    int uc_num;

    /* SUM status
     * 0 SUM Module Inactive
     * 1 SUM Module Active
     */
    uint32 status;

    /* Host Buffer used for to sum stats information in SDK. */
    bcm_sum_stat_info_t *host_buffer;
} _bcm_int_sum_info_t;

/* Global data structures extern declarations */
extern _bcm_int_sum_info_t *sum_global_info[BCM_MAX_NUM_UNITS];

/* Get the SUM_INFO struct for the unit */
#define SUM_INFO_GET(_u) sum_global_info[(_u)]

/* Size of SUM stats info structure */
#define SUM_STAT_SIZE   sizeof(bcm_sum_stat_info_t)

#endif /* INCLUDE_SUM */

extern int _bcm_th3_sum_sync(int unit);

#endif  /* !_BCM_INIT_SUM_H_ */
