/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ulink_pack.h
 * Purpose:     Interface to pack and unpack routines common to
 *              SDK and uKernel for:
 *              - ULINK control message 
 *
 *              This is to be shared between SDK host and uKernel.
 */


#ifndef   _SOC_SHARED_ULINK_PACK_H_
#define   _SOC_SHARED_ULINK_PACK_H_

#ifdef BCM_UKERNEL
/* Build for uKernel not SDK */
#include "sdk_typedefs.h"
#else
#include <sal/types.h>
#endif

#define UL_MAX_NUM_PORTS                  256
#define UL_PBMP_PORT_MAX                  UL_MAX_NUM_PORTS 
#define UL_PBMP_WORD_WIDTH                32
#define UL_PBMP_BYTE_WIDTH                8
#define UL_PBMP_WBIT(port)                (1U<<((port) % UL_PBMP_WORD_WIDTH))
#define UL_PBMP_WENT(port)                ((port)/UL_PBMP_WORD_WIDTH)
#define UL_PBMP_WORD_GET(bm, word)        (bm[(word)])

#define UL_PBMP_WIDTH     (((UL_PBMP_PORT_MAX + 32 - 1)/32)*32)

#define UL_PBMP_WORD_MAX      \
    ((UL_PBMP_WIDTH + UL_PBMP_WORD_WIDTH-1) / UL_PBMP_WORD_WIDTH)

#define UL_PBMP_ENTRY(bm, port)   \
    (UL_PBMP_WORD_GET(bm, UL_PBMP_WENT(port)))
#define UL_PBMP_MEMBER(bm, port)  \
    ((UL_PBMP_ENTRY(bm, port) & UL_PBMP_WBIT(port)) != 0)

#define UL_PBMP_ASSIGN(dst, src)  (dst) = (src)

typedef struct __attribute__ ((__packed__)) _soc_ulink_pbm_msg {
    uint8 flags;         /* Not used currently */
    uint8 words;         /* Number of pbits words valid in message */
    uint32 pbits[UL_PBMP_WORD_MAX]; /* PBMP of link up ports */
} soc_ulink_pbm_msg_t;

#define UC_LINK_DMA_BUFFER_LEN      sizeof(soc_ulink_pbm_msg_t)

extern uint8 *
shr_ulink_pbm_unpack(uint8 *bufp, soc_ulink_pbm_msg_t *msg);

extern uint8 *
shr_ulink_pbm_pack(uint8 *bufp, soc_ulink_pbm_msg_t *msg);

#endif /* _SOC_SHARED_OLP_PACK_H_ */


