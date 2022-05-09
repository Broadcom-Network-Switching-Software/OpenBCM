/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * FE3200 FABRIC STATUS H
 */
 
#ifndef _SOC_FE3200_FABRIC_STATUS_H_
#define _SOC_FE3200_FABRIC_STATUS_H_

#include <soc/dcmn/fabric.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/error.h>
#include <bcm/fabric.h>

soc_error_t soc_fe3200_fabric_reachability_status_get(int unit, int moduleid, int links_max, uint32 *links_array, int *links_count);
soc_error_t soc_fe3200_fabric_link_status_all_get(int unit, int links_array_max_size, uint32* link_status, uint32* errored_token_count, int* links_array_count);
soc_error_t soc_fe3200_fabric_link_connectivity_status_get(int unit, soc_port_t link_id, bcm_fabric_link_connectivity_t *link_partner);
soc_error_t soc_fe3200_fabric_link_status_clear(int unit, soc_port_t link);
soc_error_t soc_fe3200_fabric_link_status_get(int unit, soc_port_t link_id, uint32 *link_status, uint32 *errored_token_count);

#endif

