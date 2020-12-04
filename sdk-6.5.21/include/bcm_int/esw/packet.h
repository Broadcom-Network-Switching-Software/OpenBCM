/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        packet.h
 */

#ifndef   _BCM_INT_PACKET_H_
#define   _BCM_INT_PACKET_H_

#include <sal/types.h>
#include <sal/core/sync.h>

#include <soc/dma.h>

typedef struct _bcm_pkt_dv_s {
    uint32 flags;
#define BCM_PKT_DV_ADD_STK_TAG    (1 << 0)
#define BCM_PKT_DV_ADD_VLAN_TAG   (1 << 1)
#define BCM_PKT_DV_DEL_VLAN_TAG   (1 << 2)
#define BCM_PKT_DV_ADD_HG_HDR     (1 << 3)

    /* We may be in a stacked system, but not add a stack tag */
#define BCM_PKT_DV_STACKED_SYSTEM (1 << 4)

    /* These are from the user */
    int      pkt_bytes;    /* Bytes in packet from user */
    int      payload_len;  /* Base payload length */
    int      payload_off;  /* Offset into packet where payload lies */
    int      l2_hdr_off;   /* Offset of L2 header in packet */
    int      pad_size;     /* Num bytes to pad if needed */
    uint16   vlan_tag;     /* VLAN tag_control (PRI + CFI + VLAN ID) */

    /* Bitmaps needed gotten from port encapsulation mode */
    pbmp_t   ether_pbm;    /* Port bitmap for nominal ports */
    pbmp_t   higig_pbm;    /* HiGig ports derived from pbmp */
    pbmp_t   allyr_pbm;    /* BCM 5632 ports */

    /* Current bit maps to be used for descriptor adds */
    pbmp_t   cur_pbm;
    pbmp_t   cur_upbm;
    pbmp_t   cur_l3pbm;

    /* Used to describe the packet and dv */
    int      ndv;          /* Number of DV pieces needed */
    dv_t     *dv;          /* Pointer to DV chain */
    uint32   dcb_flags;    /* DCB control word */
} _bcm_pkt_dv_t;


#endif	/* !_BCM_INT_PACKET_H_ */
