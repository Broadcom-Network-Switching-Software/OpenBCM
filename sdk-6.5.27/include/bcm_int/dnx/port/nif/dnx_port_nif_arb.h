/**
 *
 * \file dnx_port_nif_arb.h
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * NIF Arbiter procedures for DNX.
 *
 */

#ifndef _NIF_ARB_H_
#define _NIF_ARB_H_

#include <bcm/types.h>
#include <bcm/stat.h>

#define ARB_RX_DIRECTION (1 << 0)
#define ARB_TX_DIRECTION (1 << 1)

typedef enum
{
    DNX_PORT_ARB_MAP_DESTINATION_MACSEC_0 = 0,

    DNX_PORT_ARB_MAP_DESTINATION_MACSEC_1 = 1,

    DNX_PORT_ARB_MAP_DESTINATION_BYPASS = 2,

    DNX_PORT_ARB_MAP_DESTINATION_NOF
} dnx_port_map_destination_e;

shr_error_e dnx_port_arb_init(
    int unit);

shr_error_e dnx_port_arb_power_down(
    int unit);

shr_error_e dnx_port_arb_port_add(
    int unit,
    bcm_port_t logical_port);

shr_error_e dnx_port_arb_port_remove(
    int unit,
    bcm_port_t logical_port);

shr_error_e dnx_port_arb_calendar_set(
    int unit,
    bcm_pbmp_t calendars);

shr_error_e dnx_port_arb_shadow_calendar_build(
    int unit,
    bcm_pbmp_t calendars);

shr_error_e dnx_port_arb_calendar_switch_active_id(
    int unit,
    bcm_pbmp_t calendars);

shr_error_e dnx_port_arb_tx_tmac_link_list_set(
    int unit,
    bcm_port_t logical_port,
    int enable);
/**
 * \brief - switch to new linked list (G.HAO speed change)
 */
shr_error_e dnx_port_arb_tx_tmac_link_list_switch(
    int unit,
    bcm_port_t logical_port);

shr_error_e dnx_port_arb_tx_sif_port_add(
    int unit,
    bcm_stat_stif_source_t source,
    bcm_port_t logical_port);

shr_error_e dnx_port_arb_port_enable(
    int unit,
    bcm_port_t logical_port,
    uint32 direction,
    int enable);

shr_error_e dnx_port_arb_credits_init(
    int unit,
    bcm_port_t logical_port);

shr_error_e dnx_port_arb_pm_port_reset(
    int unit,
    bcm_port_t logical_port,
    uint32 direction,
    int enable);

shr_error_e dnx_port_arb_tmac_credits_set(
    int unit,
    bcm_port_t logical_port,
    int enable);

shr_error_e dnx_port_arb_pm_credits_set(
    int unit,
    bcm_port_t logical_port,
    int credits_val);

shr_error_e dnx_port_arb_tmac_start_tx_threshold_set(
    int unit,
    bcm_port_t logical_port);

shr_error_e dnx_port_arb_map_destination_set(
    int unit,
    bcm_port_t logical_port,
    dnx_port_map_destination_e mapping_type);

#endif /* _NIF_ARB_H_ */
