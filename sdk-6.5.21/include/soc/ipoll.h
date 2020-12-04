/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Functions for polling IRQs instead of using hardware interrupts.
 */

#ifndef _SOC_IPOLL_H
#define _SOC_IPOLL_H

typedef void (*ipoll_handler_t)(void *data);

extern int soc_ipoll_connect(int dev, ipoll_handler_t handler, void *data);
extern int soc_ipoll_disconnect(int dev);
extern int soc_ipoll_pause(int dev);
extern int soc_ipoll_continue(int dev);

#ifdef SEPARATE_PKTDMA_INTR_HANDLER
extern int soc_ipoll_pktdma_connect(int dev, ipoll_handler_t handler, void *data);
extern int soc_ipoll_pktdma_disconnect(int dev);
#endif

#endif /* _SOC_IPOLL_H */
