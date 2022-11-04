/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * DNXF TX H
 */
#ifndef _BCM_INT_DNXF_TX_H_
#define _BCM_INT_DNXF_TX_H_

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF (Ramon) family only!"
#endif

#include <sal/types.h>
#include <bcm/types.h>

int dnxf_tx_init(
    int unit);

int dnxf_tx_deinit(
    int unit);

int dnxf_pkt_dma_tx(
    int unit,
    bcm_pkt_t * tx_pkt,
    void *cookie);

#endif /*_BCM_INT_DNXF_RX_H_*/
