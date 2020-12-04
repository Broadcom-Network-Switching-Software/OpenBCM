/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SOC_DNXC_FABRIC_SOURCE_ROUTED_CELL_H
#define SOC_DNXC_FABRIC_SOURCE_ROUTED_CELL_H

#include <soc/dnxc/dnxc_defs.h>
#include <soc/dnxc/fabric.h>

#define DNXC_CELL_NOF_LINKS_IN_PATH_LINKS            (4)

typedef struct dnxc_sr_cell_link_list_s
{
    dnxc_fabric_device_type_t src_entity_type;
    dnxc_fabric_device_type_t dest_entity_type;

    soc_port_t path_links[DNXC_CELL_NOF_LINKS_IN_PATH_LINKS];
    int pipe_id;
} dnxc_sr_cell_link_list_t;

shr_error_e soc_dnxc_actual_entity_value(
    int unit,
    dnxc_fabric_device_type_t device_entity,
    soc_dnxc_device_type_actual_value_t * actual_entity);

#endif
