/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        packet.c
 * Purpose:
 * Requires:    
 */

#include "cmicsim.h"
#include <sal/core/alloc.h>

#if !defined(min)
#define min(a,b)  ((a) < (b) ? (a) : (b))
#endif


int
pcid_add_pkt(pcid_info_t *pcid_info, uint8 *pkt_data, int pkt_len, uint32 *dcbd)
{
    packet_t *newpkt, *pp;
    int      i; 
    int      word_count=8; /* FB/ER 3 + 5 words of HG + PBE */
    
    if (pcid_info->pkt_count < CPU_MAX_PACKET_QUEUE) {
        newpkt = sal_alloc(sizeof(packet_t), "pcid_add_pkt");
        if (newpkt == NULL) {
            return 1;
        }
        newpkt->length = min(pkt_len, PKT_SIZE_MAX);
        newpkt->consum = 0;
        newpkt->next = 0;

#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(pcid_info->unit)) {
            word_count = 13; /* 4 + 7 words of HG + PBE */
        }
#endif /* BCM_TRX_SUPPORT */

        for (i = 0; i < word_count; i++) {
            newpkt->dcbd[i] = dcbd[i];
        }
        memset(newpkt->data, 0, PKT_SIZE_MAX);
        memcpy(newpkt->data, pkt_data, min(pkt_len, PKT_SIZE_MAX));

        sal_mutex_take(pcid_info->pkt_mutex, sal_mutex_FOREVER);
        ++(pcid_info->pkt_count);
        /* Queue current packet on end of list */
        if (!pcid_info->pkt_list) {
            pcid_info->pkt_list = newpkt;
        } else {
            for (pp = pcid_info->pkt_list; pp->next; pp = pp->next)
                ;
            pp->next = newpkt;
        }
        sal_mutex_give(pcid_info->pkt_mutex);

        return 0;
    }

    return 1;
}

