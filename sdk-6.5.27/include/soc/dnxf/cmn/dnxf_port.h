/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * DNXF PORT H
 */

#ifndef _SOC_DNXF_PORT_H_
#define _SOC_DNXF_PORT_H_

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF (Ramon) family only!"
#endif

#include <soc/types.h>
#include <soc/error.h>
#include <soc/sand/shrextend/shrextend_error.h>
#include <soc/portmod/portmod.h>

#define SOC_DNXF_PBMP_PORT_REMOVE               0
#define SOC_DNXF_PBMP_PORT_ADD                  1

typedef enum soc_dnxf_port_update_type_s
{
    soc_dnxf_port_update_type_sfi,
    soc_dnxf_port_update_type_port,
    soc_dnxf_port_update_type_all,
    soc_dnxf_port_update_type_sfi_disabled,
    soc_dnxf_port_update_type_port_disabled,
    soc_dnxf_port_update_type_all_disabled
} soc_dnxf_port_update_type_t;

typedef enum soc_dnxf_port_signal_quality_s
{
    soc_dnxf_port_signal_quality_measure_mode_disable,
    soc_dnxf_port_signal_quality_measure_mode_ser,
    soc_dnxf_port_signal_quality_measure_mode_ber,
    soc_dnxf_port_signal_quality_degrade_threshold,
    soc_dnxf_port_signal_quality_degrade_recover_threshold,
    soc_dnxf_port_signal_quality_fail_threshold,
    soc_dnxf_port_signal_quality_fail_recover_threshold,
    soc_dnxf_port_signal_quality_interval,
    soc_dnxf_port_signal_quality_action_enable
} soc_dnxf_port_signal_quality_t;

shr_error_e soc_dnxf_port_loopback_set(
    int unit,
    soc_port_t port,
    portmod_loopback_mode_t loopback);
shr_error_e soc_dnxf_port_loopback_get(
    int unit,
    soc_port_t port,
    portmod_loopback_mode_t * loopback);

shr_error_e soc_dnxf_port_disable_and_save_enable_state(
    int unit,
    soc_port_t port,
    uint32 flags,
    int *orig_enabled);
shr_error_e soc_dnxf_port_restore_enable_state(
    int unit,
    soc_port_t port,
    uint32 flags,
    int orig_enabled);

shr_error_e soc_dnxf_port_pre_enable_isolate_set(
    int unit,
    soc_port_t port);

shr_error_e soc_dnxf_port_post_enable_isolate_set(
    int unit,
    soc_port_t port);

shr_error_e soc_dnxf_port_enable_get(
    int unit,
    soc_port_t port,
    int *enable);

shr_error_e soc_dnxf_port_cable_swap_info_set(
    int unit);

shr_error_e soc_dnxf_port_cable_swap_set(
    int unit,
    soc_port_t port);

shr_error_e soc_dnxf_port_max_port_get(
    int unit,
    int *max_port);

shr_error_e soc_dnxf_port_pbmp_to_fmac_bmp_convert(
    int unit,
    bcm_pbmp_t port_bmp,
    bcm_pbmp_t * fmac_bmp);

shr_error_e soc_dnxf_port_pbmp_update(
    int unit,
    soc_port_t port,
    int enable);

shr_error_e soc_dnxf_port_ber_proj_control_set(
    int unit,
    soc_port_t port,
    bcm_port_control_t type,
    int value);

shr_error_e soc_dnxf_port_ber_proj_control_get(
    int unit,
    soc_port_t port,
    bcm_port_control_t type,
    int *value_p);

shr_error_e soc_dnxf_port_lane_map_simple_get(
    int unit,
    bcm_port_lane_to_serdes_map_t * lane_to_serdes_map);

#endif
