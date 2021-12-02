/**
 *
 * \file dnx_port_nif_ofr.h
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * NIF OFR procedures for DNX.
 *
 */

#ifndef _NIF_OFR_H_
#define _NIF_OFR_H_

#include <bcm/types.h>

#define DNX_PORT_NIF_OFR_DEFAULT_PRD_PROFILE  (0)

shr_error_e dnx_port_ofr_init(
    int unit);

shr_error_e dnx_port_ofr_power_down(
    int unit);

shr_error_e dnx_port_ofr_port_add(
    int unit,
    bcm_port_t port);

shr_error_e dnx_port_ofr_rx_mem_link_list_set(
    int unit,
    bcm_port_t port,
    int enable);

shr_error_e dnx_port_ofr_port_priority_hw_set(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config);

shr_error_e dnx_port_ofr_port_priority_config_set(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config);

shr_error_e dnx_port_ofr_port_priority_config_get(
    int unit,
    bcm_port_t port,
    bcm_port_prio_config_t * priority_config);

shr_error_e dnx_port_ofr_port_remove(
    int unit,
    bcm_port_t port);

shr_error_e dnx_port_ofr_rx_sch_context_set(
    int unit,
    bcm_port_t port);

shr_error_e dnx_port_ofr_port_enable(
    int unit,
    bcm_port_t port,
    int enable);

int dnx_ofr_port_rx_priority_to_rmc_config_set(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc_arr,
    int nof_rmcs);

#endif /* _NIF_OFR_H_ */
