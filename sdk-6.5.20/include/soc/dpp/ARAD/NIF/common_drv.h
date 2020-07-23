/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC NIF COMMON DRV H
 */
 
#ifndef _SOC_NIF_COMMON_DRV_H_
#define _SOC_NIF_COMMON_DRV_H_

#include <soc/error.h>


#define MAC_MODE_INDEP                  0
#define MAC_MODE_AGGR                   1

#define NIF_AVERAGE_IPG_DEFAULT         12
#define NIF_AVERAGE_IPG_HIGIG           8
#define NIF_TX_PREAMBLE_LENGTH_DEFAULT  8
#define NIF_PACKET_MAX_SIZE             (16*1024-1)

#define NIF_XMAC_SPEED_10     0x0
#define NIF_XMAC_SPEED_100    0x1
#define NIF_XMAC_SPEED_1000   0x2
#define NIF_XMAC_SPEED_2500   0x3
#define NIF_XMAC_SPEED_10000  0x4


soc_error_t soc_common_drv_bypass_mode_set(int unit, soc_port_t port, uint32 bypass_mode);
soc_error_t soc_common_drv_mac_mode_reg_hdr_set(int unit, soc_port_t port, soc_reg_t mac_mode_reg, uint32 *reg_val);
soc_error_t soc_common_drv_fast_port_set(int unit, soc_port_t port, int is_ilkn, int turn_on);
soc_error_t soc_common_link_status_clear(int unit, soc_port_t port);
soc_error_t soc_common_link_status_get(int unit, soc_port_t port, int *is_latch_down);
soc_error_t soc_common_drv_core_port_mode_get(int unit, soc_port_t port, uint32 *core_port_mode);
soc_error_t soc_common_drv_phy_port_mode_get(int unit, soc_port_t port, uint32 *phy_port_mode);

#endif 
