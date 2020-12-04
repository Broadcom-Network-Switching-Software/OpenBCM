/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        xaui.h
 * Purpose:     Miscellaneous XAUI functions
 */

#ifndef _SOC_XAUI_H
#define _SOC_XAUI_H

#include <shared/port.h>
#include <shared/phyconfig.h>

#include <soc/defs.h>
#include <soc/macipadr.h>
#include <soc/portmode.h>

typedef struct soc_xaui_config_s {
    uint8     preemphasis;
    uint8     idriver;
    uint8     ipredriver;
    uint8     equalizer_ctrl;
} soc_xaui_config_t;

extern int soc_xaui_err_sym_detect_get(int unit, soc_port_t port, 
                                            int *enable);

extern int soc_xaui_err_sym_detect_set(int unit, soc_port_t port, 
                                            int enable);

extern int soc_xaui_err_sym_count(int unit, soc_port_t port,
                                            int *count);

extern int soc_xaui_txbert_enable(int unit, soc_port_t port, int enable);

extern int soc_xaui_txbert_pkt_count_get(int unit, soc_port_t port, 
                                         uint32 *count);

extern int soc_xaui_txbert_byte_count_get(int unit, soc_port_t port, 
                                          uint32 *count);

extern int soc_xaui_rxbert_enable(int unit, soc_port_t port, 
                                  int enable);

extern int soc_xaui_rxbert_pkt_count_get(int unit, soc_port_t port, 
                                         uint32 *count, int *prbs_lock);

extern int soc_xaui_rxbert_byte_count_get(int unit, soc_port_t port, 
                                          uint32 *count, int *prbs_lock);

extern int soc_xaui_rxbert_bit_err_count_get(int unit, soc_port_t port, 
                                             uint32 *count, int *prbs_lock);

extern int soc_xaui_rxbert_byte_err_count_get(int unit, soc_port_t port, 
                                              uint32 *count, int *prbs_lock);

extern int soc_xaui_rxbert_pkt_err_count_get(int unit, soc_port_t port, 
                                             uint32 *count, int *prbs_lock);

extern int soc_xaui_config_set(int unit, soc_port_t port, 
                               soc_xaui_config_t *config);

extern int soc_xaui_config_get(int unit, soc_port_t port, 
                               soc_xaui_config_t *config);
#endif /* _SOC_XAUI_H */
