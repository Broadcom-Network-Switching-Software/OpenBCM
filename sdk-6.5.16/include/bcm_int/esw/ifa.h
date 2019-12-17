/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:    ifa_feature.h
 */

#ifndef _BCM_INT_IFA_H_
#define _BCM_INT_IFA_H_

#if defined(INCLUDE_IFA)
#include <soc/shared/ifa.h>
#include <soc/shared/ifa_msg.h>
#include <soc/profile_mem.h>
#include <shared/idxres_fl.h>
#include <bcm/ifa.h>

/*
 * Macro:
 *     _BCM_IFA_ALLOC
 * Purpose:
 *      Generic memory allocation routine.
 * Parameters:
 *    _ptr_     - Pointer to allocated memory.
 *    _ptype_   - Pointer type.
 *    _size_    - Size of heap memory to be allocated.
 *    _descr_   - Information about this memory allocation.
 */
#define _BCM_IFA_ALLOC(_ptr_,_ptype_,_size_,_descr_)                        \
    do {                                                                    \
        if (NULL == (_ptr_)) {                                              \
            (_ptr_) = (_ptype_ *) sal_alloc((_size_), (_descr_));           \
        }                                                                   \
        if((_ptr_) != NULL) {                                               \
            sal_memset((_ptr_), 0, (_size_));                               \
        }  else {                                                           \
            LOG_ERROR(BSL_LS_BCM_IFA,                               \
                    (BSL_META("IFA Error: Allocation failure %s\n"),        \
                     (_descr_)));                                           \
        }                                                                   \
    } while (0)

/* Internal data structure to store
 * global IFA information.
 */
typedef struct _bcm_int_ifa_info_s {
    /* flags, See BCM_INT_IFA_INFO_FLAGS_XXX */
    uint32 flags;

    /* DMA Buffer used for messaging between SDK and UKERNEL. */
    uint8 *dma_buffer;

    /* Length of the DMA buffer */
    int dma_buffer_len;

    /* uController number running IFA appl */
    int uc_num;

    /* IFA status
     * 0 IFA Module Inactive
     * 1 IFA Module Active
     */
    uint32 status;

} _bcm_int_ifa_info_t;

/* Global data structures extern declarations */
extern _bcm_int_ifa_info_t *ifa_global_info[BCM_MAX_NUM_UNITS];

/* Get the IFA_INFO struct for the unit */
#define IFA_INFO_GET(_u) ifa_global_info[(_u)]

/* Minimum length of the collecotor encap
 * UDP_HDR  =  8 bytes
 * IPv4_HDR = 20 bytes
 * ETH_HDR  = 14 bytes
 * L2_CRC   =  4 bytes
 * 8 + 20 + 14 + 4 = 46
 */
#define BCM_IFA_MIN_COLLECTOR_ENCAP_LENGTH 46

/* Length of the L2 CRC field */
#define BCM_IFA_L2_CRC_LENGTH 4

/* Maximum Possible Rx packet Header length */
#define BCM_IFA_MIN_RX_PACKET_LENGTH      128

/* Max size of the IFA Rx Packet length */
#define BCM_IFA_MAX_RX_PACKET_LENGTH      1856

/* META DATA Header */
#define BCM_IFA_METADATA_HEADER_LENGTH       32

/* Tx and Rx channel index used by IFA EAPP */
#define BCM_IFA_EAPP_TX_CHANNEL 0
#define BCM_IFA_EAPP_RX_CHANNEL 1

#endif /* INCLUDE_IFA */

extern int _bcm_esw_ifa_sync(int unit);

#endif  /* !_BCM_INT_IFA_H_ */

