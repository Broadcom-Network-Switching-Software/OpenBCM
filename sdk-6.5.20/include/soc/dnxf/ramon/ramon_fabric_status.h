/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RAMON FABRIC STATUS H
 */
 
#ifndef _SOC_RAMON_FABRIC_STATUS_H_
#define _SOC_RAMON_FABRIC_STATUS_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif 

#include <soc/dnxc/fabric.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/error.h>

shr_error_e soc_ramon_fabric_link_status_clear(int unit, soc_port_t link);
shr_error_e soc_ramon_fabric_link_status_get(int unit, soc_port_t link_id, uint32 *link_status, uint32 *errored_token_count);
shr_error_e soc_ramon_fabric_reachability_status_get(int unit, int moduleid, int links_max, uint32 *links_array, int *links_count);
shr_error_e soc_ramon_fabric_link_connectivity_status_get(int unit, soc_port_t link_id, bcm_fabric_link_connectivity_t *link_partner);

#endif

