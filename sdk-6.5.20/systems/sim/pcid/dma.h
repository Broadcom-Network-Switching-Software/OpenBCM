/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        dma.h
 * Purpose:     
 */

#ifndef   _PCID_DMA_H_
#define   _PCID_DMA_H_

#include <soc/defs.h>
#include <soc/dcb.h>
#include <sys/types.h>

#include "pcid.h"

extern uint32 pcid_dcb_fetch(pcid_info_t *pcid_info, uint32 addr, dcb_t *dcb);
extern uint32 pcid_dcb_store(pcid_info_t *pcid_info, uint32 addr, dcb_t *dcb);
extern void pcid_dma_tx_start(pcid_info_t *pcid_info, int ch);
extern void pcid_dma_cmicm_tx_start(pcid_info_t *pcid_info, int ch);
extern void pcid_dma_rx_start(pcid_info_t *pcid_info, int ch);
extern void pcid_dma_cmicm_rx_start(pcid_info_t *pcid_info, int ch);
extern void pcid_dma_rx_check(pcid_info_t *pcid_info, int chan);
extern void pcid_dma_cmicm_rx_check(pcid_info_t *pcid_info, int chan);
extern void pcid_dma_start_chan(pcid_info_t *pcid_info, int ch);
extern void pcid_dma_stat_write(pcid_info_t *pcid_info, uint32 value);
extern void pcid_dma_ctrl_write(pcid_info_t *pcid_info, uint32 value);
extern void pcid_dma_cmicm_ctrl_write(pcid_info_t *pcid_info, uint32 reg, uint32 value);
extern void pcid_dma_stop_all(pcid_info_t *pcid_info);

/* Call back function type for tx */
typedef int (*pcid_tx_cb_f)(pcid_info_t *pcid_info, uint8 *pkt_data, int eop);
extern void pcid_dma_tx_cb_set(pcid_info_t *pcid_info, pcid_tx_cb_f fun);
void _pcid_loop_tx_cb(pcid_info_t *pcid_info, dcb_t *dcb, int chan);

extern uint32
pcid_reg_read(pcid_info_t *pcid_info, uint32 addr);
extern void
pcid_reg_write(pcid_info_t *pcid_info, uint32 addr, uint32 value);
extern void
pcid_reg_or_write(pcid_info_t* pcid_info, uint32 addr, uint32 or_value);
extern void
pcid_reg_and_write(pcid_info_t* pcid_info, uint32 addr, uint32 and_value);

#endif /* _PCID_DMA_H_ */
