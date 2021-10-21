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
 */

#ifndef _NIF_OFR_H_
#define _NIF_OFR_H_

#include <bcm/types.h>

shr_error_e dnx_port_ofr_init(
    int unit);

shr_error_e dnx_port_ofr_power_down(
    int unit);

shr_error_e dnx_port_ofr_port_add(
    int unit,
    bcm_port_t port);

shr_error_e dnx_port_ofr_port_priority_set(
    int unit,
    bcm_port_t port,
    bcm_port_prio_config_t * priority_config);

shr_error_e dnx_port_ofr_port_remove(
    int unit,
    bcm_port_t port);

shr_error_e dnx_port_ofr_rx_mem_link_list_set(
    int unit,
    bcm_port_t port,
    int enable);

shr_error_e dnx_port_ofr_rx_sch_context_set(
    int unit,
    bcm_port_t port);

shr_error_e dnx_port_ofr_port_enable(
    int unit,
    bcm_port_t port,
    int enable);

int dnx_ofr_prd_tpid_set(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 tpid_value);

int dnx_ofr_prd_tpid_get(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 *tpid_value);

int dnx_ofr_prd_map_set(
    int unit,
    bcm_port_t port,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    uint32 priority,
    uint32 is_tdm);

int dnx_ofr_prd_map_get(
    int unit,
    bcm_port_t port,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    uint32 *priority,
    uint32 *is_tdm);

int dnx_ofr_prd_properties_set(
    int unit,
    bcm_port_t port,
    imb_prd_properties_t property,
    uint32 val);

int dnx_ofr_prd_properties_get(
    int unit,
    bcm_port_t port,
    imb_prd_properties_t property,
    uint32 *val);

#endif /* _NIF_OFR_H_ */
