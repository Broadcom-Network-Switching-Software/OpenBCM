/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        knet.h
 * Purpose:     Definitions for the knet type.
 */

#ifndef   _SOC_KNET_H_
#define   _SOC_KNET_H_

#include <kcom.h>

typedef struct soc_knet_vectors_s {
    kcom_chan_t kcom;
    int (*irq_mask_set)(int unit, uint32 addr, uint32 mask);
    int (*hw_unit_get)(int unit, int inverse);
} soc_knet_vectors_t;

typedef int (*soc_knet_rx_cb_t)(kcom_msg_t *, unsigned int, void *);

extern int
soc_knet_config(soc_knet_vectors_t *vect);

extern int
soc_knet_cleanup(void);

extern int
soc_knet_init(int unit);

extern int
soc_knet_cmd_req(kcom_msg_t *kmsg, unsigned int len, unsigned int buf_size);

extern int
soc_knet_irq_mask_set(int unit, uint32 addr, uint32 mask);

extern int
soc_knet_hw_reset(int unit, int channel);

extern int
soc_knet_hw_init(int unit);

extern int
soc_knet_rx_register(soc_knet_rx_cb_t callback, void *cookie, uint32 flags);

extern int
soc_knet_rx_unregister(soc_knet_rx_cb_t callback);

extern int
soc_eth_knet_hw_config(int unit, int type, int chan, uint32 flags, uint32 value);

extern int
soc_knet_debug_pkt_set(int unit, int enable);

extern int
soc_knet_debug_pkt_get(int unit, int *value);

extern int
soc_knet_wb_cleanup(int unit);

#endif /* _SOC_KNET_H_ */
