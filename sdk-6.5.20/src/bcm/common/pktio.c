/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/pktio.h>
#include <sal/core/libc.h>

#define PKTIO_RXPMD_SIZE    (96)
#define PKTIO_TX_MH_SIZE    (32)

void
bcm_pktio_pmd_t_init(bcm_pktio_pmd_t *pmd)
{
    if (pmd != NULL) {
        pmd->rxpmd = pmd->data;
        pmd->txpmd = pmd->rxpmd + PKTIO_RXPMD_SIZE / 4;
        pmd->higig = pmd->txpmd + PKTIO_TX_MH_SIZE / 4;
        pmd->lbhdr = pmd->higig + PKTIO_TX_MH_SIZE / 4;
    }
}

void
bcm_pktio_filter_t_init(bcm_pktio_filter_t *filter)
{
    if (filter != NULL) {
        sal_memset(filter, 0, sizeof(bcm_pktio_filter_t));

        bcm_pktio_pmd_t_init(&(filter->m_pmd_data));
        bcm_pktio_pmd_t_init(&(filter->m_pmd_mask));
    }
}
