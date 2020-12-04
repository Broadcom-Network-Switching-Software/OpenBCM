/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        soc/shared/dcbformats/type39.h
 * Purpose:     Define dma control block (DCB) format for a type39 DCB
 *              used by the 88690 (Jericho 2)
 *
 *              This file is shared between the SDK and the embedded applications.
 */

#ifndef _SOC_SHARED_DCBFORMATS_TYPE39_H
#define _SOC_SHARED_DCBFORMATS_TYPE39_H

/*
 * DMA Control Block - Type 39
 * Used on 88690 devices
 * 4 words
 */
typedef struct {
    uint32  addr_lo;                    /* T36.0: physical address, lower 32 bits */
    uint32  addr_hi;                    /* T36.0: physical address, upper 32 bits */

#ifdef  LE_HOST
    uint32  c_count:16,                 /* Requested byte count */
            c_chain:1,                  /* Chaining */
            c_sg:1,                     /* Scatter Gather */
            c_reload:1,                 /* Reload */
            c_hg:1,                     /* Higig (TX) */
            :2,                         /* reserved */
            c_purge:1,                  /* Purge packet (TX) */
            c_intr:2,                   /* Desc Intr Mode */
            c_desc:4,                   /* descriptors remaining */
            c_desc_status:1,            /* disable descriptor status write */
            :2;                         /* reserved */
#else
    uint32  :2,                         /* resered */
            c_desc_status:1,            /* disable descriptor status write */
            c_desc:4,                   /* descriptors remaining */
            c_intr:2,                   /* Desc Intr Mode */
            c_purge:1,                  /* Purge packet (TX) */
            :2,                         /* reserved */
            c_hg:1,                     /* Higig (TX) */
            c_reload:1,                 /* Reload */
            c_sg:1,                     /* Scatter Gather */
            c_chain:1,                  /* Chaining */
            c_count:16;                 /* Requested byte count */
#endif  /* LE_HOST */

#ifdef  LE_HOST
    uint32  count:16,                   /* Transferred byte count */
            end:1,                      /* End bit (RX) */
            start:1,                    /* Start bit (RX) */
            error:1,                    /* Cell Error (RX) */
            ecc_error:1,                /* packet ECC Error (RX) */
            :11,                        /* Reserved */
            done:1;                     /* Descriptor Done */
#else
    uint32  done:1,                     /* Descriptor Done */
            :11,                        /* Reserved */
            ecc_error:1,                /* Packet ECC Error (RX) */
            error:1,                    /* Cell Error (RX) */
            start:1,                    /* Start bit (RX) */
            end:1,                      /* End bit (RX) */
            count:16;                   /* Transferred byte count */
#endif
} dcb39_t;
#endif /* _SOC_SHARED_DCBFORMATS_TYPE39_H */
