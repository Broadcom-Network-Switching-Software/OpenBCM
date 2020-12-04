/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RAMON FABRIC LINKS H
 */

#ifndef _SOC_RAMON_CGM_H_
#define _SOC_RAMON_CGM_H_

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF (Ramon) family only!"
#endif

#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_fabric_flow_control.h>
#include <soc/dnxc/dnxc_defs.h>
#include <soc/error.h>
#include <soc/types.h>
_shr_error_t soc_ramon_cgm_threshold_to_reg_table_get(
    int unit,
    const soc_dnxf_cgm_threshold_to_reg_binding_t ** reg_binding_table);
_shr_error_t soc_ramon_cgm_link_profile_set(
    int unit,
    int profile_id,
    uint32 flags,
    uint32 links_count,
    soc_port_t * links);
_shr_error_t soc_ramon_cgm_link_profile_get(
    int unit,
    int profile_id,
    uint32 flags,
    uint32 links_count_max,
    uint32 *links_count,
    soc_port_t * links);

_shr_error_t soc_ramon_cgm_rci_resolution_set(
    int unit,
    soc_dnxf_fabric_rci_resolution_key_t * key,
    soc_dnxf_fabric_rci_resolution_config_t * config);
_shr_error_t soc_ramon_cgm_rci_resolution_get(
    int unit,
    soc_dnxf_fabric_rci_resolution_key_t * key,
    soc_dnxf_fabric_rci_resolution_config_t * config);

#endif
